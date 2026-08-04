/* SPEC: PJ2026-050111 portable packages v0.2; RFC 4648 codec binding. */
#include "_base64_binding.h"
#include "pika_capability_config.h"

#include <stddef.h>
#include <stdint.h>

#if PIKA_CAPABILITY_LIBRARY_BASE64_ENABLE

#ifndef PIKA_BASE64_INPUT_BYTE_LIMIT
#define PIKA_BASE64_INPUT_BYTE_LIMIT 192u
#endif

#ifndef PIKA_BASE64_OUTPUT_BYTE_LIMIT
#define PIKA_BASE64_OUTPUT_BYTE_LIMIT 256u
#endif

#if PIKA_BASE64_OUTPUT_BYTE_LIMIT > PIKA_BINDING_RESULT_BYTE_LIMIT
#error "PIKA_BASE64_OUTPUT_BYTE_LIMIT exceeds PIKA_BINDING_RESULT_BYTE_LIMIT"
#endif

static uint8_t pika_base64_output[PIKA_BASE64_OUTPUT_BYTE_LIMIT];
static const uint8_t pika_base64_alphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static PikaStatus base64_validate(const PikaBindingCall* call,
                                  PikaBindingValue* result) {
    if (call == NULL || result == NULL || call->argument_count != 1u ||
        call->arguments == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus base64_read_buffer(const PikaBindingValue* value,
                                     int bytes_only,
                                     PikaBindingBuffer* buffer) {
    int binary;
    if (value == NULL || buffer == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    binary = value->kind == PIKA_BINDING_VALUE_BYTES ||
             value->kind == PIKA_BINDING_VALUE_BYTEARRAY;
    if (!binary &&
        (bytes_only != 0 || value->kind != PIKA_BINDING_VALUE_STRING)) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    *buffer = value->as.buffer;
    if (buffer->length > 0u && buffer->data == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (buffer->length > PIKA_BASE64_INPUT_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    return PIKA_STATUS_OK;
}

static int base64_value(uint8_t input) {
    if (input >= (uint8_t)'A' && input <= (uint8_t)'Z') {
        return (int)(input - (uint8_t)'A');
    }
    if (input >= (uint8_t)'a' && input <= (uint8_t)'z') {
        return (int)(input - (uint8_t)'a') + 26;
    }
    if (input >= (uint8_t)'0' && input <= (uint8_t)'9') {
        return (int)(input - (uint8_t)'0') + 52;
    }
    if (input == (uint8_t)'+') {
        return 62;
    }
    if (input == (uint8_t)'/') {
        return 63;
    }
    return -1;
}

PikaStatus pika_binding__base64_b64encode(const PikaBindingCall* call,
                                          PikaBindingValue* result) {
    PikaBindingBuffer input;
    uint32_t source = 0u;
    uint32_t destination = 0u;
    uint32_t required;
    PikaStatus status = base64_validate(call, result);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = base64_read_buffer(&call->arguments[0], 1, &input);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    required = ((input.length + 2u) / 3u) * 4u;
    if (required > PIKA_BASE64_OUTPUT_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    while (source + 3u <= input.length) {
        uint32_t value = ((uint32_t)input.data[source] << 16) |
                         ((uint32_t)input.data[source + 1u] << 8) |
                         (uint32_t)input.data[source + 2u];
        pika_base64_output[destination++] =
            pika_base64_alphabet[(value >> 18) & 0x3fu];
        pika_base64_output[destination++] =
            pika_base64_alphabet[(value >> 12) & 0x3fu];
        pika_base64_output[destination++] =
            pika_base64_alphabet[(value >> 6) & 0x3fu];
        pika_base64_output[destination++] =
            pika_base64_alphabet[value & 0x3fu];
        source += 3u;
    }
    if (source < input.length) {
        uint32_t value = (uint32_t)input.data[source] << 16;
        pika_base64_output[destination++] =
            pika_base64_alphabet[(value >> 18) & 0x3fu];
        if (source + 1u < input.length) {
            value |= (uint32_t)input.data[source + 1u] << 8;
            pika_base64_output[destination++] =
                pika_base64_alphabet[(value >> 12) & 0x3fu];
            pika_base64_output[destination++] =
                pika_base64_alphabet[(value >> 6) & 0x3fu];
        } else {
            pika_base64_output[destination++] =
                pika_base64_alphabet[(value >> 12) & 0x3fu];
            pika_base64_output[destination++] = (uint8_t)'=';
        }
        pika_base64_output[destination++] = (uint8_t)'=';
    }
    result->kind = PIKA_BINDING_VALUE_BYTES;
    result->as.buffer.data = pika_base64_output;
    result->as.buffer.length = destination;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__base64_b64decode(const PikaBindingCall* call,
                                          PikaBindingValue* result) {
    PikaBindingBuffer input;
    uint32_t source;
    uint32_t destination = 0u;
    uint8_t quartet[4];
    uint32_t quartet_length = 0u;
    int pending_double_padding = 0;
    int finished = 0;
    PikaStatus status = base64_validate(call, result);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = base64_read_buffer(&call->arguments[0], 0, &input);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    for (source = 0u; source < input.length && !finished; ++source) {
        uint8_t current = input.data[source];
        int decoded = base64_value(current);
        if (decoded >= 0) {
            uint32_t value;
            pending_double_padding = 0;
            quartet[quartet_length++] = (uint8_t)decoded;
            if (quartet_length != 4u) continue;
            if (destination + 3u > PIKA_BASE64_OUTPUT_BYTE_LIMIT) {
                return PIKA_STATUS_STORAGE_TOO_SMALL;
            }
            value = ((uint32_t)quartet[0] << 18) |
                    ((uint32_t)quartet[1] << 12) |
                    ((uint32_t)quartet[2] << 6) |
                    (uint32_t)quartet[3];
            pika_base64_output[destination++] = (uint8_t)(value >> 16);
            pika_base64_output[destination++] = (uint8_t)(value >> 8);
            pika_base64_output[destination++] = (uint8_t)value;
            quartet_length = 0u;
        } else if (current == (uint8_t)'=' && quartet_length == 2u) {
            if (!pending_double_padding) {
                pending_double_padding = 1;
                continue;
            }
            if (destination >= PIKA_BASE64_OUTPUT_BYTE_LIMIT) {
                return PIKA_STATUS_STORAGE_TOO_SMALL;
            }
            pika_base64_output[destination++] =
                (uint8_t)(((uint32_t)quartet[0] << 2) |
                          ((uint32_t)quartet[1] >> 4));
            finished = 1;
        } else if (current == (uint8_t)'=' && quartet_length == 3u) {
            uint32_t value;
            if (destination + 2u > PIKA_BASE64_OUTPUT_BYTE_LIMIT) {
                return PIKA_STATUS_STORAGE_TOO_SMALL;
            }
            value = ((uint32_t)quartet[0] << 10) |
                    ((uint32_t)quartet[1] << 4) |
                    ((uint32_t)quartet[2] >> 2);
            pika_base64_output[destination++] = (uint8_t)(value >> 8);
            pika_base64_output[destination++] = (uint8_t)value;
            finished = 1;
        }
    }
    if (!finished && quartet_length != 0u) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    result->kind = PIKA_BINDING_VALUE_BYTES;
    result->as.buffer.data = pika_base64_output;
    result->as.buffer.length = destination;
    return PIKA_STATUS_OK;
}

#endif
