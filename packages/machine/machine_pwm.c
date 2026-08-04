/* SPEC: PJ2026-050112 STM32 machine v0.3; PWM Python facade. */
#include "machine_pwm.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "machine_pin.h"
#include "machine_platform.h"
#include "pika_capability_config.h"

#if PIKA_CAPABILITY_MACHINE_PWM_ENABLE

#define PIKA_MACHINE_PWM_NANOSECONDS_PER_SECOND UINT64_C(1000000000)
#define PIKA_MACHINE_PWM_MAX_FREQUENCY_HZ UINT32_C(1000000000)

typedef struct {
    void* platform_handle;
    uint8_t allocated;
} PikaMachinePwmObject;

typedef enum {
    PIKA_MACHINE_PWM_FIELD_FREQUENCY = 0,
    PIKA_MACHINE_PWM_FIELD_DUTY_U16 = 1,
    PIKA_MACHINE_PWM_FIELD_DUTY_NS = 2,
} PikaMachinePwmField;

static PikaMachinePwmObject
    pika_machine_pwm_objects[PIKA_MACHINE_PWM_OBJECT_LIMIT];

static PikaStatus pika_machine_pwm_validate_call(
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

static PikaStatus pika_machine_pwm_device_status(
    PikaStatus status) {
    return status == PIKA_STATUS_UNSUPPORTED_CAPABILITY
               ? PIKA_STATUS_RUNTIME_ERROR
               : status;
}

static PikaStatus pika_machine_pwm_integer(
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

static PikaStatus pika_machine_pwm_optional_integer(
    const PikaBindingValue* value,
    uint8_t* present,
    int64_t* integer) {
    if (value == NULL || present == NULL || integer == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value->kind == PIKA_BINDING_VALUE_NONE) {
        *present = 0u;
        *integer = 0;
        return PIKA_STATUS_OK;
    }
    *present = 1u;
    return pika_machine_pwm_integer(value, integer);
}

static PikaStatus pika_machine_pwm_source_identifier(
    const PikaBindingValue* source,
    int64_t* identifier) {
    const PikaBindingObject* object;
    if (source == NULL || identifier == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (source->kind == PIKA_BINDING_VALUE_INTEGER) {
        *identifier = source->as.integer;
        return PIKA_STATUS_OK;
    }
    if (source->kind != PIKA_BINDING_VALUE_OPAQUE ||
        source->as.opaque == NULL) {
        return PIKA_STATUS_TYPE_ERROR;
    }
    object = (const PikaBindingObject*)source->as.opaque;
    if (object->active == 0u || object->type == NULL ||
        object->type->name == NULL || object->instance == NULL ||
        strcmp(object->type->name, "Pin") != 0) {
        return PIKA_STATUS_TYPE_ERROR;
    }
    return pika_machine_pin_identifier(
        object->instance, identifier);
}

static PikaStatus pika_machine_pwm_parse_settings(
    const PikaBindingValue* arguments,
    PikaMachinePwmSettings* settings) {
    int64_t frequency;
    int64_t duty_u16;
    int64_t duty_ns;
    uint8_t frequency_present;
    uint8_t duty_u16_present;
    uint8_t duty_ns_present;
    uint64_t period_ns;
    PikaStatus status;
    if (arguments == NULL || settings == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    memset(settings, 0, sizeof(*settings));
    settings->enabled = 1u;
    status = pika_machine_pwm_optional_integer(
        &arguments[0], &frequency_present, &frequency);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = pika_machine_pwm_optional_integer(
        &arguments[1], &duty_u16_present, &duty_u16);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = pika_machine_pwm_optional_integer(
        &arguments[2], &duty_ns_present, &duty_ns);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (duty_u16_present != 0u && duty_ns_present != 0u) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (frequency_present != 0u) {
        if (frequency <= 0 ||
            frequency >
                (int64_t)PIKA_MACHINE_PWM_MAX_FREQUENCY_HZ) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        settings->frequency_present = 1u;
        settings->frequency_hz = (uint32_t)frequency;
    }
    if (duty_u16_present != 0u) {
        if (duty_u16 < 0 ||
            duty_u16 > (int64_t)UINT16_MAX) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        settings->duty_mode = PIKA_MACHINE_PWM_DUTY_U16;
        settings->duty_u16 = (uint16_t)duty_u16;
    } else if (duty_ns_present != 0u) {
        if (duty_ns < 0) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        settings->duty_mode = PIKA_MACHINE_PWM_DUTY_NS;
        settings->duty_ns = (uint64_t)duty_ns;
    }
    if (settings->frequency_present != 0u &&
        settings->duty_mode == PIKA_MACHINE_PWM_DUTY_NS) {
        period_ns = PIKA_MACHINE_PWM_NANOSECONDS_PER_SECOND /
                    (uint64_t)settings->frequency_hz;
        if (period_ns == 0u ||
            settings->duty_ns > period_ns) {
            return PIKA_STATUS_VALUE_ERROR;
        }
    }
    return PIKA_STATUS_OK;
}

static PikaMachinePwmObject* pika_machine_pwm_allocate(void) {
    uint32_t index;
    for (index = 0u;
         index < (uint32_t)PIKA_MACHINE_PWM_OBJECT_LIMIT;
         ++index) {
        if (pika_machine_pwm_objects[index].allocated == 0u) {
            memset(&pika_machine_pwm_objects[index], 0,
                   sizeof(pika_machine_pwm_objects[index]));
            pika_machine_pwm_objects[index].allocated = 1u;
            return &pika_machine_pwm_objects[index];
        }
    }
    return NULL;
}

static PikaMachinePwmObject* pika_machine_pwm_find(
    void* instance) {
    uint32_t index;
    for (index = 0u;
         index < (uint32_t)PIKA_MACHINE_PWM_OBJECT_LIMIT;
         ++index) {
        if (pika_machine_pwm_objects[index].allocated != 0u &&
            instance == &pika_machine_pwm_objects[index]) {
            return &pika_machine_pwm_objects[index];
        }
    }
    return NULL;
}

static PikaStatus pika_machine_pwm_apply(
    PikaMachinePwmObject* object,
    const PikaMachinePwmSettings* settings) {
    if (object == NULL || settings == NULL ||
        object->platform_handle == NULL) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    return pika_machine_pwm_device_status(
        pika_machine_platform_pwm_configure(
            object->platform_handle, settings));
}

static PikaStatus pika_machine_pwm_read_state(
    PikaMachinePwmObject* object,
    PikaMachinePwmState* state) {
    if (object == NULL || state == NULL ||
        object->platform_handle == NULL) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    memset(state, 0, sizeof(*state));
    return pika_machine_pwm_device_status(
        pika_machine_platform_pwm_state(
            object->platform_handle, state));
}

static PikaStatus pika_machine_pwm_update(
    const PikaBindingCall* call,
    PikaBindingValue* result,
    PikaMachinePwmField field) {
    PikaMachinePwmObject* object;
    PikaMachinePwmSettings settings;
    PikaMachinePwmState state;
    int64_t integer;
    uint8_t present;
    PikaStatus status =
        pika_machine_pwm_validate_call(call, result, 1u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    object = pika_machine_pwm_find(call->instance);
    if (object == NULL) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    status = pika_machine_pwm_optional_integer(
        &call->arguments[0], &present, &integer);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (present == 0u) {
        status = pika_machine_pwm_read_state(object, &state);
        if (status != PIKA_STATUS_OK) {
            return status;
        }
        result->kind = PIKA_BINDING_VALUE_INTEGER;
        if (field == PIKA_MACHINE_PWM_FIELD_FREQUENCY) {
            result->as.integer = state.frequency_valid != 0u
                                     ? (int64_t)state.frequency_hz
                                     : 0;
        } else if (field == PIKA_MACHINE_PWM_FIELD_DUTY_U16) {
            result->as.integer = (int64_t)state.duty_u16;
        } else {
            result->as.integer = (int64_t)state.duty_ns;
        }
        return PIKA_STATUS_OK;
    }
    memset(&settings, 0, sizeof(settings));
    settings.enabled = 1u;
    if (field == PIKA_MACHINE_PWM_FIELD_FREQUENCY) {
        if (integer <= 0 ||
            integer > (int64_t)PIKA_MACHINE_PWM_MAX_FREQUENCY_HZ) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        settings.frequency_present = 1u;
        settings.frequency_hz = (uint32_t)integer;
    } else if (field == PIKA_MACHINE_PWM_FIELD_DUTY_U16) {
        if (integer < 0 || integer > (int64_t)UINT16_MAX) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        settings.duty_mode = PIKA_MACHINE_PWM_DUTY_U16;
        settings.duty_u16 = (uint16_t)integer;
    } else {
        if (integer < 0) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        settings.duty_mode = PIKA_MACHINE_PWM_DUTY_NS;
        settings.duty_ns = (uint64_t)integer;
    }
    status = pika_machine_pwm_apply(object, &settings);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_machine_PWM_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaMachinePwmConfig config;
    PikaMachinePwmSettings settings;
    PikaMachinePwmObject* object;
    PikaStatus status =
        pika_machine_pwm_validate_call(call, result, 4u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    memset(&config, 0, sizeof(config));
    status = pika_machine_pwm_source_identifier(
        &call->arguments[0], &config.identifier);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (config.identifier < 0 ||
        config.identifier > (int64_t)INT32_MAX) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    status = pika_machine_pwm_parse_settings(
        &call->arguments[1], &settings);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    object = pika_machine_pwm_allocate();
    if (object == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    status = pika_machine_pwm_device_status(
        pika_machine_platform_pwm_open(
            &config, &object->platform_handle));
    if (status != PIKA_STATUS_OK ||
        object->platform_handle == NULL) {
        if (object->platform_handle != NULL) {
            (void)pika_machine_platform_pwm_close(
                object->platform_handle);
        }
        memset(object, 0, sizeof(*object));
        return status == PIKA_STATUS_OK
                   ? PIKA_STATUS_INTERNAL_ERROR
                   : status;
    }
    status = pika_machine_pwm_apply(object, &settings);
    if (status != PIKA_STATUS_OK) {
        (void)pika_machine_platform_pwm_close(
            object->platform_handle);
        memset(object, 0, sizeof(*object));
        return status;
    }
    result->kind = PIKA_BINDING_VALUE_OPAQUE;
    result->as.opaque = object;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_machine_PWM_method_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaMachinePwmObject* object;
    PikaMachinePwmSettings settings;
    PikaStatus status =
        pika_machine_pwm_validate_call(call, result, 3u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    object = pika_machine_pwm_find(call->instance);
    if (object == NULL) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    status = pika_machine_pwm_parse_settings(
        call->arguments, &settings);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = pika_machine_pwm_apply(object, &settings);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_machine_PWM_freq(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    return pika_machine_pwm_update(
        call, result, PIKA_MACHINE_PWM_FIELD_FREQUENCY);
}

PikaStatus pika_binding_machine_PWM_duty_u16(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    return pika_machine_pwm_update(
        call, result, PIKA_MACHINE_PWM_FIELD_DUTY_U16);
}

PikaStatus pika_binding_machine_PWM_duty_ns(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    return pika_machine_pwm_update(
        call, result, PIKA_MACHINE_PWM_FIELD_DUTY_NS);
}

PikaStatus pika_binding_machine_PWM_deinit(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaMachinePwmObject* object;
    PikaStatus status =
        pika_machine_pwm_validate_call(call, result, 0u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    object = pika_machine_pwm_find(call->instance);
    if (object == NULL) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    status = pika_machine_pwm_device_status(
        pika_machine_platform_pwm_disable(
            object->platform_handle));
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

void pika_binding_machine_PWM_destroy(
    void* module_context,
    void* instance) {
    PikaMachinePwmObject* object =
        pika_machine_pwm_find(instance);
    (void)module_context;
    if (object == NULL) {
        return;
    }
    if (object->platform_handle != NULL) {
        (void)pika_machine_platform_pwm_close(
            object->platform_handle);
    }
    memset(object, 0, sizeof(*object));
}

#endif
