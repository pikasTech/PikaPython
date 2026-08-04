/* SPEC: PJ2026-050111 portable packages v0.3; bounded struct codec. */
#include "_struct_binding.h"
#include "pika_capability_config.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if PIKA_CAPABILITY_LIBRARY_STRUCT_BASIC_ENABLE

#ifndef PIKA_STRUCT_FORMAT_BYTE_LIMIT
#define PIKA_STRUCT_FORMAT_BYTE_LIMIT 64u
#endif

#ifndef PIKA_STRUCT_FIELD_LIMIT
#define PIKA_STRUCT_FIELD_LIMIT 32u
#endif

#ifndef PIKA_STRUCT_ARGUMENT_LIMIT
#define PIKA_STRUCT_ARGUMENT_LIMIT PIKA_BINDING_CONTAINER_ELEMENT_LIMIT
#endif

#ifndef PIKA_STRUCT_OUTPUT_BYTE_LIMIT
#define PIKA_STRUCT_OUTPUT_BYTE_LIMIT PIKA_BINDING_RESULT_BYTE_LIMIT
#endif

#if PIKA_STRUCT_ARGUMENT_LIMIT > PIKA_BINDING_CONTAINER_ELEMENT_LIMIT
#error "PIKA_STRUCT_ARGUMENT_LIMIT exceeds PIKA_BINDING_CONTAINER_ELEMENT_LIMIT"
#endif

#if PIKA_STRUCT_OUTPUT_BYTE_LIMIT > PIKA_BINDING_RESULT_BYTE_LIMIT
#error "PIKA_STRUCT_OUTPUT_BYTE_LIMIT exceeds PIKA_BINDING_RESULT_BYTE_LIMIT"
#endif

typedef enum {
    PIKA_STRUCT_ENDIAN_NATIVE = 0,
    PIKA_STRUCT_ENDIAN_LITTLE = 1,
    PIKA_STRUCT_ENDIAN_BIG = 2
} PikaStructEndian;

typedef struct {
    uint8_t code;
    uint8_t width;
    uint8_t alignment;
    uint8_t reserved;
    uint32_t count;
    uint32_t offset;
} PikaStructField;

typedef struct {
    PikaStructField fields[PIKA_STRUCT_FIELD_LIMIT];
    uint32_t field_count;
    uint32_t value_count;
    uint32_t size;
    PikaStructEndian endian;
    uint8_t native_layout;
} PikaStructLayout;

static uint8_t pika_struct_output[PIKA_STRUCT_OUTPUT_BYTE_LIMIT];
static PikaBindingValue pika_struct_values[PIKA_STRUCT_ARGUMENT_LIMIT];

static int struct_host_is_little(void) {
    const uint16_t value = 1u;
    return *((const uint8_t*)&value) == 1u;
}

static PikaStatus struct_checked_add(uint32_t left,
                                     uint32_t right,
                                     uint32_t* result) {
    if (result == NULL || left > UINT32_MAX - right) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    *result = left + right;
    return PIKA_STATUS_OK;
}

static PikaStatus struct_checked_multiply(uint32_t left,
                                          uint32_t right,
                                          uint32_t* result) {
    if (result == NULL || (right != 0u && left > UINT32_MAX / right)) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    *result = left * right;
    return PIKA_STATUS_OK;
}

static PikaStatus struct_align(uint32_t value,
                               uint32_t alignment,
                               uint32_t* result) {
    uint32_t remainder;
    if (alignment <= 1u) {
        *result = value;
        return PIKA_STATUS_OK;
    }
    remainder = value % alignment;
    return remainder == 0u
               ? ((*result = value), PIKA_STATUS_OK)
               : struct_checked_add(value, alignment - remainder, result);
}

