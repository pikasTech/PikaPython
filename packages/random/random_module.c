/* SPEC: PJ2026-050111 portable packages v0.2; deterministic random binding. */
#include "_random_binding.h"
#include "pika_capability_config.h"

#include <stddef.h>
#include <stdint.h>

#if PIKA_CAPABILITY_LIBRARY_RANDOM_ENABLE

static uint64_t pika_random_state = UINT64_C(0x9e3779b97f4a7c15);

static PikaStatus random_validate(const PikaBindingCall* call,
                                  PikaBindingValue* result,
                                  uint8_t count) {
    if (call == NULL || result == NULL || call->argument_count != count ||
        (count > 0u && call->arguments == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus random_number(const PikaBindingValue* value,
                                double* result) {
    if (value == NULL || result == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value->kind == PIKA_BINDING_VALUE_INTEGER) {
        *result = (double)value->as.integer;
        return PIKA_STATUS_OK;
    }
    if (value->kind == PIKA_BINDING_VALUE_FLOAT) {
        *result = value->as.floating;
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}

static uint64_t random_next(void) {
    uint64_t value = pika_random_state;
    value ^= value >> 12;
    value ^= value << 25;
    value ^= value >> 27;
    pika_random_state = value;
    return value * UINT64_C(2685821657736338717);
}

static uint64_t random_bounded(uint64_t bound) {
    uint64_t value;
    uint64_t threshold;
    if (bound == 0u) {
        return random_next();
    }
    threshold = (UINT64_C(0) - bound) % bound;
    do {
        value = random_next();
    } while (value < threshold);
    return value % bound;
}

static int64_t random_bits_to_integer(uint64_t bits) {
    if (bits <= (uint64_t)INT64_MAX) {
        return (int64_t)bits;
    }
    {
        uint64_t magnitude = UINT64_MAX - bits + UINT64_C(1);
        if (magnitude == (UINT64_C(1) << 63)) {
            return INT64_MIN;
        }
        return -(int64_t)magnitude;
    }
}

static int64_t random_add_offset(int64_t start, uint64_t offset) {
    if (start >= 0) {
        return start + (int64_t)offset;
    }
    {
        uint64_t magnitude = (uint64_t)(-(start + 1)) + UINT64_C(1);
        if (offset < magnitude) {
            uint64_t remainder = magnitude - offset;
            if (remainder == (UINT64_C(1) << 63)) {
                return INT64_MIN;
            }
            return -(int64_t)remainder;
        }
        return (int64_t)(offset - magnitude);
    }
}

static int64_t random_subtract_offset(int64_t start, uint64_t offset) {
    if (start <= 0) {
        uint64_t magnitude = (uint64_t)(-(start + 1)) + UINT64_C(1);
        uint64_t result_magnitude = magnitude + offset;
        if (result_magnitude == (UINT64_C(1) << 63)) {
            return INT64_MIN;
        }
        return -(int64_t)result_magnitude;
    }
    if (offset <= (uint64_t)start) {
        return start - (int64_t)offset;
    }
    return -(int64_t)(offset - (uint64_t)start);
}

PikaStatus pika_binding__random_seed(const PikaBindingCall* call,
                                     PikaBindingValue* result) {
    uint64_t value;
    PikaStatus status = random_validate(call, result, 1u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    value = (uint64_t)call->arguments[0].as.integer;
    value += UINT64_C(0x9e3779b97f4a7c15);
    value = (value ^ (value >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    value = (value ^ (value >> 27)) * UINT64_C(0x94d049bb133111eb);
    value ^= value >> 31;
    pika_random_state = value == 0u
                            ? UINT64_C(0x2545f4914f6cdd1d)
                            : value;
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__random_random(const PikaBindingCall* call,
                                       PikaBindingValue* result) {
    PikaStatus status = random_validate(call, result, 0u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    result->kind = PIKA_BINDING_VALUE_FLOAT;
    result->as.floating =
        (double)(random_next() >> 11) * (1.0 / 9007199254740992.0);
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__random_randint(const PikaBindingCall* call,
                                        PikaBindingValue* result) {
    int64_t lower;
    int64_t upper;
    uint64_t span;
    uint64_t offset;
    PikaStatus status = random_validate(call, result, 2u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    lower = call->arguments[0].as.integer;
    upper = call->arguments[1].as.integer;
    if (lower > upper) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    span = (uint64_t)upper - (uint64_t)lower + UINT64_C(1);
    offset = random_bounded(span);
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    result->as.integer = span == 0u
                             ? random_bits_to_integer(offset)
                             : random_add_offset(lower, offset);
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__random_randrange(const PikaBindingCall* call,
                                          PikaBindingValue* result) {
    int64_t start;
    int64_t stop;
    int64_t step;
    uint64_t distance;
    uint64_t magnitude;
    uint64_t count;
    uint64_t offset;
    PikaStatus status = random_validate(call, result, 3u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    start = call->arguments[0].as.integer;
    stop = call->arguments[1].as.integer;
    step = call->arguments[2].as.integer;
    if (step == 0) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (step > 0) {
        if (start >= stop) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        distance = (uint64_t)stop - (uint64_t)start;
        magnitude = (uint64_t)step;
        count = UINT64_C(1) + (distance - UINT64_C(1)) / magnitude;
        offset = random_bounded(count) * magnitude;
        result->as.integer = random_add_offset(start, offset);
    } else {
        if (start <= stop) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        distance = (uint64_t)start - (uint64_t)stop;
        magnitude = (uint64_t)(-(step + 1)) + UINT64_C(1);
        count = UINT64_C(1) + (distance - UINT64_C(1)) / magnitude;
        offset = random_bounded(count) * magnitude;
        result->as.integer = random_subtract_offset(start, offset);
    }
    result->kind = PIKA_BINDING_VALUE_INTEGER;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__random_uniform(const PikaBindingCall* call,
                                        PikaBindingValue* result) {
    double lower;
    double upper;
    double unit;
    PikaStatus status = random_validate(call, result, 2u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = random_number(&call->arguments[0], &lower);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = random_number(&call->arguments[1], &upper);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    unit = (double)(random_next() >> 11) *
           (1.0 / 9007199254740992.0);
    result->kind = PIKA_BINDING_VALUE_FLOAT;
    result->as.floating = lower + (upper - lower) * unit;
    return PIKA_STATUS_OK;
}

#endif
