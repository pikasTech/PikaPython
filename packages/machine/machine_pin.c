/* SPEC: PJ2026-050112 STM32 machine v0.2; Pin Python facade. */
#include "machine_pin.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "machine_platform.h"
#include "pika_capability_config.h"

#if PIKA_CAPABILITY_MACHINE_PIN_ENABLE

typedef struct {
    PikaMachinePinConfig config;
    void* platform_handle;
#if PIKA_CAPABILITY_BINDING_PYTHON_CALLABLE_ENABLE
    PikaBindingPythonCallable callback;
    const PikaBindingObject* receiver;
#endif
    uint8_t active;
} PikaMachinePinObject;

static PikaMachinePinObject
    pika_machine_pin_objects[PIKA_MACHINE_PIN_OBJECT_LIMIT];

static PikaStatus pika_machine_pin_validate_call(
    const PikaBindingCall* call,
    PikaBindingValue* result,
    uint8_t argument_count) {
    if (call == NULL || result == NULL ||
        call->argument_count != argument_count ||
        (argument_count > 0u && call->arguments == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus pika_machine_pin_integer(
    const PikaBindingValue* value,
    int64_t* integer) {
    if (value == NULL || integer == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value->kind == PIKA_BINDING_VALUE_INTEGER) {
        *integer = value->as.integer;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_BOOLEAN) {
        *integer = value->as.boolean != 0u ? 1 : 0;
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_ERROR;
}

static PikaStatus pika_machine_pin_optional_value(
    const PikaBindingValue* value,
    uint8_t* present,
    uint8_t* level) {
    int64_t integer;
    PikaStatus status;
    if (value == NULL || present == NULL || level == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value->kind == PIKA_BINDING_VALUE_NONE) {
        *present = 0u;
        *level = 0u;
        return PIKA_STATUS_OK;
    }
    status = pika_machine_pin_integer(value, &integer);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (integer < 0 || integer > 1) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    *present = 1u;
    *level = (uint8_t)integer;
    return PIKA_STATUS_OK;
}

static PikaStatus pika_machine_pin_validate_config(
    const PikaMachinePinConfig* config) {
    if (config == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (config->identifier < 0 ||
        config->identifier > (int64_t)INT32_MAX) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (config->mode != PIKA_MACHINE_PIN_MODE_IN &&
        config->mode != PIKA_MACHINE_PIN_MODE_OUT &&
        config->mode != PIKA_MACHINE_PIN_MODE_OPEN_DRAIN) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (config->pull != PIKA_MACHINE_PIN_PULL_NONE &&
        config->pull != PIKA_MACHINE_PIN_PULL_UP &&
        config->pull != PIKA_MACHINE_PIN_PULL_DOWN) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (config->mode == PIKA_MACHINE_PIN_MODE_OUT &&
        config->pull != PIKA_MACHINE_PIN_PULL_NONE) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (config->mode == PIKA_MACHINE_PIN_MODE_OPEN_DRAIN &&
        config->pull == PIKA_MACHINE_PIN_PULL_DOWN) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (config->mode == PIKA_MACHINE_PIN_MODE_IN &&
        config->initial_value_valid != 0u) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (config->initial_value_valid != 0u &&
        config->initial_value > 1u) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    return PIKA_STATUS_OK;
}

static int pika_machine_pin_mode_argument_valid(int64_t mode) {
    return mode == -1 ||
           mode == (int64_t)PIKA_MACHINE_PIN_MODE_IN ||
           mode == (int64_t)PIKA_MACHINE_PIN_MODE_OUT ||
           mode == (int64_t)PIKA_MACHINE_PIN_MODE_OPEN_DRAIN;
}

static int pika_machine_pin_pull_argument_valid(int64_t pull) {
    return pull == -1 ||
           pull == (int64_t)PIKA_MACHINE_PIN_PULL_UP ||
           pull == (int64_t)PIKA_MACHINE_PIN_PULL_DOWN;
}

static PikaMachinePinObject* pika_machine_pin_allocate(void) {
    uint32_t index;
    for (index = 0u;
         index < (uint32_t)PIKA_MACHINE_PIN_OBJECT_LIMIT;
         ++index) {
        if (pika_machine_pin_objects[index].active == 0u) {
            memset(&pika_machine_pin_objects[index], 0,
                   sizeof(pika_machine_pin_objects[index]));
            pika_machine_pin_objects[index].active = 1u;
            return &pika_machine_pin_objects[index];
        }
    }
    return NULL;
}

static PikaMachinePinObject* pika_machine_pin_find(void* instance) {
    uint32_t index;
    for (index = 0u;
         index < (uint32_t)PIKA_MACHINE_PIN_OBJECT_LIMIT;
         ++index) {
        if (pika_machine_pin_objects[index].active != 0u &&
            instance == &pika_machine_pin_objects[index]) {
            return &pika_machine_pin_objects[index];
        }
    }
    return NULL;
}

PikaStatus pika_machine_pin_identifier(
    void* instance,
    int64_t* identifier) {
    PikaMachinePinObject* object;
    if (identifier == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = pika_machine_pin_find(instance);
    if (object == NULL) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    *identifier = object->config.identifier;
    return PIKA_STATUS_OK;
}

static void pika_machine_pin_result_none(PikaBindingValue* result) {
    result->kind = PIKA_BINDING_VALUE_NONE;
    result->as.integer = 0;
}

PikaStatus pika_binding_machine_Pin_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaMachinePinConfig config;
    PikaMachinePinObject* object;
    int64_t mode;
    int64_t pull;
    PikaStatus status =
        pika_machine_pin_validate_call(call, result, 4u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (call->arguments[0].kind != PIKA_BINDING_VALUE_INTEGER ||
        call->arguments[1].kind != PIKA_BINDING_VALUE_INTEGER ||
        call->arguments[2].kind != PIKA_BINDING_VALUE_INTEGER) {
        return PIKA_STATUS_TYPE_ERROR;
    }
    mode = call->arguments[1].as.integer;
    pull = call->arguments[2].as.integer;
    if (!pika_machine_pin_mode_argument_valid(mode) ||
        !pika_machine_pin_pull_argument_valid(pull)) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    config.identifier = call->arguments[0].as.integer;
    config.mode = mode == -1
                      ? PIKA_MACHINE_PIN_MODE_IN
                      : (PikaMachinePinMode)mode;
    config.pull = pull == -1
                      ? PIKA_MACHINE_PIN_PULL_NONE
                      : (PikaMachinePinPull)pull;
    status = pika_machine_pin_optional_value(
        &call->arguments[3], &config.initial_value_valid,
        &config.initial_value);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = pika_machine_pin_validate_config(&config);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    object = pika_machine_pin_allocate();
    if (object == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    status = pika_machine_platform_pin_open(
        &config, &object->platform_handle);
    if (status != PIKA_STATUS_OK ||
        object->platform_handle == NULL) {
        memset(object, 0, sizeof(*object));
        return status == PIKA_STATUS_OK
                   ? PIKA_STATUS_INTERNAL_ERROR
                   : status;
    }
    object->config = config;
    result->kind = PIKA_BINDING_VALUE_OPAQUE;
    result->as.opaque = object;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_machine_Pin_method_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaMachinePinConfig config;
    PikaMachinePinObject* object;
    int64_t mode;
    int64_t pull;
    PikaStatus status =
        pika_machine_pin_validate_call(call, result, 3u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (call->arguments[0].kind != PIKA_BINDING_VALUE_INTEGER ||
        call->arguments[1].kind != PIKA_BINDING_VALUE_INTEGER) {
        return PIKA_STATUS_TYPE_ERROR;
    }
    object = pika_machine_pin_find(call->instance);
    if (object == NULL) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    mode = call->arguments[0].as.integer;
    pull = call->arguments[1].as.integer;
    if (!pika_machine_pin_mode_argument_valid(mode) ||
        !pika_machine_pin_pull_argument_valid(pull)) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    config = object->config;
    if (mode != -1) {
        config.mode = (PikaMachinePinMode)mode;
    }
    if (pull != -1) {
        config.pull = (PikaMachinePinPull)pull;
    }
    status = pika_machine_pin_optional_value(
        &call->arguments[2], &config.initial_value_valid,
        &config.initial_value);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = pika_machine_pin_validate_config(&config);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = pika_machine_platform_pin_configure(
        object->platform_handle, &config);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    object->config = config;
    pika_machine_pin_result_none(result);
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_machine_Pin_value(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaMachinePinObject* object;
    uint8_t present;
    uint8_t level;
    int current;
    PikaStatus status =
        pika_machine_pin_validate_call(call, result, 1u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    object = pika_machine_pin_find(call->instance);
    if (object == NULL) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    status = pika_machine_pin_optional_value(
        &call->arguments[0], &present, &level);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (present == 0u) {
        status = pika_machine_platform_pin_read(
            object->platform_handle, &current);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        result->kind = PIKA_BINDING_VALUE_INTEGER;
        result->as.integer = current != 0 ? 1 : 0;
        return PIKA_STATUS_OK;
    }
    if (object->config.mode == PIKA_MACHINE_PIN_MODE_IN) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    status = pika_machine_platform_pin_write(
        object->platform_handle, (int)level);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    pika_machine_pin_result_none(result);
    return PIKA_STATUS_OK;
}

static PikaStatus pika_machine_pin_write_fixed(
    const PikaBindingCall* call,
    PikaBindingValue* result,
    int level) {
    PikaMachinePinObject* object;
    PikaStatus status =
        pika_machine_pin_validate_call(call, result, 0u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    object = pika_machine_pin_find(call->instance);
    if (object == NULL) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    if (object->config.mode == PIKA_MACHINE_PIN_MODE_IN) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    status = pika_machine_platform_pin_write(
        object->platform_handle, level);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    pika_machine_pin_result_none(result);
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_machine_Pin_on(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    return pika_machine_pin_write_fixed(call, result, 1);
}

PikaStatus pika_binding_machine_Pin_off(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    return pika_machine_pin_write_fixed(call, result, 0);
}

#if PIKA_CAPABILITY_BINDING_PYTHON_CALLABLE_ENABLE

PikaStatus pika_binding_machine_Pin_irq(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaMachinePinObject* object;
    PikaStatus status =
        pika_machine_pin_validate_call(call, result, 1u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (call->receiver == NULL ||
        call->arguments[0].kind !=
            PIKA_BINDING_VALUE_CALLABLE) {
        return PIKA_STATUS_TYPE_ERROR;
    }
    object = pika_machine_pin_find(call->instance);
    if (object == NULL) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    pika_binding_callable_release(&object->callback);
    object->callback = call->arguments[0].as.callable;
    object->receiver = call->receiver;
    pika_machine_pin_result_none(result);
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_machine_Pin_trigger(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaMachinePinObject* object;
    PikaBindingValue argument;
    PikaStatus status =
        pika_machine_pin_validate_call(call, result, 0u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    object = pika_machine_pin_find(call->instance);
    if (object == NULL || object->receiver == NULL ||
        object->callback.active == 0u) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    memset(&argument, 0, sizeof(argument));
    argument.kind = PIKA_BINDING_VALUE_OPAQUE;
    argument.as.opaque = (void*)object->receiver;
    status = pika_binding_schedule(
        &object->callback, &argument, 1u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    pika_machine_pin_result_none(result);
    return PIKA_STATUS_OK;
}

#endif

void pika_binding_machine_Pin_destroy(
    void* module_context,
    void* instance) {
    PikaMachinePinObject* object = pika_machine_pin_find(instance);
    (void)module_context;
    if (object == NULL) {
        return;
    }
    if (object->platform_handle != NULL) {
        (void)pika_machine_platform_pin_close(
            object->platform_handle);
    }
#if PIKA_CAPABILITY_BINDING_PYTHON_CALLABLE_ENABLE
    pika_binding_callable_release(&object->callback);
#endif
    memset(object, 0, sizeof(*object));
}

#endif