static PikaStatus struct_type_info(uint8_t code,
                                   int native_layout,
                                   uint8_t* width,
                                   uint8_t* alignment) {
    uint32_t size = 0u;
    switch (code) {
        case 'x':
        case 'c':
        case 'b':
        case 'B':
        case '?': size = 1u; break;
        case 'h':
        case 'H': size = native_layout ? sizeof(short) : 2u; break;
        case 'i':
        case 'I': size = native_layout ? sizeof(int) : 4u; break;
        case 'l':
        case 'L': size = native_layout ? sizeof(long) : 4u; break;
        case 'q':
        case 'Q': size = sizeof(long long); break;
        case 'f': size = sizeof(float); break;
        case 'd': size = sizeof(double); break;
        case 's': size = 1u; break;
        default: return PIKA_STATUS_UNSUPPORTED_FORMAT;
    }
    if (size == 0u || size > 8u ||
        ((code == 'f' && size != 4u) ||
         (code == 'd' && size != 8u))) {
        return PIKA_STATUS_UNSUPPORTED_FORMAT;
    }
    *width = (uint8_t)size;
    *alignment = native_layout != 0 ? (uint8_t)size : 1u;
    return PIKA_STATUS_OK;
}

static PikaStatus struct_parse_layout(PikaBindingBuffer format,
                                      PikaStructLayout* layout) {
    uint32_t position = 0u;
    uint32_t offset = 0u;
    if (layout == NULL ||
        (format.length > 0u && format.data == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (format.length > PIKA_STRUCT_FORMAT_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    memset(layout, 0, sizeof(*layout));
    layout->native_layout = 1u;
    layout->endian = struct_host_is_little()
                         ? PIKA_STRUCT_ENDIAN_LITTLE
                         : PIKA_STRUCT_ENDIAN_BIG;
    if (format.length > 0u) {
        uint8_t prefix = format.data[0];
        if (prefix == '@' || prefix == '=' || prefix == '<' ||
            prefix == '>' || prefix == '!') {
            position = 1u;
            layout->native_layout = prefix == '@' ? 1u : 0u;
            if (prefix == '<') {
                layout->endian = PIKA_STRUCT_ENDIAN_LITTLE;
            } else if (prefix == '>' || prefix == '!') {
                layout->endian = PIKA_STRUCT_ENDIAN_BIG;
            }
        }
    }
    while (position < format.length) {
        uint32_t count = 0u;
        uint32_t field_bytes;
        uint8_t code;
        uint8_t width;
        uint8_t alignment;
        int has_count = 0;
        PikaStructField* field;
        while (position < format.length &&
               format.data[position] >= (uint8_t)'0' &&
               format.data[position] <= (uint8_t)'9') {
            uint32_t digit =
                (uint32_t)(format.data[position] - (uint8_t)'0');
            has_count = 1;
            if (count > (UINT32_MAX - digit) / 10u) {
                return PIKA_STATUS_STORAGE_TOO_SMALL;
            }
            count = count * 10u + digit;
            ++position;
        }
        if (position >= format.length) {
            return PIKA_STATUS_UNSUPPORTED_FORMAT;
        }
        code = format.data[position++];
        if (code == 0u || code == ' ' || code == '\t' ||
            code == '\r' || code == '\n') {
            return PIKA_STATUS_UNSUPPORTED_FORMAT;
        }
        if (!has_count) count = 1u;
        if (layout->field_count >= PIKA_STRUCT_FIELD_LIMIT) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        if (struct_type_info(
                code, layout->native_layout, &width, &alignment) !=
            PIKA_STATUS_OK) {
            return PIKA_STATUS_UNSUPPORTED_FORMAT;
        }
        if (struct_align(offset, alignment, &offset) != PIKA_STATUS_OK ||
            struct_checked_multiply(
                count, width, &field_bytes) != PIKA_STATUS_OK ||
            struct_checked_add(offset, field_bytes, &layout->size) !=
                PIKA_STATUS_OK) {
            return PIKA_STATUS_STORAGE_TOO_SMALL;
        }
        field = &layout->fields[layout->field_count++];
        field->code = code;
        field->width = width;
        field->alignment = alignment;
        field->count = count;
        field->offset = offset;
        offset = layout->size;
        if (code != 'x') {
            uint32_t added = code == 's' ? 1u : count;
            if (struct_checked_add(
                    layout->value_count, added,
                    &layout->value_count) != PIKA_STATUS_OK ||
                layout->value_count > PIKA_STRUCT_ARGUMENT_LIMIT) {
                return PIKA_STATUS_STORAGE_TOO_SMALL;
            }
        }
    }
    if (layout->size > PIKA_STRUCT_OUTPUT_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    return PIKA_STATUS_OK;
}

static void struct_store_unsigned(uint8_t* destination,
                                  uint8_t width,
                                  uint64_t value,
                                  PikaStructEndian endian) {
    uint32_t index;
    for (index = 0u; index < width; ++index) {
        uint32_t shift = endian == PIKA_STRUCT_ENDIAN_LITTLE
                             ? index * 8u
                             : (uint32_t)(width - 1u - index) * 8u;
        destination[index] = (uint8_t)(value >> shift);
    }
}

static uint64_t struct_load_unsigned(const uint8_t* source,
                                     uint8_t width,
                                     PikaStructEndian endian) {
    uint64_t value = 0u;
    uint32_t index;
    for (index = 0u; index < width; ++index) {
        uint32_t shift = endian == PIKA_STRUCT_ENDIAN_LITTLE
                             ? index * 8u
                             : (uint32_t)(width - 1u - index) * 8u;
        value |= (uint64_t)source[index] << shift;
    }
    return value;
}

static int struct_is_signed(uint8_t code) {
    return code == 'b' || code == 'h' || code == 'i' ||
           code == 'l' || code == 'q';
}

static int struct_is_integer(uint8_t code) {
    return struct_is_signed(code) || code == 'B' || code == 'H' ||
           code == 'I' || code == 'L' || code == 'Q';
}

static PikaStatus struct_pack_integer(const PikaBindingValue* value,
                                      uint8_t code,
                                      uint8_t width,
                                      uint8_t* destination,
                                      PikaStructEndian endian) {
    int64_t integer;
    uint64_t bits;
    if (value->kind != PIKA_BINDING_VALUE_INTEGER &&
        value->kind != PIKA_BINDING_VALUE_BOOLEAN) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    integer = value->kind == PIKA_BINDING_VALUE_BOOLEAN
                  ? (value->as.boolean != 0u ? 1 : 0)
                  : value->as.integer;
    if (struct_is_signed(code)) {
        if (width < 8u) {
            int64_t minimum = -(INT64_C(1) << (width * 8u - 1u));
            int64_t maximum =
                (INT64_C(1) << (width * 8u - 1u)) - 1;
            if (integer < minimum || integer > maximum) {
                return PIKA_STATUS_VALUE_ERROR;
            }
        }
    } else {
        uint64_t maximum = width == 8u
                               ? UINT64_MAX
                               : (UINT64_C(1) << (width * 8u)) - 1u;
        if (integer < 0 || (uint64_t)integer > maximum) {
            return PIKA_STATUS_VALUE_ERROR;
        }
    }
    bits = (uint64_t)integer;
    struct_store_unsigned(destination, width, bits, endian);
    return PIKA_STATUS_OK;
}

static PikaStatus struct_pack_float(const PikaBindingValue* value,
                                    uint8_t code,
                                    uint8_t* destination,
                                    PikaStructEndian endian) {
    double floating;
    if (value->kind == PIKA_BINDING_VALUE_FLOAT) {
        floating = value->as.floating;
    } else if (value->kind == PIKA_BINDING_VALUE_INTEGER) {
        floating = (double)value->as.integer;
    } else if (value->kind == PIKA_BINDING_VALUE_BOOLEAN) {
        floating = value->as.boolean != 0u ? 1.0 : 0.0;
    } else {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (code == 'f') {
        float narrowed = (float)floating;
        uint32_t bits;
        memcpy(&bits, &narrowed, sizeof(bits));
        struct_store_unsigned(destination, 4u, bits, endian);
    } else {
        uint64_t bits;
        memcpy(&bits, &floating, sizeof(bits));
        struct_store_unsigned(destination, 8u, bits, endian);
    }
    return PIKA_STATUS_OK;
}

static PikaStatus struct_pack_layout(const PikaStructLayout* layout,
                                     PikaBindingSequence values,
                                     PikaBindingValue* result) {
    uint32_t field_index;
    uint32_t value_index = 0u;
    memset(pika_struct_output, 0, layout->size);
    if (values.length != layout->value_count ||
        (values.length > 0u && values.values == NULL)) {
        return PIKA_STATUS_ARGUMENT_COUNT;
    }
    for (field_index = 0u;
         field_index < layout->field_count;
         ++field_index) {
        const PikaStructField* field = &layout->fields[field_index];
        uint32_t item;
        if (field->code == 'x') continue;
        if (field->code == 's') {
            const PikaBindingValue* value = &values.values[value_index++];
            uint32_t copied;
            if (value->kind != PIKA_BINDING_VALUE_BYTES &&
                value->kind != PIKA_BINDING_VALUE_BYTEARRAY) {
                return PIKA_STATUS_TYPE_MISMATCH;
            }
            copied = value->as.buffer.length < field->count
                         ? value->as.buffer.length
                         : field->count;
            if (copied > 0u) {
                memcpy(&pika_struct_output[field->offset],
                       value->as.buffer.data, copied);
            }
            continue;
        }
        for (item = 0u; item < field->count; ++item) {
            const PikaBindingValue* value = &values.values[value_index++];
            uint8_t* destination =
                &pika_struct_output[field->offset + item * field->width];
            PikaStatus status;
            if (field->code == 'c') {
                if (value->kind != PIKA_BINDING_VALUE_BYTES ||
                    value->as.buffer.length != 1u) {
                    return PIKA_STATUS_TYPE_MISMATCH;
                }
                destination[0] = value->as.buffer.data[0];
                continue;
            }
            if (field->code == '?') {
                if (value->kind == PIKA_BINDING_VALUE_BOOLEAN) {
                    destination[0] = value->as.boolean != 0u ? 1u : 0u;
                } else if (value->kind == PIKA_BINDING_VALUE_INTEGER) {
                    destination[0] = value->as.integer != 0 ? 1u : 0u;
                } else {
                    return PIKA_STATUS_TYPE_MISMATCH;
                }
                continue;
            }
            status = struct_is_integer(field->code)
                         ? struct_pack_integer(
                               value, field->code, field->width,
                               destination, layout->endian)
                         : struct_pack_float(
                               value, field->code, destination,
                               layout->endian);
            if (status != PIKA_STATUS_OK) return status;
        }
    }
    result->kind = PIKA_BINDING_VALUE_BYTES;
    result->as.buffer.data = pika_struct_output;
    result->as.buffer.length = layout->size;
    return PIKA_STATUS_OK;
}

static PikaStatus struct_unpack_layout(const PikaStructLayout* layout,
                                       PikaBindingBuffer buffer,
                                       uint32_t offset,
                                       PikaBindingValue* result) {
    uint32_t field_index;
    uint32_t value_index = 0u;
    if ((buffer.length > 0u && buffer.data == NULL) ||
        offset > buffer.length ||
        layout->size > buffer.length - offset) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    for (field_index = 0u;
         field_index < layout->field_count;
         ++field_index) {
        const PikaStructField* field = &layout->fields[field_index];
        uint32_t item;
        if (field->code == 'x') continue;
        if (field->code == 's') {
            PikaBindingValue* value = &pika_struct_values[value_index++];
            value->kind = PIKA_BINDING_VALUE_BYTES;
            value->as.buffer.data =
                &buffer.data[offset + field->offset];
            value->as.buffer.length = field->count;
            continue;
        }
        for (item = 0u; item < field->count; ++item) {
            const uint8_t* source =
                &buffer.data[offset + field->offset + item * field->width];
            PikaBindingValue* value = &pika_struct_values[value_index++];
            memset(value, 0, sizeof(*value));
            if (field->code == 'c') {
                value->kind = PIKA_BINDING_VALUE_BYTES;
                value->as.buffer.data = source;
                value->as.buffer.length = 1u;
            } else if (field->code == '?') {
                value->kind = PIKA_BINDING_VALUE_BOOLEAN;
                value->as.boolean = source[0] != 0u ? 1u : 0u;
            } else if (field->code == 'f') {
                uint32_t bits = (uint32_t)struct_load_unsigned(
                    source, 4u, layout->endian);
                float floating;
                memcpy(&floating, &bits, sizeof(floating));
                value->kind = PIKA_BINDING_VALUE_FLOAT;
                value->as.floating = floating;
            } else if (field->code == 'd') {
                uint64_t bits = struct_load_unsigned(
                    source, 8u, layout->endian);
                value->kind = PIKA_BINDING_VALUE_FLOAT;
                memcpy(&value->as.floating, &bits, sizeof(bits));
            } else {
                uint64_t integer = struct_load_unsigned(
                    source, field->width, layout->endian);
                if (struct_is_signed(field->code) &&
                    field->width < 8u &&
                    (integer &
                     (UINT64_C(1) << (field->width * 8u - 1u))) != 0u) {
                    integer |= UINT64_MAX << (field->width * 8u);
                }
                if (!struct_is_signed(field->code) &&
                    integer > (uint64_t)INT64_MAX) {
                    return PIKA_STATUS_VALUE_ERROR;
                }
                value->kind = PIKA_BINDING_VALUE_INTEGER;
                value->as.integer = (int64_t)integer;
            }
        }
    }
    result->kind = PIKA_BINDING_VALUE_TUPLE;
    result->as.sequence.values = pika_struct_values;
    result->as.sequence.length = value_index;
    return PIKA_STATUS_OK;
}

static PikaStatus struct_validate_call(const PikaBindingCall* call,
                                       PikaBindingValue* result,
                                       uint8_t count) {
    if (call == NULL || result == NULL ||
        call->argument_count != count ||
        (count > 0u && call->arguments == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus struct_public_status(PikaStatus status) {
    return status == PIKA_STATUS_UNSUPPORTED_FORMAT
               ? PIKA_STATUS_VALUE_ERROR
               : status;
}

PikaStatus pika_binding__struct_pack_values(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaStructLayout layout;
    PikaStatus status = struct_validate_call(call, result, 2u);
    if (status == PIKA_STATUS_OK) {
        status = struct_parse_layout(
            call->arguments[0].as.buffer, &layout);
    }
    if (status == PIKA_STATUS_OK) {
        status = struct_pack_layout(
            &layout, call->arguments[1].as.sequence, result);
    }
    return struct_public_status(status);
}

PikaStatus pika_binding__struct_unpack(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaStructLayout layout;
    PikaStatus status = struct_validate_call(call, result, 2u);
    if (status == PIKA_STATUS_OK) {
        status = struct_parse_layout(
            call->arguments[0].as.buffer, &layout);
    }
    if (status == PIKA_STATUS_OK) {
        if (call->arguments[1].kind != PIKA_BINDING_VALUE_BYTES &&
            call->arguments[1].kind != PIKA_BINDING_VALUE_BYTEARRAY) {
            return PIKA_STATUS_TYPE_MISMATCH;
        }
        if (call->arguments[1].as.buffer.length != layout.size) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        status = struct_unpack_layout(
            &layout, call->arguments[1].as.buffer, 0u, result);
    }
    return struct_public_status(status);
}

PikaStatus pika_binding__struct_unpack_from(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaStructLayout layout;
    int64_t offset;
    PikaStatus status = struct_validate_call(call, result, 3u);
    if (status != PIKA_STATUS_OK) return status;
    offset = call->arguments[2].as.integer;
    if (call->arguments[1].kind != PIKA_BINDING_VALUE_BYTES &&
        call->arguments[1].kind != PIKA_BINDING_VALUE_BYTEARRAY) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    if (offset < 0) {
        if ((uint64_t)(-(offset + 1)) + 1u >
            call->arguments[1].as.buffer.length) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        offset += call->arguments[1].as.buffer.length;
    }
    if ((uint64_t)offset > UINT32_MAX) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    status = struct_parse_layout(
        call->arguments[0].as.buffer, &layout);
    if (status == PIKA_STATUS_OK) {
        status = struct_unpack_layout(
            &layout, call->arguments[1].as.buffer,
            (uint32_t)offset, result);
    }
    return struct_public_status(status);
}

PikaStatus pika_binding__struct_calcsize(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaStructLayout layout;
    PikaStatus status = struct_validate_call(call, result, 1u);
    if (status == PIKA_STATUS_OK) {
        status = struct_parse_layout(
            call->arguments[0].as.buffer, &layout);
    }
    if (status == PIKA_STATUS_OK) {
        result->kind = PIKA_BINDING_VALUE_INTEGER;
        result->as.integer = layout.size;
    }
    return struct_public_status(status);
}

#endif
