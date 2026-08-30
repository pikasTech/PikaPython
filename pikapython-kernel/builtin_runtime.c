/* SPEC: PJ2026-0501; embedded builtin execution. */
#include "runtime_internal.h"

#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if PIKA_BUILTIN_RUNTIME_ENABLE

#define PIKA_BUILTIN_TEXT_LIMIT 1024u
#ifndef PIKA_BUILTIN_HELP_TOPIC_LIMIT
#define PIKA_BUILTIN_HELP_TOPIC_LIMIT 63u
#endif

typedef struct {
    char bytes[PIKA_BUILTIN_TEXT_LIMIT];
    uint32_t length;
} PikaTextBuffer;

static PikaRuntimeValue argument_value(const PikaRuntimeContext* context,
                                       const PikaRuntimeFrame* frame,
                                       uint8_t base,
                                       uint32_t index) {
    return pika_runtime_read_value(
        context, frame->value_base + (uint32_t)base + index);
}

static PikaStatus write_result(PikaRuntimeContext* context,
                               const PikaRuntimeFrame* frame,
                               uint8_t slot,
                               PikaRuntimeValue value) {
    return pika_runtime_write_value(
        context, frame->value_base + slot, value);
}

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_LEN_ENABLE
static PikaRuntimeValue integer_value(int64_t value) {
    PikaRuntimeValue result;
    memset(&result, 0, sizeof(result));
    result.kind = PIKA_RUNTIME_VALUE_INTEGER;
    result.as.integer = value;
    return result;
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE
static PikaRuntimeValue boolean_value(int value) {
    PikaRuntimeValue result = integer_value(value != 0 ? 1 : 0);
    result.kind = PIKA_RUNTIME_VALUE_BOOLEAN;
    return result;
}
#endif

static PikaRuntimeValue none_value(void) {
    PikaRuntimeValue result;
    memset(&result, 0, sizeof(result));
    result.kind = PIKA_RUNTIME_VALUE_NONE;
    return result;
}

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE
static PikaStatus integer_argument(PikaRuntimeValue value,
                                   int64_t* result) {
    if (value.kind != PIKA_RUNTIME_VALUE_INTEGER &&
        value.kind != PIKA_RUNTIME_VALUE_BOOLEAN) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    *result = value.as.integer;
    return PIKA_STATUS_OK;
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE

static uint64_t integer_magnitude(int64_t value) {
    if (value < 0) return (uint64_t)(-(value + 1)) + 1u;
    return (uint64_t)value;
}

static uint64_t modulo_add(uint64_t left,
                           uint64_t right,
                           uint64_t modulus) {
    if (left >= modulus - right) return left - (modulus - right);
    return left + right;
}

static uint64_t modulo_multiply(uint64_t left,
                                uint64_t right,
                                uint64_t modulus) {
    uint64_t result = 0u;
    while (right != 0u) {
        if ((right & 1u) != 0u) {
            result = modulo_add(result, left, modulus);
        }
        right >>= 1;
        if (right != 0u) left = modulo_add(left, left, modulus);
    }
    return result;
}

static uint64_t modulo_residue(int64_t value, uint64_t modulus) {
    uint64_t remainder = integer_magnitude(value) % modulus;
    if (value < 0 && remainder != 0u) return modulus - remainder;
    return remainder;
}

static PikaStatus integer_power_modulo(int64_t base,
                                       int64_t exponent,
                                       int64_t modulus,
                                       int64_t* result) {
    uint64_t modulus_size;
    uint64_t factor;
    uint64_t output;
    if (exponent < 0 || modulus == 0) return PIKA_STATUS_TYPE_MISMATCH;
    modulus_size = integer_magnitude(modulus);
    output = 1u % modulus_size;
    factor = modulo_residue(base, modulus_size);
    while (exponent != 0) {
        if ((exponent & 1) != 0) {
            output = modulo_multiply(output, factor, modulus_size);
        }
        exponent >>= 1;
        if (exponent != 0) {
            factor = modulo_multiply(factor, factor, modulus_size);
        }
    }
    if (modulus < 0 && output != 0u) {
        *result = -(int64_t)(modulus_size - output);
    } else {
        *result = (int64_t)output;
    }
    return PIKA_STATUS_OK;
}

#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE
static PikaStatus text_argument(PikaRuntimeContext* context,
                                PikaRuntimeValue value,
                                const char** bytes,
                                uint32_t* length) {
    PikaObjectSlot* slot;
    if (value.kind != PIKA_RUNTIME_VALUE_STRING &&
        value.kind != PIKA_RUNTIME_VALUE_TYPE) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    slot = pika_runtime_object(context, value);
    if (slot == NULL || (slot->length != 0u && slot->data == NULL)) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    *bytes = (const char*)slot->data;
    *length = slot->length;
    return PIKA_STATUS_OK;
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_LEN_ENABLE
static int is_sequence_kind(PikaRuntimeValueKind kind) {
    return kind == PIKA_RUNTIME_VALUE_LIST ||
           kind == PIKA_RUNTIME_VALUE_TUPLE ||
           kind == PIKA_RUNTIME_VALUE_SET;
}

static int is_iterable_kind(PikaRuntimeValueKind kind) {
    return is_sequence_kind(kind) ||
           kind == PIKA_RUNTIME_VALUE_STRING ||
           kind == PIKA_RUNTIME_VALUE_BYTES ||
           kind == PIKA_RUNTIME_VALUE_BYTEARRAY ||
           kind == PIKA_RUNTIME_VALUE_RANGE ||
           kind == PIKA_RUNTIME_VALUE_ITERATOR;
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE
static PikaStatus text_collector(void* context,
                                 const char* bytes,
                                 uint32_t length) {
    PikaTextBuffer* buffer = (PikaTextBuffer*)context;
    if (buffer == NULL || (bytes == NULL && length != 0u) ||
        length > PIKA_BUILTIN_TEXT_LIMIT - buffer->length - 1u) {
        return PIKA_STATUS_OUTPUT_ERROR;
    }
    memcpy(&buffer->bytes[buffer->length], bytes, length);
    buffer->length += length;
    buffer->bytes[buffer->length] = '\0';
    return PIKA_STATUS_OK;
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE
static PikaStatus value_text(PikaRuntimeContext* context,
                             PikaRuntimeValue value,
                             int representation,
                             PikaRuntimeValue* result) {
    PikaTextBuffer buffer;
    PikaOutput output;
    const PikaOutput* original;
    PikaStatus status;
    memset(&buffer, 0, sizeof(buffer));
    output.write = text_collector;
    output.context = &buffer;
    original = context->output;
    context->output = &output;
    status = pika_runtime_write_value_text(context, value, representation, 0);
    context->output = original;
    if (status != PIKA_STATUS_OK) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    return pika_runtime_create_text(
        context, PIKA_RUNTIME_VALUE_STRING, buffer.bytes,
        buffer.length, result);
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE
static PikaStatus sequence_values(PikaRuntimeContext* context,
                                  PikaRuntimeValue value,
                                  PikaRuntimeValue** values,
                                  uint32_t* count) {
    PikaObjectSlot* slot;
    if (!is_iterable_kind(value.kind)) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    *values = NULL;
    *count = 0u;
    if (value.kind == PIKA_RUNTIME_VALUE_ITERATOR) {
        PikaIteratorData* state;
        PikaObjectSlot* source_slot;
        PikaRuntimeValue source;
        uint32_t source_length;
        uint32_t remaining;
        PikaStatus status;
        slot = pika_runtime_object(context, value);
        if (slot == NULL || slot->data == NULL) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        state = (PikaIteratorData*)slot->data;
        if (state->source_index >= context->objects->count) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        source_slot = &context->objects->slots[state->source_index];
        source.kind = (PikaRuntimeValueKind)source_slot->kind;
        source.as.object_index = state->source_index;
        if (source.kind == PIKA_RUNTIME_VALUE_RANGE) {
            status = pika_runtime_range_length(
                context, source, &source_length);
        } else {
            source_length = source_slot->length;
            status = PIKA_STATUS_OK;
        }
        if (status != PIKA_STATUS_OK) return status;
        if (state->position >= source_length) return PIKA_STATUS_OK;
        remaining = source_length - state->position;
        *values = (PikaRuntimeValue*)malloc(
            (size_t)remaining * sizeof(PikaRuntimeValue));
        if (*values == NULL) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        for (uint32_t index = 0u; index < remaining; ++index) {
            status = pika_runtime_sequence_item(
                context, source,
                (int64_t)(state->position + index), &(*values)[index]);
            if (status != PIKA_STATUS_OK) {
                free(*values);
                *values = NULL;
                return status;
            }
        }
        state->position = source_length;
        *count = remaining;
        return PIKA_STATUS_OK;
    }
    slot = pika_runtime_object(context, value);
    if (slot == NULL || (slot->length != 0u && slot->data == NULL)) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    *count = slot->length;
    if (value.kind == PIKA_RUNTIME_VALUE_RANGE) {
        PikaStatus status = pika_runtime_range_length(
            context, value, count);
        if (status != PIKA_STATUS_OK) return status;
        if (*count == 0u) return PIKA_STATUS_OK;
        *values = (PikaRuntimeValue*)malloc(
            (size_t)*count * sizeof(PikaRuntimeValue));
        if (*values == NULL) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        for (uint32_t index = 0u; index < *count; ++index) {
            status = pika_runtime_sequence_item(
                context, value, (int64_t)index, &(*values)[index]);
            if (status != PIKA_STATUS_OK) {
                free(*values);
                *values = NULL;
                return status;
            }
        }
        return PIKA_STATUS_OK;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_STRING ||
        value.kind == PIKA_RUNTIME_VALUE_BYTES ||
        value.kind == PIKA_RUNTIME_VALUE_BYTEARRAY) {
        if (slot->length == 0u) return PIKA_STATUS_OK;
        *values = (PikaRuntimeValue*)malloc(
            (size_t)slot->length * sizeof(PikaRuntimeValue));
        if (*values == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
        for (uint32_t index = 0u; index < slot->length; ++index) {
            if (value.kind == PIKA_RUNTIME_VALUE_STRING) {
                char byte = ((const char*)slot->data)[index];
                PikaStatus status = pika_runtime_create_text(
                    context, PIKA_RUNTIME_VALUE_STRING, &byte, 1u,
                    &(*values)[index]);
                if (status != PIKA_STATUS_OK) {
                    free(*values);
                    return status;
                }
            } else {
                (*values)[index] = integer_value(
                    (unsigned char)((const char*)slot->data)[index]);
            }
        }
        return PIKA_STATUS_OK;
    }
    if (slot->length == 0u) return PIKA_STATUS_OK;
    *values = (PikaRuntimeValue*)malloc(
        (size_t)slot->length * sizeof(PikaRuntimeValue));
    if (*values == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    memcpy(*values, slot->data,
           (size_t)slot->length * sizeof(PikaRuntimeValue));
    return PIKA_STATUS_OK;
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE
static PikaStatus make_sequence(PikaRuntimeContext* context,
                                PikaRuntimeValueKind kind,
                                const PikaRuntimeValue* values,
                                uint32_t count,
                                PikaRuntimeValue* result) {
    return pika_runtime_create_sequence(context, kind, values, count, result);
}

static PikaStatus create_map(PikaRuntimeContext* context,
                             const PikaMapEntry* entries,
                             uint32_t count,
                             PikaRuntimeValue* result) {
    uint32_t object_index;
    void* data = NULL;
    PikaStatus status = pika_runtime_create_object(
        context, PIKA_RUNTIME_VALUE_DICT, count,
        (uint32_t)sizeof(PikaMapEntry), &object_index, &data);
    if (status != PIKA_STATUS_OK) return status;
    if (count != 0u) memcpy(data, entries, count * sizeof(PikaMapEntry));
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_RUNTIME_VALUE_DICT;
    result->as.object_index = object_index;
    return PIKA_STATUS_OK;
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE

static PikaStatus builtin_abs(PikaRuntimeValue value,
                              PikaRuntimeValue* result) {
    if (value.kind == PIKA_RUNTIME_VALUE_FLOAT) {
        memset(result, 0, sizeof(*result));
        result->kind = PIKA_RUNTIME_VALUE_FLOAT;
        result->as.floating = fabs(value.as.floating);
        return PIKA_STATUS_OK;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_INTEGER ||
        value.kind == PIKA_RUNTIME_VALUE_BOOLEAN) {
        if (value.as.integer == INT64_MIN) return PIKA_STATUS_INVALID_OPERAND;
        *result = integer_value(
            value.as.integer < 0 ? -value.as.integer : value.as.integer);
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}

static PikaStatus builtin_all_any(PikaRuntimeContext* context,
                                  PikaRuntimeValue value,
                                  int want_all,
                                  PikaRuntimeValue* result) {
    PikaRuntimeValue* values = NULL;
    uint32_t count = 0u;
    PikaStatus status = sequence_values(context, value, &values, &count);
    int answer;
    if (status != PIKA_STATUS_OK) return status;
    answer = want_all ? 1 : 0;
    for (uint32_t index = 0u; index < count; ++index) {
        int truth = pika_runtime_value_truthy(context, values[index]);
        if ((want_all && !truth) || (!want_all && truth)) {
            answer = want_all ? 0 : 1;
            break;
        }
    }
    free(values);
    *result = boolean_value(answer);
    return PIKA_STATUS_OK;
}

static PikaStatus builtin_integer_text(PikaRuntimeValue value,
                                       int base,
                                       const char* prefix,
                                       PikaRuntimeContext* context,
                                       PikaRuntimeValue* result) {
    int64_t integer;
    char buffer[80];
    char reversed[64];
    static const char digits[] = "0123456789abcdef";
    uint32_t count = 0u;
    uint32_t output = 0u;
    uint64_t magnitude;
    if (integer_argument(value, &integer) != PIKA_STATUS_OK) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    magnitude = integer < 0 ? (uint64_t)(-(integer + 1)) + 1u
                            : (uint64_t)integer;
    do {
        reversed[count++] = digits[magnitude % (uint32_t)base];
        magnitude /= (uint32_t)base;
    } while (magnitude != 0u);
    if (integer < 0) buffer[output++] = '-';
    while (*prefix != '\0') buffer[output++] = *prefix++;
    while (count > 0u) buffer[output++] = reversed[--count];
    return pika_runtime_create_text(
        context, PIKA_RUNTIME_VALUE_STRING, buffer, output, result);
}

#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE
static PikaStatus builtin_int(PikaRuntimeContext* context,
                              PikaRuntimeValue value,
                              PikaRuntimeValue base_value,
                              int has_base,
                              PikaRuntimeValue* result) {
    int64_t integer;
    if (has_base && value.kind != PIKA_RUNTIME_VALUE_STRING) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_INTEGER ||
        value.kind == PIKA_RUNTIME_VALUE_BOOLEAN) {
        *result = integer_value(value.as.integer);
        return PIKA_STATUS_OK;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_FLOAT) {
        if (!isfinite(value.as.floating) ||
            value.as.floating < -9223372036854775808.0 ||
            value.as.floating >= 9223372036854775808.0) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        *result = integer_value((int64_t)value.as.floating);
        return PIKA_STATUS_OK;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_STRING) {
        const char* bytes;
        uint32_t length;
        char* copy;
        char* end;
        int base = 10;
        PikaStatus text_status =
            text_argument(context, value, &bytes, &length);
        if (text_status != PIKA_STATUS_OK) return text_status;
        if (has_base) {
            if (integer_argument(base_value, &integer) != PIKA_STATUS_OK ||
                (integer != 0 && (integer < 2 || integer > 36))) {
                return PIKA_STATUS_TYPE_MISMATCH;
            }
            base = (int)integer;
        }
        copy = (char*)malloc((size_t)length + 1u);
        if (copy == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
        memcpy(copy, bytes, length);
        copy[length] = '\0';
        errno = 0;
        integer = strtoll(copy, &end, base);
        while (end != copy && isspace((unsigned char)*end)) ++end;
        if (errno == ERANGE) {
            free(copy);
            return PIKA_STATUS_INVALID_OPERAND;
        }
        if (end == copy || *end != '\0') {
            free(copy);
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        free(copy);
        *result = integer_value(integer);
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE

static PikaStatus builtin_float(PikaRuntimeContext* context,
                                PikaRuntimeValue value,
                                PikaRuntimeValue* result) {
    if (value.kind == PIKA_RUNTIME_VALUE_FLOAT) {
        *result = value;
        return PIKA_STATUS_OK;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_INTEGER ||
        value.kind == PIKA_RUNTIME_VALUE_BOOLEAN) {
        memset(result, 0, sizeof(*result));
        result->kind = PIKA_RUNTIME_VALUE_FLOAT;
        result->as.floating = (double)value.as.integer;
        return PIKA_STATUS_OK;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_STRING) {
        const char* bytes;
        uint32_t length;
        char* copy;
        char* end;
        double floating;
        PikaStatus text_status =
            text_argument(context, value, &bytes, &length);
        if (text_status != PIKA_STATUS_OK) return text_status;
        copy = (char*)malloc((size_t)length + 1u);
        if (copy == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
        memcpy(copy, bytes, length);
        copy[length] = '\0';
        floating = strtod(copy, &end);
        while (end != copy && isspace((unsigned char)*end)) ++end;
        if (end == copy || *end != '\0') {
            free(copy);
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        free(copy);
        memset(result, 0, sizeof(*result));
        result->kind = PIKA_RUNTIME_VALUE_FLOAT;
        result->as.floating = floating;
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}

#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE
static PikaStatus builtin_bytes(PikaRuntimeContext* context,
                                PikaRuntimeValue value,
                                int has_value,
                                PikaRuntimeValueKind kind,
                                PikaRuntimeValue* result) {
    if (!has_value) {
        return pika_runtime_create_text(context, kind, NULL, 0u, result);
    }
    if (kind == PIKA_RUNTIME_VALUE_BYTEARRAY &&
        value.kind == PIKA_RUNTIME_VALUE_INTEGER) {
        char* bytes;
        PikaStatus status;
        if (value.as.integer < 0) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        if (value.as.integer > (int64_t)UINT32_MAX) {
            return PIKA_STATUS_OVERFLOW_ERROR;
        }
        if (value.as.integer == 0) {
            return pika_runtime_create_text(
                context, kind, NULL, 0u, result);
        }
        bytes = (char*)calloc((size_t)value.as.integer, 1u);
        if (bytes == NULL) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        status = pika_runtime_create_text(
            context, kind, bytes, (uint32_t)value.as.integer, result);
        free(bytes);
        return status;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_STRING) {
        PikaObjectSlot* slot = pika_runtime_object(context, value);
        if (slot == NULL || (slot->length != 0u && slot->data == NULL)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        return pika_runtime_create_text(
            context, kind, (const char*)slot->data, slot->length, result);
    }
    PikaRuntimeValue* values = NULL;
    uint32_t count = 0u;
    PikaStatus status = sequence_values(context, value, &values, &count);
    char* bytes;
    if (status != PIKA_STATUS_OK) return status;
    if (count == 0u) {
        free(values);
        return pika_runtime_create_text(
            context, kind, NULL, 0u, result);
    }
    bytes = (char*)malloc((size_t)count);
    if (bytes == NULL) {
        free(values);
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    for (uint32_t index = 0u; index < count; ++index) {
        int64_t integer;
        status = integer_argument(values[index], &integer);
        if (status != PIKA_STATUS_OK || integer < 0 || integer > 255) {
            free(values);
            free(bytes);
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        bytes[index] = (char)integer;
    }
    status = pika_runtime_create_text(context, kind, bytes, count, result);
    free(values);
    free(bytes);
    return status;
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE
static PikaStatus builtin_list_tuple(PikaRuntimeContext* context,
                                     PikaRuntimeValue value,
                                     int has_value,
                                     PikaRuntimeValueKind kind,
                                     PikaRuntimeValue* result) {
    PikaRuntimeValue* values = NULL;
    uint32_t count = 0u;
    PikaStatus status;
    if (!has_value) {
        return make_sequence(context, kind, NULL, 0u, result);
    }
    status = sequence_values(context, value, &values, &count);
    if (status != PIKA_STATUS_OK) return status;
    status = make_sequence(context, kind, values, count, result);
    free(values);
    return status;
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_ITER_RANGE_ENABLE

static PikaStatus builtin_range(PikaRuntimeContext* context,
                                const PikaRuntimeValue* arguments,
                                uint8_t count,
                                PikaRuntimeValue* result) {
    int64_t start = 0;
    int64_t stop = 0;
    int64_t step = 1;
    PikaStatus status;
    if (count == 1u) {
        status = integer_argument(arguments[0], &stop);
    } else {
        status = integer_argument(arguments[0], &start);
        if (status == PIKA_STATUS_OK) {
            status = integer_argument(arguments[1], &stop);
        }
        if (status == PIKA_STATUS_OK && count == 3u) {
            status = integer_argument(arguments[2], &step);
        }
    }
    if (status != PIKA_STATUS_OK) return status;
    return pika_runtime_create_range(context, start, stop, step, result);
}

#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_LEN_ENABLE
static PikaStatus builtin_len(PikaRuntimeContext* context,
                              PikaRuntimeValue value,
                              PikaRuntimeValue* result) {
    PikaObjectSlot* slot = pika_runtime_object(context, value);
    if (slot == NULL || (!is_iterable_kind(value.kind) &&
                         value.kind != PIKA_RUNTIME_VALUE_DICT)) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_RANGE) {
        uint32_t length = 0u;
        PikaStatus status = pika_runtime_range_length(
            context, value, &length);
        if (status != PIKA_STATUS_OK) return status;
        *result = integer_value((int64_t)length);
    } else {
        *result = integer_value(slot->length);
    }
    return PIKA_STATUS_OK;
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE

static PikaStatus builtin_set(PikaRuntimeContext* context,
                              PikaRuntimeValue value,
                              int has_value,
                              PikaRuntimeValue* result) {
    PikaRuntimeValue* values = NULL;
    uint32_t count = 0u;
    uint32_t unique = 0u;
    PikaStatus status;
    if (!has_value) {
        return make_sequence(context, PIKA_RUNTIME_VALUE_SET,
                             NULL, 0u, result);
    }
    status = sequence_values(context, value, &values, &count);
    if (status != PIKA_STATUS_OK) return status;
    for (uint32_t index = 0u; index < count; ++index) {
        int exists = 0;
        for (uint32_t other = 0u; other < unique; ++other) {
            status = pika_runtime_value_equal(
                context, values[index], values[other], &exists);
            if (status != PIKA_STATUS_OK) {
                free(values);
                return status;
            }
            if (exists) break;
        }
        if (!exists) values[unique++] = values[index];
    }
    for (uint32_t index = 1u; index < unique; ++index) {
        PikaRuntimeValue item = values[index];
        uint32_t position = index;
        while (position > 0u &&
               values[position - 1u].kind == PIKA_RUNTIME_VALUE_INTEGER &&
               item.kind == PIKA_RUNTIME_VALUE_INTEGER &&
               values[position - 1u].as.integer > item.as.integer) {
            values[position] = values[position - 1u];
            --position;
        }
        values[position] = item;
    }
    status = make_sequence(context, PIKA_RUNTIME_VALUE_SET,
                           values, unique, result);
    free(values);
    return status;
}

static PikaStatus builtin_min_max(PikaRuntimeContext* context,
                                  const PikaRuntimeValue* arguments,
                                  uint8_t count,
                                  int maximum,
                                  PikaRuntimeValue* result) {
    PikaRuntimeValue* values = NULL;
    uint32_t value_count = count;
    int owns_values = 0;
    PikaStatus status;
    if (count == 1u && !is_iterable_kind(arguments[0].kind)) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (count == 1u) {
        status = sequence_values(context, arguments[0], &values, &value_count);
        if (status != PIKA_STATUS_OK) return status;
        owns_values = 1;
    } else {
        values = (PikaRuntimeValue*)arguments;
    }
    if (value_count == 0u) {
        if (owns_values) free(values);
        return PIKA_STATUS_INVALID_OPERAND;
    }
    *result = values[0];
    for (uint32_t index = 1u; index < value_count; ++index) {
        PikaObjectSlot* left_slot;
        PikaObjectSlot* right_slot;
        int comparison;
        if ((values[index].kind == PIKA_RUNTIME_VALUE_INTEGER ||
             values[index].kind == PIKA_RUNTIME_VALUE_BOOLEAN ||
             values[index].kind == PIKA_RUNTIME_VALUE_FLOAT) &&
            (result->kind == PIKA_RUNTIME_VALUE_INTEGER ||
             result->kind == PIKA_RUNTIME_VALUE_BOOLEAN ||
             result->kind == PIKA_RUNTIME_VALUE_FLOAT)) {
            double left = values[index].kind == PIKA_RUNTIME_VALUE_FLOAT
                              ? values[index].as.floating
                              : (double)values[index].as.integer;
            double right = result->kind == PIKA_RUNTIME_VALUE_FLOAT
                               ? result->as.floating
                               : (double)result->as.integer;
            comparison = left < right ? -1 : left > right ? 1 : 0;
        } else if (values[index].kind == PIKA_RUNTIME_VALUE_STRING &&
                   result->kind == PIKA_RUNTIME_VALUE_STRING) {
            left_slot = pika_runtime_object(context, values[index]);
            right_slot = pika_runtime_object(context, *result);
            if (left_slot == NULL || right_slot == NULL ||
                (left_slot->length != 0u && left_slot->data == NULL) ||
                (right_slot->length != 0u && right_slot->data == NULL)) {
                if (owns_values) free(values);
                return PIKA_STATUS_INVALID_OPERAND;
            }
            uint32_t common = left_slot->length < right_slot->length
                                  ? left_slot->length : right_slot->length;
            int compared = memcmp(left_slot->data, right_slot->data, common);
            comparison = compared < 0 ? -1 : compared > 0 ? 1
                         : left_slot->length < right_slot->length ? -1
                         : left_slot->length > right_slot->length ? 1 : 0;
        } else {
            if (owns_values) free(values);
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        if ((maximum && comparison > 0) || (!maximum && comparison < 0)) {
            *result = values[index];
        }
    }
    if (owns_values) free(values);
    return PIKA_STATUS_OK;
}

static PikaStatus builtin_sum(PikaRuntimeContext* context,
                              PikaRuntimeValue value,
                              PikaRuntimeValue start,
                              PikaRuntimeValue* result) {
    PikaRuntimeValue* values = NULL;
    uint32_t count = 0u;
    PikaStatus status = sequence_values(context, value, &values, &count);
    if (status != PIKA_STATUS_OK) return status;
    *result = start;
    for (uint32_t index = 0u; index < count; ++index) {
        if ((result->kind == PIKA_RUNTIME_VALUE_INTEGER ||
             result->kind == PIKA_RUNTIME_VALUE_BOOLEAN) &&
            (values[index].kind == PIKA_RUNTIME_VALUE_INTEGER ||
             values[index].kind == PIKA_RUNTIME_VALUE_BOOLEAN)) {
            int64_t next = values[index].as.integer;
            if ((next > 0 && result->as.integer > INT64_MAX - next) ||
                (next < 0 && result->as.integer < INT64_MIN - next)) {
                status = PIKA_STATUS_INVALID_OPERAND;
                break;
            }
            *result = integer_value(result->as.integer + next);
        } else if ((result->kind == PIKA_RUNTIME_VALUE_INTEGER ||
                    result->kind == PIKA_RUNTIME_VALUE_FLOAT) &&
                   (values[index].kind == PIKA_RUNTIME_VALUE_INTEGER ||
                    values[index].kind == PIKA_RUNTIME_VALUE_FLOAT)) {
            double left = result->kind == PIKA_RUNTIME_VALUE_FLOAT
                              ? result->as.floating
                              : (double)result->as.integer;
            double right = values[index].kind == PIKA_RUNTIME_VALUE_FLOAT
                               ? values[index].as.floating
                               : (double)values[index].as.integer;
            memset(result, 0, sizeof(*result));
            result->kind = PIKA_RUNTIME_VALUE_FLOAT;
            result->as.floating = left + right;
        } else {
            status = PIKA_STATUS_TYPE_MISMATCH;
            break;
        }
    }
    free(values);
    return status;
}

static PikaStatus builtin_enumerate(PikaRuntimeContext* context,
                                    PikaRuntimeValue value,
                                    int64_t start,
                                    PikaRuntimeValue* result) {
    PikaRuntimeValue* values = NULL;
    PikaRuntimeValue* output = NULL;
    uint32_t count = 0u;
    PikaStatus status = sequence_values(context, value, &values, &count);
    if (status != PIKA_STATUS_OK) return status;
    output = (PikaRuntimeValue*)malloc(
        (size_t)count * sizeof(PikaRuntimeValue));
    if (output == NULL && count != 0u) {
        free(values);
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    for (uint32_t index = 0u; index < count; ++index) {
        PikaRuntimeValue pair[2];
        pair[0] = integer_value(start + (int64_t)index);
        pair[1] = values[index];
        status = make_sequence(context, PIKA_RUNTIME_VALUE_TUPLE,
                               pair, 2u, &output[index]);
        if (status != PIKA_STATUS_OK) break;
    }
    if (status == PIKA_STATUS_OK) {
        status = make_sequence(context, PIKA_RUNTIME_VALUE_LIST,
                               output, count, result);
    }
    free(output);
    free(values);
    return status;
}

static PikaStatus builtin_reversed(PikaRuntimeContext* context,
                                   PikaRuntimeValue value,
                                   PikaRuntimeValue* result) {
    PikaRuntimeValue* values = NULL;
    uint32_t count = 0u;
    PikaStatus status = sequence_values(context, value, &values, &count);
    if (status != PIKA_STATUS_OK) return status;
    for (uint32_t index = 0u; index < count / 2u; ++index) {
        PikaRuntimeValue item = values[index];
        values[index] = values[count - index - 1u];
        values[count - index - 1u] = item;
    }
    status = make_sequence(context, PIKA_RUNTIME_VALUE_LIST,
                           values, count, result);
    free(values);
    return status;
}

static PikaStatus builtin_sorted(PikaRuntimeContext* context,
                                 PikaRuntimeValue value,
                                 PikaRuntimeValue* result) {
    PikaRuntimeValue* values = NULL;
    uint32_t count = 0u;
    PikaStatus status = sequence_values(context, value, &values, &count);
    if (status != PIKA_STATUS_OK) return status;
    for (uint32_t index = 1u; index < count; ++index) {
        PikaRuntimeValue item = values[index];
        uint32_t position = index;
        while (position > 0u) {
            int comparison;
            PikaRuntimeValue left = values[position - 1u];
            if (left.kind == PIKA_RUNTIME_VALUE_INTEGER &&
                item.kind == PIKA_RUNTIME_VALUE_INTEGER) {
                comparison = left.as.integer < item.as.integer ? -1
                           : left.as.integer > item.as.integer ? 1 : 0;
            } else if (left.kind == PIKA_RUNTIME_VALUE_STRING &&
                       item.kind == PIKA_RUNTIME_VALUE_STRING) {
                PikaObjectSlot* left_slot = pika_runtime_object(context, left);
                PikaObjectSlot* item_slot = pika_runtime_object(context, item);
                if (left_slot == NULL || item_slot == NULL ||
                    (left_slot->length != 0u && left_slot->data == NULL) ||
                    (item_slot->length != 0u && item_slot->data == NULL)) {
                    status = PIKA_STATUS_INVALID_OPERAND;
                    break;
                }
                uint32_t common = left_slot->length < item_slot->length
                                      ? left_slot->length : item_slot->length;
                int compared = common == 0u
                                   ? 0
                                   : memcmp(left_slot->data,
                                            item_slot->data, common);
                comparison = compared < 0 ? -1 : compared > 0 ? 1
                             : left_slot->length < item_slot->length ? -1
                             : left_slot->length > item_slot->length ? 1 : 0;
            } else {
                status = PIKA_STATUS_TYPE_MISMATCH;
                break;
            }
            if (comparison <= 0) break;
            values[position] = values[position - 1u];
            --position;
        }
        if (status != PIKA_STATUS_OK) break;
        values[position] = item;
    }
    if (status == PIKA_STATUS_OK) {
        status = make_sequence(context, PIKA_RUNTIME_VALUE_LIST,
                               values, count, result);
    }
    free(values);
    return status;
}

static PikaStatus builtin_zip(PikaRuntimeContext* context,
                              const PikaRuntimeValue* arguments,
                              uint8_t count,
                              PikaRuntimeValue* result) {
    PikaRuntimeValue** sources;
    uint32_t* lengths;
    uint32_t output_count = UINT32_MAX;
    PikaRuntimeValue* output = NULL;
    PikaStatus status = PIKA_STATUS_OK;
    if (count == 0u) {
        return make_sequence(context, PIKA_RUNTIME_VALUE_LIST,
                             NULL, 0u, result);
    }
    sources = (PikaRuntimeValue**)calloc(count, sizeof(*sources));
    lengths = (uint32_t*)calloc(count, sizeof(*lengths));
    if (sources == NULL || lengths == NULL) {
        free(sources);
        free(lengths);
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    for (uint32_t index = 0u; index < count; ++index) {
        status = sequence_values(context, arguments[index],
                                 &sources[index], &lengths[index]);
        if (status != PIKA_STATUS_OK) break;
        if (lengths[index] < output_count) output_count = lengths[index];
    }
    if (status == PIKA_STATUS_OK) {
        output = (PikaRuntimeValue*)malloc(
            (size_t)output_count * sizeof(PikaRuntimeValue));
        if (output == NULL && output_count != 0u) {
            status = PIKA_STATUS_STORAGE_TOO_SMALL;
        }
    }
    for (uint32_t index = 0u;
         status == PIKA_STATUS_OK && index < output_count; ++index) {
        PikaRuntimeValue* tuple = (PikaRuntimeValue*)malloc(
            (size_t)count * sizeof(PikaRuntimeValue));
        if (tuple == NULL) {
            status = PIKA_STATUS_STORAGE_TOO_SMALL;
            break;
        }
        for (uint32_t source = 0u; source < count; ++source) {
            tuple[source] = sources[source][index];
        }
        status = make_sequence(context, PIKA_RUNTIME_VALUE_TUPLE,
                               tuple, count, &output[index]);
        free(tuple);
    }
    if (status == PIKA_STATUS_OK) {
        status = make_sequence(context, PIKA_RUNTIME_VALUE_LIST,
                               output, output_count, result);
    }
    for (uint32_t index = 0u; index < count; ++index) free(sources[index]);
    free(sources);
    free(lengths);
    free(output);
    return status;
}

#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE
static PikaStatus builtin_dict(PikaRuntimeContext* context,
                               PikaRuntimeValue value,
                               int has_value,
                               PikaRuntimeValue* result) {
    PikaRuntimeValue* pairs = NULL;
    PikaMapEntry* entries = NULL;
    uint32_t count = 0u;
    PikaStatus status;
    if (!has_value) {
        return create_map(context, NULL, 0u, result);
    }
    if (value.kind == PIKA_RUNTIME_VALUE_DICT) {
        *result = value;
        return PIKA_STATUS_OK;
    }
    status = sequence_values(context, value, &pairs, &count);
    if (status != PIKA_STATUS_OK) return status;
    entries = (PikaMapEntry*)malloc((size_t)count * sizeof(*entries));
    if (entries == NULL && count != 0u) {
        free(pairs);
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    for (uint32_t index = 0u; index < count; ++index) {
        PikaObjectSlot* pair_slot = pika_runtime_object(context, pairs[index]);
        if (pair_slot == NULL || pair_slot->length != 2u) {
            free(entries);
            free(pairs);
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        if (pair_slot->data == NULL) {
            free(entries);
            free(pairs);
            return PIKA_STATUS_INVALID_OPERAND;
        }
        entries[index].key = ((PikaRuntimeValue*)pair_slot->data)[0];
        entries[index].value = ((PikaRuntimeValue*)pair_slot->data)[1];
    }
    status = create_map(context, entries, count, result);
    free(entries);
    free(pairs);
    return status;
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_TYPE_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_HELP_ENABLE

static const char* builtin_type_name(PikaRuntimeValue value) {
    switch (value.kind) {
        case PIKA_RUNTIME_VALUE_NONE: return "NoneType";
        case PIKA_RUNTIME_VALUE_BOOLEAN: return "bool";
        case PIKA_RUNTIME_VALUE_INTEGER: return "int";
        case PIKA_RUNTIME_VALUE_FLOAT: return "float";
        case PIKA_RUNTIME_VALUE_STRING: return "str";
        case PIKA_RUNTIME_VALUE_BYTES: return "bytes";
        case PIKA_RUNTIME_VALUE_BYTEARRAY: return "bytearray";
        case PIKA_RUNTIME_VALUE_LIST: return "list";
        case PIKA_RUNTIME_VALUE_TUPLE: return "tuple";
        case PIKA_RUNTIME_VALUE_DICT: return "dict";
        case PIKA_RUNTIME_VALUE_SET: return "set";
        case PIKA_RUNTIME_VALUE_RANGE: return "range";
        case PIKA_RUNTIME_VALUE_TYPE: return "type";
        case PIKA_RUNTIME_VALUE_MODULE: return "module";
#if PIKA_CAPABILITY_EXCEPTION_BASIC_ENABLE
        case PIKA_RUNTIME_VALUE_EXCEPTION:
            return pika_status_exception_name(
                (PikaStatus)value.reserved);
#endif
        default: return "object";
    }
}

static PikaStatus builtin_type(PikaRuntimeContext* context,
                               PikaRuntimeValue value,
                               PikaRuntimeValue* result) {
    const char* name = builtin_type_name(value);
    return pika_runtime_create_text(
        context, PIKA_RUNTIME_VALUE_TYPE, name,
        (uint32_t)strlen(name), result);
}

#endif

#if PIKA_CAPABILITY_BUILTIN_HELP_ENABLE
static PikaStatus builtin_help_write(
    PikaRuntimeContext* context,
    const char* text,
    size_t length) {
    if (context->output == NULL || context->output->write == NULL) {
        return PIKA_STATUS_OUTPUT_ERROR;
    }
    return context->output->write(
               context->output->context, text, length) == PIKA_STATUS_OK
               ? PIKA_STATUS_OK
               : PIKA_STATUS_OUTPUT_ERROR;
}

static PikaStatus builtin_help_write_callable(
    PikaRuntimeContext* context,
    const PikaBindingCallable* callable,
    const char* indent) {
    uint8_t index;
    PikaStatus status = builtin_help_write(
        context, indent, strlen(indent));
    if (status == PIKA_STATUS_OK) {
        status = builtin_help_write(
            context, callable->name, strlen(callable->name));
    }
    if (status == PIKA_STATUS_OK) {
        status = builtin_help_write(context, "(", 1u);
    }
    for (index = 0u;
         status == PIKA_STATUS_OK && index < callable->parameter_count;
         ++index) {
        const char* name =
            callable->parameter_names != NULL &&
                    callable->parameter_names[index] != NULL
                ? callable->parameter_names[index]
                : "arg";
        if (index != 0u) {
            status = builtin_help_write(context, ", ", 2u);
        }
        if (status == PIKA_STATUS_OK) {
            status = builtin_help_write(
                context, name, strlen(name));
        }
        if (status == PIKA_STATUS_OK &&
            index >= callable->required_parameter_count) {
            status = builtin_help_write(context, "=...", 4u);
        }
    }
    if (status == PIKA_STATUS_OK) {
        status = builtin_help_write(context, ")\n", 2u);
    }
    return status;
}

static const char* builtin_help_module_name(
    const PikaBindingModule* module) {
    if (module == NULL || module->name == NULL) {
        return NULL;
    }
    return module->name[0] == '_' && module->name[1] != '\0'
               ? module->name + 1
               : module->name;
}

static PikaStatus builtin_help_write_module(
    PikaRuntimeContext* context,
    const PikaBindingModule* module) {
    static const char functions[] = "\nFUNCTIONS\n";
    static const char classes[] = "\nCLASSES\n";
    static const char data[] = "\nDATA\n";
    const char* module_name = builtin_help_module_name(module);
    uint16_t index;
    PikaStatus status = builtin_help_write(
        context, "Help on module ", 15u);
    if (status == PIKA_STATUS_OK) {
        status = builtin_help_write(
            context, module_name, strlen(module_name));
    }
    if (status == PIKA_STATUS_OK) {
        status = builtin_help_write(context, ":\n\nNAME\n    ", 12u);
    }
    if (status == PIKA_STATUS_OK) {
        status = builtin_help_write(
            context, module_name, strlen(module_name));
    }
    if (status == PIKA_STATUS_OK &&
        module->function_count == 0u &&
        module->class_count == 0u &&
        module->constant_count == 0u) {
        status = builtin_help_write(context, "\n", 1u);
    }
    if (status == PIKA_STATUS_OK && module->function_count > 0u) {
        status = builtin_help_write(
            context, functions, sizeof(functions) - 1u);
    }
    for (index = 0u;
         status == PIKA_STATUS_OK && index < module->function_count;
         ++index) {
        status = builtin_help_write_callable(
            context, &module->functions[index], "    ");
    }
    if (status == PIKA_STATUS_OK && module->class_count > 0u) {
        status = builtin_help_write(
            context, classes, sizeof(classes) - 1u);
    }
    for (index = 0u;
         status == PIKA_STATUS_OK && index < module->class_count;
         ++index) {
        const PikaBindingClass* class_descriptor =
            &module->classes[index];
        uint16_t method_index;
        status = builtin_help_write(context, "    class ", 10u);
        if (status == PIKA_STATUS_OK) {
            status = builtin_help_write(
                context, class_descriptor->name,
                strlen(class_descriptor->name));
        }
        if (status == PIKA_STATUS_OK) {
            status = builtin_help_write(context, "\n", 1u);
        }
        for (method_index = 0u;
             status == PIKA_STATUS_OK &&
             method_index < class_descriptor->method_count;
             ++method_index) {
            status = builtin_help_write_callable(
                context, &class_descriptor->methods[method_index],
                "        ");
        }
    }
    if (status == PIKA_STATUS_OK && module->constant_count > 0u) {
        status = builtin_help_write(
            context, data, sizeof(data) - 1u);
    }
    for (index = 0u;
         status == PIKA_STATUS_OK && index < module->constant_count;
         ++index) {
        status = builtin_help_write(context, "    ", 4u);
        if (status == PIKA_STATUS_OK) {
            status = builtin_help_write(
                context, module->constants[index].name,
                strlen(module->constants[index].name));
        }
        if (status == PIKA_STATUS_OK) {
            status = builtin_help_write(context, "\n", 1u);
        }
    }
    return status;
}

static PikaStatus builtin_help_write_modules(
    PikaRuntimeContext* context) {
    uint16_t index;
    PikaStatus status = builtin_help_write(
        context, "Available modules:", 18u);
    if (status != PIKA_STATUS_OK) return status;
    for (index = 0u;
         index < context->program->python_module_count;
         ++index) {
        const PikaPythonModule* module =
            &context->program->python_modules[index];
        const char* name =
            &context->program->module_names[module->name_offset];
        uint16_t previous;
        int duplicate = 0;
        if ((module->flags & PIKA_PYTHON_MODULE_PUBLIC) == 0u) {
            continue;
        }
        for (previous = 0u; previous < index; ++previous) {
            const PikaPythonModule* prior =
                &context->program->python_modules[previous];
            if ((prior->flags & PIKA_PYTHON_MODULE_PUBLIC) == 0u) {
                continue;
            }
            if (prior->name_length == module->name_length &&
                memcmp(
                    &context->program->module_names[prior->name_offset],
                    name, module->name_length) == 0) {
                duplicate = 1;
                break;
            }
        }
        if (duplicate) continue;
        status = builtin_help_write(context, " ", 1u);
        if (status == PIKA_STATUS_OK) {
            status = builtin_help_write(
                context, name, module->name_length);
        }
        if (status != PIKA_STATUS_OK) return status;
    }
    for (index = 0u;
         index < context->program->binding_module_count;
         ++index) {
        const PikaBindingModule* module =
            context->program->binding_modules[index];
        const char* name = builtin_help_module_name(module);
        uint16_t previous;
        int duplicate = 0;
        if (name == NULL) continue;
        for (previous = 0u;
             previous < context->program->python_module_count;
             ++previous) {
            const PikaPythonModule* python_module =
                &context->program->python_modules[previous];
            if ((python_module->flags &
                 PIKA_PYTHON_MODULE_PUBLIC) == 0u) {
                continue;
            }
            if (strlen(name) == python_module->name_length &&
                memcmp(
                    name,
                    &context->program->module_names[
                        python_module->name_offset],
                    python_module->name_length) == 0) {
                duplicate = 1;
                break;
            }
        }
        if (duplicate) continue;
        for (previous = 0u; previous < index; ++previous) {
            const char* previous_name = builtin_help_module_name(
                context->program->binding_modules[previous]);
            if (previous_name != NULL &&
                strcmp(previous_name, name) == 0) {
                duplicate = 1;
                break;
            }
        }
        if (duplicate) continue;
        status = builtin_help_write(context, " ", 1u);
        if (status == PIKA_STATUS_OK) {
            status = builtin_help_write(
                context, name, strlen(name));
        }
        if (status != PIKA_STATUS_OK) return status;
    }
    return builtin_help_write(context, "\n", 1u);
}

static const PikaPythonModule* builtin_help_find_python_module(
    const PikaRuntimeContext* context,
    const char* name) {
    uint16_t index;
    size_t length = strlen(name);
    for (index = 0u;
         index < context->program->python_module_count;
         ++index) {
        const PikaPythonModule* module =
            &context->program->python_modules[index];
        if ((module->flags & PIKA_PYTHON_MODULE_PUBLIC) != 0u &&
            length == module->name_length &&
            memcmp(
                name,
                &context->program->module_names[module->name_offset],
                module->name_length) == 0) {
            return module;
        }
    }
    return NULL;
}

static PikaStatus builtin_help_write_python_module(
    PikaRuntimeContext* context,
    const PikaPythonModule* module) {
    const char* name =
        &context->program->module_names[module->name_offset];
    PikaStatus status = builtin_help_write(
        context, "Help on module ", 15u);
    if (status == PIKA_STATUS_OK) {
        status = builtin_help_write(
            context, name, module->name_length);
    }
    if (status == PIKA_STATUS_OK) {
        status = builtin_help_write(context, ":\n\nNAME\n    ", 12u);
    }
    if (status == PIKA_STATUS_OK) {
        status = builtin_help_write(
            context, name, module->name_length);
    }
    if (status == PIKA_STATUS_OK) {
        status = builtin_help_write(context, "\n", 1u);
    }
    return status;
}

static const PikaBindingModule* builtin_help_find_module(
    const PikaRuntimeContext* context,
    const char* name) {
    uint16_t index;
    for (index = 0u;
         index < context->program->binding_module_count;
         ++index) {
        const PikaBindingModule* module =
            context->program->binding_modules[index];
        const char* public_name = builtin_help_module_name(module);
        if (public_name != NULL && strcmp(public_name, name) == 0) {
            return module;
        }
    }
    return NULL;
}

static PikaStatus builtin_help(PikaRuntimeContext* context,
                               const PikaRuntimeValue* arguments,
                               uint8_t count,
                               PikaRuntimeValue* result) {
    static const char banner[] =
        "Welcome to Python help utility!\n\n"
        "Type help(\"name\") for help about a builtin.\n\n"
        "Built-in functions:";
    const PikaBuiltinDescriptor* descriptor;
    const PikaBindingModule* module;
    const PikaPythonModule* python_module;
    PikaBuiltinId id;
    uint16_t index;
    PikaStatus status;
    if (count == 0u) {
        status = builtin_help_write(
            context, banner, sizeof(banner) - 1u);
        if (status != PIKA_STATUS_OK) return status;
        for (index = 0u; index < pika_builtin_catalog_count(); ++index) {
            descriptor = pika_builtin_descriptor((PikaBuiltinId)index);
            if (descriptor == NULL || !descriptor->implemented ||
                descriptor->name == NULL) continue;
            status = builtin_help_write(context, " ", 1u);
            if (status == PIKA_STATUS_OK) {
                status = builtin_help_write(
                    context, descriptor->name,
                    strlen(descriptor->name));
            }
            if (status != PIKA_STATUS_OK) return status;
        }
        status = builtin_help_write(context, "\n", 1u);
        if (status == PIKA_STATUS_OK) {
            status = builtin_help_write_modules(context);
        }
    } else if (arguments[0].kind == PIKA_RUNTIME_VALUE_STRING) {
        const char* topic;
        uint32_t topic_length;
        char name[PIKA_BUILTIN_HELP_TOPIC_LIMIT + 1u];
        status = text_argument(
            context, arguments[0], &topic, &topic_length);
        if (status != PIKA_STATUS_OK) return status;
        if (topic_length > PIKA_BUILTIN_HELP_TOPIC_LIMIT) {
            return PIKA_STATUS_FRONTEND_LIMIT;
        }
        memcpy(name, topic, topic_length);
        name[topic_length] = '\0';
        if (strcmp(name, "modules") == 0) {
            status = builtin_help_write_modules(context);
        } else if ((module = builtin_help_find_module(
                        context, name)) != NULL) {
            status = builtin_help_write_module(context, module);
        } else if ((python_module = builtin_help_find_python_module(
                        context, name)) != NULL) {
            status = builtin_help_write_python_module(
                context, python_module);
        } else if (pika_builtin_find(name, &id) == PIKA_STATUS_OK) {
            descriptor = pika_builtin_descriptor(id);
            status = builtin_help_write(
                context, "Help on built-in function ", 26u);
            if (status == PIKA_STATUS_OK) {
                status = builtin_help_write(
                    context, descriptor->name,
                    strlen(descriptor->name));
            }
            if (status == PIKA_STATUS_OK) {
                status = builtin_help_write(context, ":\n\n", 3u);
            }
            if (status == PIKA_STATUS_OK) {
                status = builtin_help_write(
                    context, descriptor->name,
                    strlen(descriptor->name));
            }
            if (status == PIKA_STATUS_OK) {
                status = builtin_help_write(context, "(...)\n", 6u);
            }
        } else {
            status = builtin_help_write(
                context, "No Python documentation found for '", 35u);
            if (status == PIKA_STATUS_OK) {
                status = builtin_help_write(
                    context, topic, topic_length);
            }
            if (status == PIKA_STATUS_OK) {
                status = builtin_help_write(context, "'.\n", 3u);
            }
        }
    } else if (arguments[0].kind == PIKA_RUNTIME_VALUE_MODULE) {
        PikaObjectSlot* slot = pika_runtime_object(
            context, arguments[0]);
        char name[PIKA_BUILTIN_HELP_TOPIC_LIMIT + 1u];
        if (slot == NULL || slot->length > PIKA_BUILTIN_HELP_TOPIC_LIMIT ||
            (slot->length > 0u && slot->data == NULL)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        memcpy(name, slot->data, slot->length);
        name[slot->length] = '\0';
        module = builtin_help_find_module(context, name);
        python_module = builtin_help_find_python_module(context, name);
        if (module != NULL) {
            status = builtin_help_write_module(context, module);
        } else if (python_module != NULL) {
            status = builtin_help_write_python_module(
                context, python_module);
        } else {
            status = PIKA_STATUS_MODULE_NOT_FOUND;
        }
    } else {
        const char* name = builtin_type_name(arguments[0]);
        status = builtin_help_write(context, "Help on ", 8u);
        if (status == PIKA_STATUS_OK) {
            status = builtin_help_write(context, name, strlen(name));
        }
        if (status == PIKA_STATUS_OK) {
            status = builtin_help_write(context, " object:\n\nclass ", 16u);
        }
        if (status == PIKA_STATUS_OK) {
            status = builtin_help_write(context, name, strlen(name));
        }
        if (status == PIKA_STATUS_OK) {
            status = builtin_help_write(context, "(object)\n", 9u);
        }
    }
    if (status != PIKA_STATUS_OK) return status;
    *result = none_value();
    return PIKA_STATUS_OK;
}
#endif

#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE

static PikaStatus builtin_isinstance(PikaRuntimeContext* context,
                                     PikaRuntimeValue value,
                                     PikaRuntimeValue type,
                                     PikaRuntimeValue* result) {
    const char* name;
    uint32_t length;
    const char* expected;
    if (type.kind != PIKA_RUNTIME_VALUE_TYPE ||
        text_argument(context, type, &name, &length) != PIKA_STATUS_OK) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    switch (value.kind) {
        case PIKA_RUNTIME_VALUE_NONE: expected = "NoneType"; break;
        case PIKA_RUNTIME_VALUE_BOOLEAN: expected = "bool"; break;
        case PIKA_RUNTIME_VALUE_INTEGER: expected = "int"; break;
        case PIKA_RUNTIME_VALUE_FLOAT: expected = "float"; break;
        case PIKA_RUNTIME_VALUE_STRING: expected = "str"; break;
        case PIKA_RUNTIME_VALUE_BYTES: expected = "bytes"; break;
        case PIKA_RUNTIME_VALUE_BYTEARRAY: expected = "bytearray"; break;
        case PIKA_RUNTIME_VALUE_LIST: expected = "list"; break;
        case PIKA_RUNTIME_VALUE_TUPLE: expected = "tuple"; break;
        case PIKA_RUNTIME_VALUE_DICT: expected = "dict"; break;
        case PIKA_RUNTIME_VALUE_SET: expected = "set"; break;
        case PIKA_RUNTIME_VALUE_RANGE: expected = "range"; break;
        default: expected = "object"; break;
    }
    *result = boolean_value(
        (strlen(expected) == length &&
         memcmp(expected, name, length) == 0) ||
        (value.kind == PIKA_RUNTIME_VALUE_BOOLEAN &&
         length == 3u && memcmp(name, "int", 3u) == 0));
    return PIKA_STATUS_OK;
}

static PikaStatus builtin_hash(PikaRuntimeContext* context,
                               PikaRuntimeValue value,
                               PikaRuntimeValue* result) {
    if (value.kind == PIKA_RUNTIME_VALUE_INTEGER ||
        value.kind == PIKA_RUNTIME_VALUE_BOOLEAN) {
        *result = integer_value(value.as.integer);
        return PIKA_STATUS_OK;
    }
    if (value.kind == PIKA_RUNTIME_VALUE_STRING ||
        value.kind == PIKA_RUNTIME_VALUE_BYTES) {
        PikaObjectSlot* slot = pika_runtime_object(context, value);
        uint64_t hash = 1469598103934665603ULL;
        if (slot == NULL || (slot->length != 0u && slot->data == NULL)) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        for (uint32_t index = 0u; index < slot->length; ++index) {
            hash ^= (unsigned char)((const char*)slot->data)[index];
            hash *= 1099511628211ULL;
        }
        *result = integer_value((int64_t)hash);
        return PIKA_STATUS_OK;
    }
    return PIKA_STATUS_TYPE_MISMATCH;
}

static PikaStatus builtin_round(PikaRuntimeValue value,
                                int has_digits,
                                PikaRuntimeValue digits_value,
                                PikaRuntimeValue* result) {
    int64_t digits = 0;
    double number;
    if (value.kind == PIKA_RUNTIME_VALUE_FLOAT) number = value.as.floating;
    else if (value.kind == PIKA_RUNTIME_VALUE_INTEGER ||
             value.kind == PIKA_RUNTIME_VALUE_BOOLEAN) number = (double)value.as.integer;
    else return PIKA_STATUS_TYPE_MISMATCH;
    if (has_digits) {
        if (integer_argument(digits_value, &digits) != PIKA_STATUS_OK ||
            digits < -15 || digits > 15) return PIKA_STATUS_TYPE_MISMATCH;
        if (value.kind == PIKA_RUNTIME_VALUE_INTEGER ||
            value.kind == PIKA_RUNTIME_VALUE_BOOLEAN) {
            int64_t integer = value.as.integer;
            int64_t factor = 1;
            int64_t quotient;
            int64_t remainder;
            int64_t magnitude;
            PikaStatus status;
            if (digits >= 0) {
                *result = integer_value(integer);
                return PIKA_STATUS_OK;
            }
            for (int64_t index = 0; index < -digits; ++index) {
                factor *= 10;
            }
            quotient = integer / factor;
            remainder = integer % factor;
            magnitude = remainder < 0 ? -remainder : remainder;
            if (magnitude > factor / 2 ||
                (magnitude == factor / 2 &&
                 (quotient % 2) != 0)) {
                quotient += integer < 0 ? -1 : 1;
            }
            status = pika_runtime_integer_multiply_checked(
                quotient, factor, &integer);
            if (status != PIKA_STATUS_OK) return status;
            *result = integer_value(integer);
        } else {
            double scale = pow(10.0, (double)digits);
            memset(result, 0, sizeof(*result));
            result->kind = PIKA_RUNTIME_VALUE_FLOAT;
            result->as.floating = nearbyint(number * scale) / scale;
        }
    } else if (value.kind == PIKA_RUNTIME_VALUE_FLOAT) {
        double rounded = nearbyint(number);
        if (!isfinite(rounded) ||
            rounded < -9223372036854775808.0 ||
            rounded >= 9223372036854775808.0) {
            return PIKA_STATUS_INVALID_OPERAND;
        }
        *result = integer_value((int64_t)rounded);
    } else {
        *result = integer_value(value.as.integer);
    }
    return PIKA_STATUS_OK;
}

static PikaStatus builtin_format(PikaRuntimeContext* context,
                                 PikaRuntimeValue value,
                                 PikaRuntimeValue spec,
                                 int has_spec,
                                 PikaRuntimeValue* result) {
    const char* format = "";
    uint32_t length = 0u;
    char local[32];
    char output[128];
    int written;
    if (has_spec) {
        if (text_argument(context, spec, &format, &length) != PIKA_STATUS_OK ||
            length >= sizeof(local)) return PIKA_STATUS_TYPE_MISMATCH;
    }
    memcpy(local, format, length);
    local[length] = '\0';
    if (value.kind == PIKA_RUNTIME_VALUE_INTEGER ||
        value.kind == PIKA_RUNTIME_VALUE_BOOLEAN) {
        if (length == 1u && local[0] == 'x') {
            uint64_t magnitude = integer_magnitude(value.as.integer);
            written = snprintf(
                output, sizeof(output),
                value.as.integer < 0 ? "-%llx" : "%llx",
                (unsigned long long)magnitude);
        } else if (length == 1u && local[0] == 'b') {
            uint64_t magnitude = integer_magnitude(value.as.integer);
            char reversed[64];
            uint32_t digits = 0u;
            uint32_t position = 0u;
            do {
                reversed[digits++] =
                    (char)('0' + (magnitude & 1u));
                magnitude >>= 1u;
            } while (magnitude != 0u);
            if (value.as.integer < 0) {
                output[position++] = '-';
            }
            while (digits > 0u) {
                output[position++] = reversed[--digits];
            }
            output[position] = '\0';
            written = (int)position;
        } else if (length > 1u && local[0] == '0' &&
                   local[length - 1u] == 'd') {
            local[length - 1u] = '\0';
            int width = (int)strtol(local, NULL, 10);
            written = snprintf(output, sizeof(output), "%0*lld", width,
                               (long long)value.as.integer);
        } else if (length == 0u ||
                   (length == 1u && local[0] == 'd')) {
            written = snprintf(output, sizeof(output), "%lld",
                               (long long)value.as.integer);
        } else {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
    } else if (value.kind == PIKA_RUNTIME_VALUE_FLOAT) {
        if (length == 0u) {
            return value_text(context, value, 0, result);
        } else if (length >= 3u && local[0] == '.' &&
                   local[length - 1u] == 'f') {
            int precision = 0;
            for (uint32_t index = 1u; index + 1u < length; ++index) {
                if (!isdigit((unsigned char)local[index])) {
                    return PIKA_STATUS_TYPE_MISMATCH;
                }
                precision = precision * 10 + local[index] - '0';
                if (precision > 15) {
                    return PIKA_STATUS_TYPE_MISMATCH;
                }
            }
            written = snprintf(
                output, sizeof(output), "%.*f",
                precision, value.as.floating);
        } else {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
    } else if (value.kind == PIKA_RUNTIME_VALUE_STRING &&
               length > 0u) {
        const char* bytes;
        uint32_t byte_count;
        uint32_t width = 0u;
        uint32_t left_padding = 0u;
        uint32_t right_padding = 0u;
        char alignment = local[0];
        if ((alignment != '<' && alignment != '>' &&
             alignment != '^') ||
            text_argument(
                context, value, &bytes, &byte_count) !=
                PIKA_STATUS_OK) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        for (uint32_t index = 1u; index < length; ++index) {
            if (!isdigit((unsigned char)local[index])) {
                return PIKA_STATUS_TYPE_MISMATCH;
            }
            width = width * 10u +
                    (uint32_t)(local[index] - '0');
        }
        if (width < byte_count) width = byte_count;
        if (width >= sizeof(output)) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        if (alignment == '>') {
            left_padding = width - byte_count;
        } else if (alignment == '<') {
            right_padding = width - byte_count;
        } else {
            left_padding = (width - byte_count) / 2u;
            right_padding = width - byte_count - left_padding;
        }
        memset(output, ' ', width);
        memcpy(&output[left_padding], bytes, byte_count);
        output[width] = '\0';
        (void)right_padding;
        written = (int)width;
    } else {
        return value_text(context, value, 0, result);
    }
    if (written < 0 || (size_t)written >= sizeof(output)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    return pika_runtime_create_text(context, PIKA_RUNTIME_VALUE_STRING,
                                    output, (uint32_t)written, result);
}

static PikaStatus builtin_next(PikaRuntimeContext* context,
                               PikaRuntimeValue iterator,
                               PikaRuntimeValue fallback,
                               int has_fallback,
                               PikaRuntimeValue* result) {
    PikaObjectSlot* iterator_slot;
    PikaIteratorData* state;
    PikaObjectSlot* source_slot;
    PikaRuntimeValue source;
    uint32_t source_length;
    PikaStatus status;
    if (iterator.kind != PIKA_RUNTIME_VALUE_ITERATOR) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    iterator_slot = pika_runtime_object(context, iterator);
    if (iterator_slot == NULL || iterator_slot->data == NULL) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    state = (PikaIteratorData*)iterator_slot->data;
    if (state->source_index >= context->objects->count) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
    source_slot = &context->objects->slots[state->source_index];
    source.kind = (PikaRuntimeValueKind)source_slot->kind;
    source.as.object_index = state->source_index;
    if (source.kind == PIKA_RUNTIME_VALUE_RANGE) {
        status = pika_runtime_range_length(
            context, source, &source_length);
        if (status != PIKA_STATUS_OK) return status;
    } else {
        source_length = source_slot->length;
    }
    if (state->position >= source_length) {
        if (has_fallback) {
            *result = fallback;
            return PIKA_STATUS_OK;
        }
        return PIKA_STATUS_INVALID_OPERAND;
    }
    status = pika_runtime_sequence_item(
        context, source, (int64_t)state->position, result);
    if (status == PIKA_STATUS_OK) ++state->position;
    return status;
}

#endif

static PikaStatus builtin_print(PikaRuntimeContext* context,
                                const PikaRuntimeValue* arguments,
                                uint8_t count,
                                PikaRuntimeValue* result) {
    if (context->output == NULL || context->output->write == NULL) {
        return PIKA_STATUS_OUTPUT_ERROR;
    }
    for (uint32_t index = 0u; index < count; ++index) {
        if (index != 0u &&
            context->output->write(context->output->context, " ", 1u) !=
                PIKA_STATUS_OK) return PIKA_STATUS_OUTPUT_ERROR;
        if (pika_runtime_write_value_text(
                context, arguments[index], 0, 0) != PIKA_STATUS_OK) {
            return PIKA_STATUS_OUTPUT_ERROR;
        }
    }
    if (context->output->write(context->output->context, "\n", 1u) !=
        PIKA_STATUS_OK) return PIKA_STATUS_OUTPUT_ERROR;
    *result = none_value();
    return PIKA_STATUS_OK;
}

static PikaStatus builtin_dispatch(PikaRuntimeContext* context,
                                   const PikaRuntimeFrame* frame,
                                   PikaBuiltinId id,
                                   uint8_t base,
                                   uint8_t count,
                                   PikaRuntimeValue* result) {
    PikaRuntimeValue arguments[PIKA_RUNTIME_VALUE_LIMIT];
    if (count > PIKA_RUNTIME_VALUE_LIMIT) return PIKA_STATUS_ARGUMENT_COUNT;
    for (uint32_t index = 0u; index < count; ++index) {
        arguments[index] = argument_value(context, frame, base, index);
    }
    switch (id) {
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE
        case PIKA_BUILTIN_ABS:
            return builtin_abs(arguments[0], result);
        case PIKA_BUILTIN_ALL:
            return builtin_all_any(context, arguments[0], 1, result);
        case PIKA_BUILTIN_ANY:
            return builtin_all_any(context, arguments[0], 0, result);
        case PIKA_BUILTIN_ASCII:
            return value_text(context, arguments[0], 1, result);
        case PIKA_BUILTIN_BIN:
            return builtin_integer_text(arguments[0], 2, "0b", context, result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE
        case PIKA_BUILTIN_BOOL:
            *result = boolean_value(
                count == 0u ? 0 : pika_runtime_value_truthy(context, arguments[0]));
            return PIKA_STATUS_OK;
        case PIKA_BUILTIN_BYTES:
            return builtin_bytes(
                context, count == 0u ? none_value() : arguments[0],
                count != 0u, PIKA_RUNTIME_VALUE_BYTES, result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE
        case PIKA_BUILTIN_BYTEARRAY:
            return builtin_bytes(
                context, count == 0u ? none_value() : arguments[0],
                count != 0u, PIKA_RUNTIME_VALUE_BYTEARRAY, result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE
        case PIKA_BUILTIN_CHR: {
            int64_t integer;
            char byte;
            if (integer_argument(arguments[0], &integer) != PIKA_STATUS_OK ||
                integer < 0 || integer > 127) return PIKA_STATUS_TYPE_MISMATCH;
            byte = (char)integer;
            return pika_runtime_create_text(
                context, PIKA_RUNTIME_VALUE_STRING, &byte, 1u, result);
        }
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE
        case PIKA_BUILTIN_DICT:
            return builtin_dict(
                context, count == 0u ? none_value() : arguments[0],
                count != 0u, result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE
        case PIKA_BUILTIN_DIVMOD: {
            int64_t left;
            int64_t right;
            int64_t quotient;
            int64_t remainder;
            PikaRuntimeValue pair[2];
            if (integer_argument(arguments[0], &left) != PIKA_STATUS_OK ||
                integer_argument(arguments[1], &right) != PIKA_STATUS_OK) {
                return PIKA_STATUS_TYPE_MISMATCH;
            }
            if (right == 0) return PIKA_STATUS_DIVISION_BY_ZERO;
            if (left == INT64_MIN && right == -1) return PIKA_STATUS_INVALID_OPERAND;
            quotient = left / right;
            remainder = left % right;
            if (remainder != 0 && ((remainder < 0) != (right < 0))) {
                remainder += right;
                --quotient;
            }
            pair[0] = integer_value(quotient);
            pair[1] = integer_value(remainder);
            return make_sequence(context, PIKA_RUNTIME_VALUE_TUPLE,
                                 pair, 2u, result);
        }
        case PIKA_BUILTIN_ENUMERATE: {
            int64_t start = 0;
            if (count == 1u) {
                return builtin_enumerate(context, arguments[0], 0, result);
            }
            if (count == 2u) {
                if (integer_argument(arguments[1], &start) != PIKA_STATUS_OK) {
                    return PIKA_STATUS_TYPE_MISMATCH;
                }
                return builtin_enumerate(context, arguments[0], start, result);
            }
            return PIKA_STATUS_ARGUMENT_COUNT;
        }
        case PIKA_BUILTIN_FLOAT:
            if (count == 0u) {
                memset(result, 0, sizeof(*result));
                result->kind = PIKA_RUNTIME_VALUE_FLOAT;
                result->as.floating = 0.0;
                return PIKA_STATUS_OK;
            }
            return builtin_float(context, arguments[0], result);
        case PIKA_BUILTIN_FORMAT:
            return builtin_format(context, arguments[0],
                                  count > 1u ? arguments[1] : none_value(),
                                  count > 1u, result);
        case PIKA_BUILTIN_HASH:
            return builtin_hash(context, arguments[0], result);
        case PIKA_BUILTIN_HEX:
            return builtin_integer_text(arguments[0], 16, "0x", context, result);
        case PIKA_BUILTIN_ID:
            *result = integer_value(
                arguments[0].kind >= PIKA_RUNTIME_VALUE_STRING
                    ? (int64_t)arguments[0].as.object_index + 1
                    : arguments[0].as.integer);
            return PIKA_STATUS_OK;
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE
        case PIKA_BUILTIN_INT:
            if (count == 0u) {
                *result = integer_value(0);
                return PIKA_STATUS_OK;
            }
            return builtin_int(context, arguments[0],
                               count > 1u ? arguments[1] : none_value(),
                               count > 1u, result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE
        case PIKA_BUILTIN_ISINSTANCE:
            return builtin_isinstance(context, arguments[0], arguments[1], result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_LEN_ENABLE
        case PIKA_BUILTIN_LEN:
            return builtin_len(context, arguments[0], result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE
        case PIKA_BUILTIN_LIST:
            return builtin_list_tuple(
                context, count == 0u ? none_value() : arguments[0],
                count != 0u, PIKA_RUNTIME_VALUE_LIST, result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE
        case PIKA_BUILTIN_MAX:
        case PIKA_BUILTIN_MIN: {
            return builtin_min_max(
                context, arguments, count,
                id == PIKA_BUILTIN_MAX, result);
        }
        case PIKA_BUILTIN_OCT:
            return builtin_integer_text(arguments[0], 8, "0o", context, result);
        case PIKA_BUILTIN_ORD: {
            const char* bytes;
            uint32_t length;
            PikaStatus status = text_argument(
                context, arguments[0], &bytes, &length);
            if (status != PIKA_STATUS_OK &&
                arguments[0].kind == PIKA_RUNTIME_VALUE_BYTES) {
                PikaObjectSlot* slot = pika_runtime_object(
                    context, arguments[0]);
                if (slot == NULL ||
                    (slot->length != 0u && slot->data == NULL)) {
                    return PIKA_STATUS_INVALID_OPERAND;
                }
                bytes = (const char*)slot->data;
                length = slot->length;
                status = PIKA_STATUS_OK;
            }
            if (status != PIKA_STATUS_OK) return status;
            if (length != 1u) {
                return PIKA_STATUS_TYPE_MISMATCH;
            }
            *result = integer_value((unsigned char)bytes[0]);
            return PIKA_STATUS_OK;
        }
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_PRINT_ENABLE
        case PIKA_BUILTIN_PRINT:
            return builtin_print(context, arguments, count, result);
#endif
#if PIKA_CAPABILITY_BUILTIN_HELP_ENABLE
        case PIKA_BUILTIN_HELP:
            return builtin_help(context, arguments, count, result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_ITER_RANGE_ENABLE
        case PIKA_BUILTIN_RANGE:
            return builtin_range(context, arguments, count, result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE
        case PIKA_BUILTIN_REPR:
            return value_text(context, arguments[0], 1, result);
        case PIKA_BUILTIN_REVERSED:
            return builtin_reversed(context, arguments[0], result);
        case PIKA_BUILTIN_ROUND:
            return builtin_round(arguments[0], count > 1u,
                                 count > 1u ? arguments[1] : none_value(), result);
        case PIKA_BUILTIN_SET:
            return builtin_set(
                context, count == 0u ? none_value() : arguments[0],
                count != 0u, result);
        case PIKA_BUILTIN_SLICE:
            return pika_runtime_create_slice(
                context, count > 0u ? arguments[0] : none_value(),
                count > 1u ? arguments[1] : none_value(),
                count > 2u ? arguments[2] : none_value(), result);
        case PIKA_BUILTIN_SORTED:
            return builtin_sorted(context, arguments[0], result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_SCALAR_CONVERT_ENABLE
        case PIKA_BUILTIN_STR:
            if (count == 0u) return pika_runtime_create_text(
                context, PIKA_RUNTIME_VALUE_STRING, NULL, 0u, result);
            if (arguments[0].kind == PIKA_RUNTIME_VALUE_STRING) {
                *result = arguments[0];
                return PIKA_STATUS_OK;
            }
            return value_text(context, arguments[0], 0, result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE
        case PIKA_BUILTIN_SUM:
            return builtin_sum(context, arguments[0],
                               count > 1u ? arguments[1] : integer_value(0), result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_CONTAINER_CONSTRUCT_ENABLE
        case PIKA_BUILTIN_TUPLE:
            return builtin_list_tuple(
                context, count == 0u ? none_value() : arguments[0],
                count != 0u, PIKA_RUNTIME_VALUE_TUPLE, result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE || \
    PIKA_CAPABILITY_BUILTIN_TYPE_ENABLE
        case PIKA_BUILTIN_TYPE:
            return builtin_type(context, arguments[0], result);
#endif
#if PIKA_CAPABILITY_BUILTIN_EMBEDDED_ENABLE
        case PIKA_BUILTIN_ZIP:
            return builtin_zip(context, arguments, count, result);
        case PIKA_BUILTIN_POW: {
            int64_t base_value;
            int64_t exponent;
            int64_t modulus;
            int64_t output;
            PikaStatus status;
            if (integer_argument(arguments[0], &base_value) != PIKA_STATUS_OK ||
                integer_argument(arguments[1], &exponent) != PIKA_STATUS_OK ||
                exponent < 0) return PIKA_STATUS_TYPE_MISMATCH;
            if (count == 3u) {
                if (integer_argument(arguments[2], &modulus) != PIKA_STATUS_OK ||
                    modulus == 0) return PIKA_STATUS_TYPE_MISMATCH;
                status = integer_power_modulo(
                    base_value, exponent, modulus, &output);
            } else {
                status = pika_runtime_integer_power(
                    base_value, exponent, &output);
            }
            if (status != PIKA_STATUS_OK) return status;
            *result = integer_value(output);
            return PIKA_STATUS_OK;
        }
        case PIKA_BUILTIN_ITER:
            if (arguments[0].kind == PIKA_RUNTIME_VALUE_ITERATOR) {
                *result = arguments[0];
                return PIKA_STATUS_OK;
            }
            if (!is_iterable_kind(arguments[0].kind)) {
                return PIKA_STATUS_TYPE_MISMATCH;
            }
            return pika_runtime_create_iterator(context, arguments[0], result);
        case PIKA_BUILTIN_NEXT:
            return builtin_next(
                context, arguments[0],
                count > 1u ? arguments[1] : none_value(), count > 1u, result);
        case PIKA_BUILTIN_GETATTR:
        case PIKA_BUILTIN_HASATTR:
        case PIKA_BUILTIN_MAP:
        case PIKA_BUILTIN_FILTER:
#endif
        default:
            return PIKA_STATUS_UNSUPPORTED_SYNTAX;
    }
}

PikaStatus pika_runtime_execute_builtin(PikaRuntimeContext* context,
                                        const PikaRuntimeFrame* frame,
                                        uint16_t builtin_id,
                                        uint8_t argument_base,
                                        uint8_t argument_count,
                                        uint8_t result_slot) {
    PikaRuntimeValue result;
    PikaStatus status;
    const PikaBuiltinDescriptor* descriptor;
    if (context == NULL || frame == NULL ||
        (context->typed_values == NULL && context->values == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if ((uint32_t)argument_base + argument_count > frame->slot_count ||
        result_slot >= frame->slot_count) {
        return PIKA_STATUS_INVALID_OPERAND;
    }
#if PIKA_CAPABILITY_PROTOCOL_TEXT_NETWORK_ENABLE
    if (builtin_id > PIKA_BUILTIN_CATALOG_COUNT) {
        const PikaTextMethodDescriptor* text_descriptor =
            pika_text_method_descriptor(builtin_id);
        if (text_descriptor != NULL) {
            if (argument_count < text_descriptor->minimum_arguments ||
                argument_count > text_descriptor->maximum_arguments) {
                return PIKA_STATUS_ARGUMENT_COUNT;
            }
            status = pika_runtime_execute_text_method(
                context, frame, builtin_id, argument_base,
                argument_count, &result);
            if (status == PIKA_STATUS_OK) {
                status = write_result(context, frame, result_slot, result);
            }
            if (status == PIKA_STATUS_OK && context->metrics != NULL) {
                context->metrics->builtin_call_count++;
            }
            return status;
        }
    }
#endif
    descriptor = pika_builtin_descriptor((PikaBuiltinId)builtin_id);
    if (descriptor == NULL) return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
    if (descriptor->implemented == 0u ||
        argument_count < descriptor->minimum_arguments ||
        argument_count > descriptor->maximum_arguments) {
        return PIKA_STATUS_ARGUMENT_COUNT;
    }
    status = builtin_dispatch(
        context, frame, (PikaBuiltinId)builtin_id, argument_base,
        argument_count, &result);
    if (status == PIKA_STATUS_OK) {
        status = write_result(context, frame, result_slot, result);
    }
    if (status == PIKA_STATUS_OK && context->metrics != NULL) {
        context->metrics->builtin_call_count++;
    }
    return status;
}

#endif
