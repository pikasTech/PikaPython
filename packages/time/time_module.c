/* SPEC: PIKA-CAP v0.18 library.time-basic; portable time binding. */
#include "time_binding.h"
#include "pika_capability_config.h"
#include "pika_platform.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>

#if PIKA_CAPABILITY_LIBRARY_TIME_BASIC_ENABLE

static PikaStatus time_validate(const PikaBindingCall* call,
                                PikaBindingValue* result,
                                uint8_t count) {
    if (call == NULL || result == NULL || call->argument_count != count ||
        (count > 0u && call->arguments == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus time_sleep_duration(
    const PikaBindingValue* value,
    uint32_t* duration) {
    double milliseconds;
    if (value == NULL || duration == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value->kind == PIKA_BINDING_VALUE_BOOLEAN) {
        *duration = value->as.boolean != 0u ? 1000u : 0u;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_INTEGER) {
        if (value->as.integer < 0) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        if ((uint64_t)value->as.integer >
            (uint64_t)UINT32_MAX / UINT64_C(1000)) {
            return PIKA_STATUS_OVERFLOW_ERROR;
        }
        *duration = (uint32_t)value->as.integer * 1000u;
        return PIKA_STATUS_OK;
    }
    if (value->kind != PIKA_BINDING_VALUE_FLOAT) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (isnan(value->as.floating)) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (!isfinite(value->as.floating)) {
        return PIKA_STATUS_OVERFLOW_ERROR;
    }
    if (value->as.floating < 0.0) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    milliseconds = value->as.floating * 1000.0;
    if (!isfinite(milliseconds) || milliseconds > (double)UINT32_MAX) {
        return PIKA_STATUS_OVERFLOW_ERROR;
    }
    *duration = (uint32_t)ceil(milliseconds);
    return PIKA_STATUS_OK;
}

static PikaStatus time_sleep_ms_duration(
    const PikaBindingValue* value,
    uint32_t* duration) {
    if (value == NULL || duration == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value->kind != PIKA_BINDING_VALUE_INTEGER) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (value->as.integer < 0) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if ((uint64_t)value->as.integer > (uint64_t)UINT32_MAX) {
        return PIKA_STATUS_OVERFLOW_ERROR;
    }
    *duration = (uint32_t)value->as.integer;
    return PIKA_STATUS_OK;
}

static PikaStatus time_tick_argument(const PikaBindingValue* value,
                                     uint32_t* tick) {
    if (value == NULL || tick == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value->kind != PIKA_BINDING_VALUE_INTEGER) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (value->as.integer < 0 ||
        (uint64_t)value->as.integer > (uint64_t)UINT32_MAX) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    *tick = (uint32_t)value->as.integer;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_time_sleep(const PikaBindingCall* call,
                                   PikaBindingValue* result) {
    uint32_t duration;
    PikaStatus status = time_validate(call, result, 1u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = time_sleep_duration(&call->arguments[0], &duration);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = pika_platform_time_sleep_ms(duration);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_time_sleep_ms(const PikaBindingCall* call,
                                      PikaBindingValue* result) {
    uint32_t duration;
    PikaStatus status = time_validate(call, result, 1u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = time_sleep_ms_duration(&call->arguments[0], &duration);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = pika_platform_time_sleep_ms(duration);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_time_ticks_ms(const PikaBindingCall* call,
                                      PikaBindingValue* result) {
    uint32_t ticks;
    PikaStatus status = time_validate(call, result, 0u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = pika_platform_time_ticks_ms(&ticks);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    result->as.integer = (int64_t)ticks;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_time_ticks_diff(const PikaBindingCall* call,
                                        PikaBindingValue* result) {
    uint32_t new_tick;
    uint32_t old_tick;
    uint32_t difference;
    PikaStatus status = time_validate(call, result, 2u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = time_tick_argument(&call->arguments[0], &new_tick);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = time_tick_argument(&call->arguments[1], &old_tick);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    difference = new_tick - old_tick;
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    if (difference <= UINT32_C(2147483647)) {
        result->as.integer = (int64_t)difference;
    } else {
        uint32_t magnitude = UINT32_MAX - difference + 1u;
        result->as.integer = -(int64_t)magnitude;
    }
    return PIKA_STATUS_OK;
}

#endif
