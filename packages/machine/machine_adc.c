/* SPEC: PJ2026-050112 STM32 machine v0.2; ADC Python facade. */
#include "machine_adc.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "machine_pin.h"
#include "machine_platform.h"
#include "pika_capability_config.h"

#if PIKA_CAPABILITY_MACHINE_ADC_ENABLE

typedef struct {
    PikaMachineAdcConfig config;
    void* platform_handle;
    uint8_t active;
} PikaMachineAdcObject;

static PikaMachineAdcObject
    pika_machine_adc_objects[PIKA_MACHINE_ADC_OBJECT_LIMIT];

static PikaStatus pika_machine_adc_validate_call(
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

static PikaStatus pika_machine_adc_device_status(
    PikaStatus status) {
    return status == PIKA_STATUS_UNSUPPORTED_CAPABILITY
               ? PIKA_STATUS_RUNTIME_ERROR
               : status;
}

static PikaStatus pika_machine_adc_source_identifier(
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
    object =
        (const PikaBindingObject*)source->as.opaque;
    if (object->active == 0u || object->type == NULL ||
        object->type->name == NULL || object->instance == NULL ||
        strcmp(object->type->name, "Pin") != 0) {
        return PIKA_STATUS_TYPE_ERROR;
    }
    return pika_machine_pin_identifier(
        object->instance, identifier);
}

static PikaMachineAdcObject* pika_machine_adc_allocate(void) {
    uint32_t index;
    for (index = 0u;
         index < (uint32_t)PIKA_MACHINE_ADC_OBJECT_LIMIT;
         ++index) {
        if (pika_machine_adc_objects[index].active == 0u) {
            memset(&pika_machine_adc_objects[index], 0,
                   sizeof(pika_machine_adc_objects[index]));
            pika_machine_adc_objects[index].active = 1u;
            return &pika_machine_adc_objects[index];
        }
    }
    return NULL;
}

static PikaMachineAdcObject* pika_machine_adc_find(
    void* instance) {
    uint32_t index;
    for (index = 0u;
         index < (uint32_t)PIKA_MACHINE_ADC_OBJECT_LIMIT;
         ++index) {
        if (pika_machine_adc_objects[index].active != 0u &&
            instance == &pika_machine_adc_objects[index]) {
            return &pika_machine_adc_objects[index];
        }
    }
    return NULL;
}

static PikaStatus pika_machine_adc_scale_u16(
    uint32_t raw_value,
    uint8_t resolution_bits,
    uint16_t* scaled_value) {
    uint32_t maximum;
    uint32_t expanded;
    uint32_t expanded_bits;
    if (scaled_value == NULL || resolution_bits == 0u ||
        resolution_bits > 16u) {
        return PIKA_STATUS_INTERNAL_ERROR;
    }
    maximum = (1u << resolution_bits) - 1u;
    if (raw_value > maximum) {
        return PIKA_STATUS_INTERNAL_ERROR;
    }
    expanded = raw_value;
    expanded_bits = resolution_bits;
    while (expanded_bits < 16u) {
        expanded = (expanded << resolution_bits) | raw_value;
        expanded_bits += resolution_bits;
    }
    if (expanded_bits > 16u) {
        expanded >>= expanded_bits - 16u;
    }
    *scaled_value = (uint16_t)expanded;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_machine_ADC_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaMachineAdcConfig config;
    PikaMachineAdcObject* object;
    PikaStatus status =
        pika_machine_adc_validate_call(call, result, 1u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = pika_machine_adc_source_identifier(
        &call->arguments[0], &config.identifier);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (config.identifier < 0 ||
        config.identifier > (int64_t)INT32_MAX) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    object = pika_machine_adc_allocate();
    if (object == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    status = pika_machine_adc_device_status(
        pika_machine_platform_adc_open(
            &config, &object->platform_handle));
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

PikaStatus pika_binding_machine_ADC_read_u16(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaMachineAdcObject* object;
    uint32_t raw_value;
    uint16_t scaled_value;
    uint8_t resolution_bits;
    PikaStatus status =
        pika_machine_adc_validate_call(call, result, 0u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    object = pika_machine_adc_find(call->instance);
    if (object == NULL) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    status = pika_machine_adc_device_status(
        pika_machine_platform_adc_read(
            object->platform_handle, &raw_value,
            &resolution_bits));
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = pika_machine_adc_scale_u16(
        raw_value, resolution_bits, &scaled_value);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    result->as.integer = (int64_t)scaled_value;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_machine_ADC_read_uv(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaMachineAdcObject* object;
    int64_t microvolts;
    PikaStatus status =
        pika_machine_adc_validate_call(call, result, 0u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    object = pika_machine_adc_find(call->instance);
    if (object == NULL) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    status = pika_machine_adc_device_status(
        pika_machine_platform_adc_read_uv(
            object->platform_handle, &microvolts));
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (microvolts < 0) {
        return PIKA_STATUS_INTERNAL_ERROR;
    }
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    result->as.integer = microvolts;
    return PIKA_STATUS_OK;
}

void pika_binding_machine_ADC_destroy(
    void* module_context,
    void* instance) {
    PikaMachineAdcObject* object =
        pika_machine_adc_find(instance);
    (void)module_context;
    if (object == NULL) {
        return;
    }
    if (object->platform_handle != NULL) {
        (void)pika_machine_platform_adc_close(
            object->platform_handle);
    }
    memset(object, 0, sizeof(*object));
}

#endif
