/* SPEC: PJ2026-050111 portable packages v0.3; JSON string codec. */
#include "_json_binding.h"
#include "pika_capability_config.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if PIKA_CAPABILITY_LIBRARY_JSON_ENABLE

#ifndef PIKA_JSON_STRING_BYTE_LIMIT
#define PIKA_JSON_STRING_BYTE_LIMIT 240u
#endif
#ifndef PIKA_JSON_INPUT_BYTE_LIMIT
#define PIKA_JSON_INPUT_BYTE_LIMIT 1024u
#endif

#if PIKA_JSON_STRING_BYTE_LIMIT > PIKA_BINDING_RESULT_BYTE_LIMIT
#error "PIKA_JSON_STRING_BYTE_LIMIT exceeds PIKA_BINDING_RESULT_BYTE_LIMIT"
#endif

static uint8_t pika_json_string_output[PIKA_JSON_STRING_BYTE_LIMIT];
static PikaBindingValue pika_json_token_output[3];

static PikaStatus json_validate_call(const PikaBindingCall* call,
                                     PikaBindingValue* result) {
    if (call == NULL || result == NULL || call->argument_count != 1u ||
        call->arguments == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

static int json_hex_value(uint8_t value) {
    if (value >= (uint8_t)'0' && value <= (uint8_t)'9') {
        return (int)(value - (uint8_t)'0');
    }
    if (value >= (uint8_t)'a' && value <= (uint8_t)'f') {
        return (int)(value - (uint8_t)'a') + 10;
    }
    if (value >= (uint8_t)'A' && value <= (uint8_t)'F') {
        return (int)(value - (uint8_t)'A') + 10;
    }
    return -1;
}

static uint8_t json_hex_digit(uint8_t value) {
    return value < 10u ? (uint8_t)('0' + value)
                       : (uint8_t)('a' + value - 10u);
}

static PikaStatus json_append_byte(uint8_t value, uint32_t* length);

static PikaStatus json_append_unicode_escape(uint16_t value,
                                             uint32_t* length) {
    const uint8_t prefix[] = {'\\', 'u'};
    uint32_t index;
    PikaStatus status;
    for (index = 0u; index < sizeof(prefix); ++index) {
        status = json_append_byte(prefix[index], length);
        if (status != PIKA_STATUS_OK) return status;
    }
    for (index = 0u; index < 4u; ++index) {
        uint32_t shift = 12u - index * 4u;
        status = json_append_byte(
            json_hex_digit((uint8_t)((value >> shift) & 0x0fu)), length);
        if (status != PIKA_STATUS_OK) return status;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus json_decode_utf8(PikaBindingBuffer input,
                                   uint8_t first,
                                   uint32_t* source,
                                   uint32_t* codepoint) {
    uint32_t remaining;
    uint32_t minimum;
    uint32_t value;
    uint32_t index;
    if (first >= 0xc2u && first <= 0xdfu) {
        remaining = 1u;
        minimum = 0x80u;
        value = first & 0x1fu;
    } else if (first >= 0xe0u && first <= 0xefu) {
        remaining = 2u;
        minimum = 0x800u;
        value = first & 0x0fu;
    } else if (first >= 0xf0u && first <= 0xf4u) {
        remaining = 3u;
        minimum = 0x10000u;
        value = first & 0x07u;
    } else {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (*source > input.length || remaining > input.length - *source) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    for (index = 0u; index < remaining; ++index) {
        uint8_t next = input.data[(*source)++];
        if ((next & 0xc0u) != 0x80u) return PIKA_STATUS_VALUE_ERROR;
        value = (value << 6u) | (uint32_t)(next & 0x3fu);
    }
    if (value < minimum || value > 0x10ffffu ||
        (value >= 0xd800u && value <= 0xdfffu)) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    *codepoint = value;
    return PIKA_STATUS_OK;
}

static PikaStatus json_append_byte(uint8_t value,
                                   uint32_t* length) {
    if (*length >= PIKA_JSON_STRING_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    pika_json_string_output[(*length)++] = value;
    return PIKA_STATUS_OK;
}

static PikaStatus json_append_codepoint(uint32_t codepoint,
                                        uint32_t* length) {
    PikaStatus status;
    if (codepoint <= 0x7fu) {
        return json_append_byte((uint8_t)codepoint, length);
    }
    if (codepoint <= 0x7ffu) {
        status = json_append_byte(
            (uint8_t)(0xc0u | (codepoint >> 6)), length);
        return status == PIKA_STATUS_OK
                   ? json_append_byte(
                         (uint8_t)(0x80u | (codepoint & 0x3fu)), length)
                   : status;
    }
    if (codepoint <= 0xffffu) {
        status = json_append_byte(
            (uint8_t)(0xe0u | (codepoint >> 12)), length);
        if (status == PIKA_STATUS_OK) {
            status = json_append_byte(
                (uint8_t)(0x80u | ((codepoint >> 6) & 0x3fu)), length);
        }
        return status == PIKA_STATUS_OK
                   ? json_append_byte(
                         (uint8_t)(0x80u | (codepoint & 0x3fu)), length)
                   : status;
    }
    if (codepoint <= 0x10ffffu) {
        status = json_append_byte(
            (uint8_t)(0xf0u | (codepoint >> 18)), length);
        if (status == PIKA_STATUS_OK) {
            status = json_append_byte(
                (uint8_t)(0x80u | ((codepoint >> 12) & 0x3fu)), length);
        }
        if (status == PIKA_STATUS_OK) {
            status = json_append_byte(
                (uint8_t)(0x80u | ((codepoint >> 6) & 0x3fu)), length);
        }
        return status == PIKA_STATUS_OK
                   ? json_append_byte(
                         (uint8_t)(0x80u | (codepoint & 0x3fu)), length)
                   : status;
    }
    return PIKA_STATUS_VALUE_ERROR;
}

PikaStatus pika_binding__json_escape(const PikaBindingCall* call,
                                     PikaBindingValue* result) {
    PikaBindingBuffer input;
    uint32_t source;
    uint32_t length = 0u;
    PikaStatus status = json_validate_call(call, result);
    if (status != PIKA_STATUS_OK) return status;
    input = call->arguments[0].as.buffer;
    if ((input.length > 0u && input.data == NULL) ||
        input.length > PIKA_JSON_STRING_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    for (source = 0u; source < input.length; ++source) {
        uint8_t value = input.data[source];
        uint8_t escaped = 0u;
        if (value == (uint8_t)'"' || value == (uint8_t)'\\') {
            status = json_append_byte((uint8_t)'\\', &length);
            escaped = value;
        } else if (value == 8u) {
            escaped = (uint8_t)'b';
        } else if (value == 12u) {
            escaped = (uint8_t)'f';
        } else if (value == (uint8_t)'\n') {
            escaped = (uint8_t)'n';
        } else if (value == (uint8_t)'\r') {
            escaped = (uint8_t)'r';
        } else if (value == (uint8_t)'\t') {
            escaped = (uint8_t)'t';
        }
        if (status != PIKA_STATUS_OK) return status;
        if (escaped != 0u) {
            if (value != (uint8_t)'"' && value != (uint8_t)'\\') {
                status = json_append_byte((uint8_t)'\\', &length);
            }
            if (status == PIKA_STATUS_OK) {
                status = json_append_byte(escaped, &length);
            }
        } else if (value < 0x20u) {
            const uint8_t prefix[] = {'\\', 'u', '0', '0'};
            uint32_t index;
            for (index = 0u; index < sizeof(prefix); ++index) {
                status = json_append_byte(prefix[index], &length);
                if (status != PIKA_STATUS_OK) return status;
            }
            status = json_append_byte(
                json_hex_digit((uint8_t)(value >> 4)), &length);
            if (status == PIKA_STATUS_OK) {
                status = json_append_byte(
                    json_hex_digit((uint8_t)(value & 0x0fu)), &length);
            }
        } else if (value < 0x80u) {
            status = json_append_byte(value, &length);
        } else {
            uint32_t codepoint;
            uint32_t cursor = source + 1u;
            status = json_decode_utf8(input, value, &cursor, &codepoint);
            if (status == PIKA_STATUS_OK) {
                source = cursor - 1u;
            }
            if (status == PIKA_STATUS_OK && codepoint <= 0xffffu) {
                status = json_append_unicode_escape(
                    (uint16_t)codepoint, &length);
            } else if (status == PIKA_STATUS_OK) {
                uint32_t scalar = codepoint - 0x10000u;
                status = json_append_unicode_escape(
                    (uint16_t)(0xd800u + (scalar >> 10u)), &length);
                if (status == PIKA_STATUS_OK) {
                    status = json_append_unicode_escape(
                        (uint16_t)(0xdc00u + (scalar & 0x3ffu)), &length);
                }
            }
        }
        if (status != PIKA_STATUS_OK) return status;
    }
    result->kind = PIKA_BINDING_VALUE_STRING;
    result->as.buffer.data = pika_json_string_output;
    result->as.buffer.length = length;
    return PIKA_STATUS_OK;
}

static PikaStatus json_read_escape(PikaBindingBuffer input,
                                   uint32_t* position,
                                   uint32_t* codepoint) {
    uint32_t value = 0u;
    uint32_t index;
    if (*position + 4u > input.length) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    for (index = 0u; index < 4u; ++index) {
        int digit = json_hex_value(input.data[*position + index]);
        if (digit < 0) return PIKA_STATUS_VALUE_ERROR;
        value = (value << 4) | (uint32_t)digit;
    }
    *position += 4u;
    *codepoint = value;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__json_unescape(const PikaBindingCall* call,
                                       PikaBindingValue* result) {
    PikaBindingBuffer input;
    uint32_t source = 0u;
    uint32_t length = 0u;
    PikaStatus status = json_validate_call(call, result);
    if (status != PIKA_STATUS_OK) return status;
    input = call->arguments[0].as.buffer;
    if ((input.length > 0u && input.data == NULL) ||
        input.length > PIKA_JSON_STRING_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    while (source < input.length) {
        uint8_t value = input.data[source++];
        if (value < 0x20u) return PIKA_STATUS_VALUE_ERROR;
        if (value != (uint8_t)'\\') {
            status = json_append_byte(value, &length);
        } else {
            uint8_t escape;
            if (source >= input.length) return PIKA_STATUS_VALUE_ERROR;
            escape = input.data[source++];
            if (escape == (uint8_t)'"' || escape == (uint8_t)'\\' ||
                escape == (uint8_t)'/') {
                status = json_append_byte(escape, &length);
            } else if (escape == (uint8_t)'b') {
                status = json_append_byte(8u, &length);
            } else if (escape == (uint8_t)'f') {
                status = json_append_byte(12u, &length);
            } else if (escape == (uint8_t)'n') {
                status = json_append_byte((uint8_t)'\n', &length);
            } else if (escape == (uint8_t)'r') {
                status = json_append_byte((uint8_t)'\r', &length);
            } else if (escape == (uint8_t)'t') {
                status = json_append_byte((uint8_t)'\t', &length);
            } else if (escape == (uint8_t)'u') {
                uint32_t codepoint;
                status = json_read_escape(input, &source, &codepoint);
                if (status != PIKA_STATUS_OK) return status;
                if (codepoint >= 0xd800u && codepoint <= 0xdbffu) {
                    uint32_t low;
                    if (source + 2u > input.length ||
                        input.data[source] != (uint8_t)'\\' ||
                        input.data[source + 1u] != (uint8_t)'u') {
                        return PIKA_STATUS_VALUE_ERROR;
                    }
                    source += 2u;
                    status = json_read_escape(input, &source, &low);
                    if (status != PIKA_STATUS_OK || low < 0xdc00u ||
                        low > 0xdfffu) {
                        return PIKA_STATUS_VALUE_ERROR;
                    }
                    codepoint = 0x10000u +
                        ((codepoint - 0xd800u) << 10) +
                        (low - 0xdc00u);
                } else if (codepoint >= 0xdc00u && codepoint <= 0xdfffu) {
                    return PIKA_STATUS_VALUE_ERROR;
                }
                status = json_append_codepoint(codepoint, &length);
            } else {
                return PIKA_STATUS_VALUE_ERROR;
            }
        }
        if (status != PIKA_STATUS_OK) return status;
    }
    result->kind = PIKA_BINDING_VALUE_STRING;
    result->as.buffer.data = pika_json_string_output;
    result->as.buffer.length = length;
    return PIKA_STATUS_OK;
}

enum {
    PIKA_JSON_TOKEN_END = 0,
    PIKA_JSON_TOKEN_LEFT_BRACKET = 1,
    PIKA_JSON_TOKEN_RIGHT_BRACKET = 2,
    PIKA_JSON_TOKEN_LEFT_BRACE = 3,
    PIKA_JSON_TOKEN_RIGHT_BRACE = 4,
    PIKA_JSON_TOKEN_COMMA = 5,
    PIKA_JSON_TOKEN_COLON = 6,
    PIKA_JSON_TOKEN_STRING = 7,
    PIKA_JSON_TOKEN_INTEGER = 8,
    PIKA_JSON_TOKEN_FLOAT = 9,
    PIKA_JSON_TOKEN_TRUE = 10,
    PIKA_JSON_TOKEN_FALSE = 11,
    PIKA_JSON_TOKEN_NULL = 12,
};

static PikaStatus json_emit_token(int64_t kind,
                                  const PikaBindingValue* value,
                                  uint32_t position,
                                  PikaBindingValue* result) {
    memset(pika_json_token_output, 0, sizeof(pika_json_token_output));
    pika_json_token_output[0].kind = PIKA_BINDING_VALUE_INTEGER;
    pika_json_token_output[0].as.integer = kind;
    if (value != NULL) {
        pika_json_token_output[1] = *value;
    } else {
        pika_json_token_output[1].kind = PIKA_BINDING_VALUE_NONE;
    }
    pika_json_token_output[2].kind = PIKA_BINDING_VALUE_INTEGER;
    pika_json_token_output[2].as.integer = (int64_t)position;
    result->kind = PIKA_BINDING_VALUE_TUPLE;
    result->as.sequence.values = pika_json_token_output;
    result->as.sequence.length = 3u;
    return PIKA_STATUS_OK;
}

static int json_bytes_equal(PikaBindingBuffer input,
                            uint32_t position,
                            const char* expected,
                            uint32_t length) {
    return position <= input.length &&
           length <= input.length - position &&
           memcmp(&input.data[position], expected, length) == 0;
}

static PikaStatus json_scan_string(PikaBindingBuffer input,
                                   uint32_t position,
                                   PikaBindingValue* value,
                                   uint32_t* next) {
    PikaBindingValue argument;
    PikaBindingCall nested;
    uint32_t start = position + 1u;
    uint32_t cursor = start;
    while (cursor < input.length) {
        uint8_t current = input.data[cursor];
        if (current == (uint8_t)'"') {
            memset(&argument, 0, sizeof(argument));
            argument.kind = PIKA_BINDING_VALUE_STRING;
            argument.as.buffer.data = &input.data[start];
            argument.as.buffer.length = cursor - start;
            memset(&nested, 0, sizeof(nested));
            nested.arguments = &argument;
            nested.argument_count = 1u;
            *next = cursor + 1u;
            return pika_binding__json_unescape(&nested, value);
        }
        if (current < 0x20u) return PIKA_STATUS_VALUE_ERROR;
        if (current == (uint8_t)'\\') {
            ++cursor;
            if (cursor >= input.length) {
                return PIKA_STATUS_VALUE_ERROR;
            }
        }
        ++cursor;
    }
    return PIKA_STATUS_VALUE_ERROR;
}

static PikaStatus json_integer_value(PikaBindingBuffer input,
                                     uint32_t start,
                                     uint32_t end,
                                     PikaBindingValue* value) {
    uint32_t cursor = start;
    uint64_t magnitude = 0u;
    uint64_t limit = (uint64_t)INT64_MAX;
    int negative = 0;
    if (input.data[cursor] == (uint8_t)'-') {
        negative = 1;
        limit += 1u;
        ++cursor;
    }
    while (cursor < end) {
        uint64_t digit = (uint64_t)(input.data[cursor] - (uint8_t)'0');
        if (magnitude > (limit - digit) / 10u) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        magnitude = magnitude * 10u + digit;
        ++cursor;
    }
    value->kind = PIKA_BINDING_VALUE_INTEGER;
    if (!negative) {
        value->as.integer = (int64_t)magnitude;
    } else if (magnitude == (uint64_t)INT64_MAX + 1u) {
        value->as.integer = INT64_MIN;
    } else {
        value->as.integer = -(int64_t)magnitude;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus json_float_value(PikaBindingBuffer input,
                                   uint32_t start,
                                   uint32_t end,
                                   uint32_t exponent_position,
                                   PikaBindingValue* value) {
    uint32_t cursor = start;
    uint32_t mantissa_end = exponent_position < end
                                ? exponent_position
                                : end;
    uint32_t fractional_digits = 0u;
    uint32_t significant_digits = 0u;
    uint32_t kept_digits = 0u;
    double mantissa = 0.0;
    int after_decimal = 0;
    int significant = 0;
    int negative = 0;
    int32_t explicit_exponent = 0;
    int exponent_negative = 0;
    int32_t scale_exponent;
    double scale;
    double floating;
    if (input.data[cursor] == (uint8_t)'-') {
        negative = 1;
        ++cursor;
    }
    while (cursor < mantissa_end) {
        uint8_t current = input.data[cursor++];
        if (current == (uint8_t)'.') {
            after_decimal = 1;
            continue;
        }
        if (after_decimal) ++fractional_digits;
        if (!significant && current != (uint8_t)'0') {
            significant = 1;
        }
        if (significant) {
            ++significant_digits;
            if (kept_digits < 18u) {
                mantissa = mantissa * 10.0 +
                           (double)(current - (uint8_t)'0');
                ++kept_digits;
            }
        }
    }
    if (exponent_position < end) {
        cursor = exponent_position + 1u;
        if (input.data[cursor] == (uint8_t)'+' ||
            input.data[cursor] == (uint8_t)'-') {
            exponent_negative =
                input.data[cursor] == (uint8_t)'-';
            ++cursor;
        }
        while (cursor < end) {
            uint32_t digit =
                (uint32_t)(input.data[cursor] - (uint8_t)'0');
            if (explicit_exponent > 1000) {
                explicit_exponent = 1001;
            } else {
                explicit_exponent = explicit_exponent * 10 +
                                    (int32_t)digit;
            }
            ++cursor;
        }
        if (exponent_negative) explicit_exponent = -explicit_exponent;
    }
    scale_exponent = explicit_exponent - (int32_t)fractional_digits +
                     (int32_t)(significant_digits - kept_digits);
    if (mantissa == 0.0) {
        floating = negative ? -0.0 : 0.0;
    } else if (scale_exponent < -400) {
        floating = negative ? -0.0 : 0.0;
    } else {
        if (scale_exponent > 400) return PIKA_STATUS_VALUE_ERROR;
        scale = pow(10.0, (double)scale_exponent);
        floating = mantissa * scale;
        if (!isfinite(floating)) return PIKA_STATUS_VALUE_ERROR;
        if (negative) floating = -floating;
    }
    value->kind = PIKA_BINDING_VALUE_FLOAT;
    value->as.floating = floating;
    return PIKA_STATUS_OK;
}

static PikaStatus json_scan_number(PikaBindingBuffer input,
                                   uint32_t position,
                                   PikaBindingValue* value,
                                   uint32_t* next,
                                   int64_t* token_kind) {
    uint32_t start = position;
    uint32_t exponent_position = input.length;
    int floating = 0;
    PikaStatus status;
    if (input.data[position] == (uint8_t)'-') {
        ++position;
        if (position >= input.length) return PIKA_STATUS_VALUE_ERROR;
    }
    if (input.data[position] == (uint8_t)'0') {
        ++position;
        if (position < input.length &&
            input.data[position] >= (uint8_t)'0' &&
            input.data[position] <= (uint8_t)'9') {
            return PIKA_STATUS_VALUE_ERROR;
        }
    } else if (input.data[position] >= (uint8_t)'1' &&
               input.data[position] <= (uint8_t)'9') {
        do {
            ++position;
        } while (position < input.length &&
                 input.data[position] >= (uint8_t)'0' &&
                 input.data[position] <= (uint8_t)'9');
    } else {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (position < input.length &&
        input.data[position] == (uint8_t)'.') {
        floating = 1;
        ++position;
        if (position >= input.length ||
            input.data[position] < (uint8_t)'0' ||
            input.data[position] > (uint8_t)'9') {
            return PIKA_STATUS_VALUE_ERROR;
        }
        do {
            ++position;
        } while (position < input.length &&
                 input.data[position] >= (uint8_t)'0' &&
                 input.data[position] <= (uint8_t)'9');
    }
    if (position < input.length &&
        (input.data[position] == (uint8_t)'e' ||
         input.data[position] == (uint8_t)'E')) {
        floating = 1;
        exponent_position = position;
        ++position;
        if (position < input.length &&
            (input.data[position] == (uint8_t)'+' ||
             input.data[position] == (uint8_t)'-')) {
            ++position;
        }
        if (position >= input.length ||
            input.data[position] < (uint8_t)'0' ||
            input.data[position] > (uint8_t)'9') {
            return PIKA_STATUS_VALUE_ERROR;
        }
        do {
            ++position;
        } while (position < input.length &&
                 input.data[position] >= (uint8_t)'0' &&
                 input.data[position] <= (uint8_t)'9');
    }
    status = floating
                 ? json_float_value(
                       input, start, position,
                       exponent_position, value)
                 : json_integer_value(input, start, position, value);
    if (status != PIKA_STATUS_OK) return status;
    *next = position;
    *token_kind = floating ? PIKA_JSON_TOKEN_FLOAT
                           : PIKA_JSON_TOKEN_INTEGER;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__json_scan(const PikaBindingCall* call,
                                   PikaBindingValue* result) {
    PikaBindingBuffer input;
    PikaBindingValue value;
    uint32_t position;
    uint32_t next;
    int64_t token_kind;
    PikaStatus status;
    if (call == NULL || result == NULL ||
        call->arguments == NULL || call->argument_count != 2u ||
        call->arguments[1].as.integer < 0) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    input = call->arguments[0].as.buffer;
    if ((input.length > 0u && input.data == NULL) ||
        input.length > PIKA_JSON_INPUT_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    if ((uint64_t)call->arguments[1].as.integer > input.length) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    position = (uint32_t)call->arguments[1].as.integer;
    while (position < input.length &&
           (input.data[position] == (uint8_t)' ' ||
            input.data[position] == (uint8_t)'\t' ||
            input.data[position] == (uint8_t)'\r' ||
            input.data[position] == (uint8_t)'\n')) {
        ++position;
    }
    if (position == input.length) {
        return json_emit_token(
            PIKA_JSON_TOKEN_END, NULL, position, result);
    }
    memset(&value, 0, sizeof(value));
    next = position + 1u;
    if (input.data[position] == (uint8_t)'[') {
        token_kind = PIKA_JSON_TOKEN_LEFT_BRACKET;
    } else if (input.data[position] == (uint8_t)']') {
        token_kind = PIKA_JSON_TOKEN_RIGHT_BRACKET;
    } else if (input.data[position] == (uint8_t)'{') {
        token_kind = PIKA_JSON_TOKEN_LEFT_BRACE;
    } else if (input.data[position] == (uint8_t)'}') {
        token_kind = PIKA_JSON_TOKEN_RIGHT_BRACE;
    } else if (input.data[position] == (uint8_t)',') {
        token_kind = PIKA_JSON_TOKEN_COMMA;
    } else if (input.data[position] == (uint8_t)':') {
        token_kind = PIKA_JSON_TOKEN_COLON;
    } else if (input.data[position] == (uint8_t)'"') {
        status = json_scan_string(input, position, &value, &next);
        if (status != PIKA_STATUS_OK) return status;
        token_kind = PIKA_JSON_TOKEN_STRING;
    } else if (json_bytes_equal(input, position, "true", 4u)) {
        value.kind = PIKA_BINDING_VALUE_BOOLEAN;
        value.as.boolean = 1u;
        next = position + 4u;
        token_kind = PIKA_JSON_TOKEN_TRUE;
    } else if (json_bytes_equal(input, position, "false", 5u)) {
        value.kind = PIKA_BINDING_VALUE_BOOLEAN;
        value.as.boolean = 0u;
        next = position + 5u;
        token_kind = PIKA_JSON_TOKEN_FALSE;
    } else if (json_bytes_equal(input, position, "null", 4u)) {
        value.kind = PIKA_BINDING_VALUE_NONE;
        next = position + 4u;
        token_kind = PIKA_JSON_TOKEN_NULL;
    } else if (input.data[position] == (uint8_t)'-' ||
               (input.data[position] >= (uint8_t)'0' &&
                input.data[position] <= (uint8_t)'9')) {
        status = json_scan_number(
            input, position, &value, &next, &token_kind);
        if (status != PIKA_STATUS_OK) return status;
    } else {
        return PIKA_STATUS_VALUE_ERROR;
    }
    return json_emit_token(token_kind, &value, next, result);
}

#endif
