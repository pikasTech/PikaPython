/* SPEC: PJ2026-050111 portable packages v0.2; hexadecimal codec binding. */
#include "binascii_binding.h"
#include "pika_capability_config.h"

#include <stddef.h>
#include <stdint.h>

#if PIKA_CAPABILITY_LIBRARY_BINASCII_ENABLE

#ifndef PIKA_BINASCII_INPUT_BYTE_LIMIT
#define PIKA_BINASCII_INPUT_BYTE_LIMIT 128u
#endif

#ifndef PIKA_BINASCII_OUTPUT_BYTE_LIMIT
#define PIKA_BINASCII_OUTPUT_BYTE_LIMIT 256u
#endif

#if PIKA_BINASCII_OUTPUT_BYTE_LIMIT > PIKA_BINDING_RESULT_BYTE_LIMIT
#error "PIKA_BINASCII_OUTPUT_BYTE_LIMIT exceeds PIKA_BINDING_RESULT_BYTE_LIMIT"
#endif

static uint8_t pika_binascii_output[PIKA_BINASCII_OUTPUT_BYTE_LIMIT];

static PikaStatus binascii_validate(const PikaBindingCall* call,
                                    PikaBindingValue* result) {
    if (call == NULL || result == NULL || call->argument_count != 1u ||
        call->arguments == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus binascii_read_buffer(const PikaBindingValue* value,
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
    if (buffer->length > PIKA_BINASCII_INPUT_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    return PIKA_STATUS_OK;
}

static uint8_t binascii_hex_digit(uint8_t value) {
    return value < 10u ? (uint8_t)('0' + value)
                       : (uint8_t)('a' + value - 10u);
}

static int binascii_hex_value(uint8_t value) {
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

static PikaStatus binascii_encode(const PikaBindingCall* call,
                                  PikaBindingValue* result) {
    PikaBindingBuffer input;
    uint32_t index;
    PikaStatus status = binascii_validate(call, result);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = binascii_read_buffer(&call->arguments[0], 1, &input);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (input.length > PIKA_BINASCII_OUTPUT_BYTE_LIMIT / 2u) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    for (index = 0u; index < input.length; ++index) {
        pika_binascii_output[index * 2u] =
            binascii_hex_digit((uint8_t)(input.data[index] >> 4));
        pika_binascii_output[index * 2u + 1u] =
            binascii_hex_digit((uint8_t)(input.data[index] & 0x0fu));
    }
    result->kind = PIKA_BINDING_VALUE_BYTES;
    result->as.buffer.data = pika_binascii_output;
    result->as.buffer.length = input.length * 2u;
    return PIKA_STATUS_OK;
}

static PikaStatus binascii_decode(const PikaBindingCall* call,
                                  PikaBindingValue* result) {
    PikaBindingBuffer input;
    uint32_t index;
    PikaStatus status = binascii_validate(call, result);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = binascii_read_buffer(&call->arguments[0], 0, &input);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if ((input.length & 1u) != 0u) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (input.length / 2u > PIKA_BINASCII_OUTPUT_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    for (index = 0u; index < input.length; index += 2u) {
        int high = binascii_hex_value(input.data[index]);
        int low = binascii_hex_value(input.data[index + 1u]);
        if (high < 0 || low < 0) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        pika_binascii_output[index / 2u] =
            (uint8_t)((uint8_t)high << 4) | (uint8_t)low;
    }
    result->kind = PIKA_BINDING_VALUE_BYTES;
    result->as.buffer.data = pika_binascii_output;
    result->as.buffer.length = input.length / 2u;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding_binascii_b2a_hex(const PikaBindingCall* call,
                                         PikaBindingValue* result) {
    return binascii_encode(call, result);
}

PikaStatus pika_binding_binascii_hexlify(const PikaBindingCall* call,
                                         PikaBindingValue* result) {
    return binascii_encode(call, result);
}

PikaStatus pika_binding_binascii_a2b_hex(const PikaBindingCall* call,
                                         PikaBindingValue* result) {
    return binascii_decode(call, result);
}

PikaStatus pika_binding_binascii_unhexlify(const PikaBindingCall* call,
                                           PikaBindingValue* result) {
    return binascii_decode(call, result);
}

#endif
