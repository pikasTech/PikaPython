/* SPEC: PJ2026-050111 portable packages v0.2; RFC 1950 zlib binding. */
#include "_zlib_binding.h"
#include "pika_capability_config.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "uzlib.h"

#if PIKA_CAPABILITY_LIBRARY_ZLIB_ENABLE

#ifndef PIKA_ZLIB_INPUT_BYTE_LIMIT
#define PIKA_ZLIB_INPUT_BYTE_LIMIT 192u
#endif

#ifndef PIKA_ZLIB_OUTPUT_BYTE_LIMIT
#define PIKA_ZLIB_OUTPUT_BYTE_LIMIT 256u
#endif

#ifndef PIKA_ZLIB_HISTORY_BYTE_LIMIT
#define PIKA_ZLIB_HISTORY_BYTE_LIMIT 256u
#endif

#if PIKA_ZLIB_OUTPUT_BYTE_LIMIT > PIKA_BINDING_RESULT_BYTE_LIMIT
#error "PIKA_ZLIB_OUTPUT_BYTE_LIMIT exceeds PIKA_BINDING_RESULT_BYTE_LIMIT"
#endif

#if PIKA_ZLIB_HISTORY_BYTE_LIMIT == 0u ||                              \
    (PIKA_ZLIB_HISTORY_BYTE_LIMIT & (PIKA_ZLIB_HISTORY_BYTE_LIMIT - 1u)) != 0u
#error "PIKA_ZLIB_HISTORY_BYTE_LIMIT must be a non-zero power of two"
#endif

typedef struct {
    uint8_t* data;
    uint32_t capacity;
    uint32_t length;
    uint8_t overflow;
} PikaZlibWriter;

static uint8_t pika_zlib_output[PIKA_ZLIB_OUTPUT_BYTE_LIMIT + 1u];
static uint8_t pika_zlib_history[PIKA_ZLIB_HISTORY_BYTE_LIMIT];
static uzlib_uncomp_t pika_zlib_decoder;

static PikaStatus zlib_validate(const PikaBindingCall* call,
                                PikaBindingValue* result,
                                uint8_t count) {
    if (call == NULL || result == NULL || call->argument_count != count ||
        (count > 0u && call->arguments == NULL)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus zlib_input(const PikaBindingValue* value,
                             PikaBindingBuffer* input) {
    if (value == NULL || input == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (value->kind != PIKA_BINDING_VALUE_BYTES &&
        value->kind != PIKA_BINDING_VALUE_BYTEARRAY) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    *input = value->as.buffer;
    if (input->length > 0u && input->data == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (input->length > PIKA_ZLIB_INPUT_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    return PIKA_STATUS_OK;
}

static void zlib_write_byte(void* context, uint8_t value) {
    PikaZlibWriter* writer = (PikaZlibWriter*)context;
    if (writer->length < writer->capacity) {
        writer->data[writer->length++] = value;
    } else {
        writer->overflow = 1u;
    }
}

static void zlib_write_u32_be(PikaZlibWriter* writer, uint32_t value) {
    zlib_write_byte(writer, (uint8_t)(value >> 24));
    zlib_write_byte(writer, (uint8_t)(value >> 16));
    zlib_write_byte(writer, (uint8_t)(value >> 8));
    zlib_write_byte(writer, (uint8_t)value);
}

PikaStatus pika_binding__zlib_compress(const PikaBindingCall* call,
                                       PikaBindingValue* result) {
    PikaBindingBuffer input;
    PikaZlibWriter writer;
    uzlib_lz77_state_t compressor;
    int64_t level;
    uint32_t checksum;
    PikaStatus status = zlib_validate(call, result, 2u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = zlib_input(&call->arguments[0], &input);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    level = call->arguments[1].as.integer;
    if (level < 0 || level > 9) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    memset(&writer, 0, sizeof(writer));
    writer.data = pika_zlib_output;
    writer.capacity = PIKA_ZLIB_OUTPUT_BYTE_LIMIT;
    zlib_write_byte(&writer, 0x18u);
    zlib_write_byte(&writer, 0x19u);
    memset(pika_zlib_history, 0, sizeof(pika_zlib_history));
    uzlib_lz77_init(
        &compressor, pika_zlib_history, sizeof(pika_zlib_history));
    compressor.dest_write_data = &writer;
    compressor.dest_write_cb = zlib_write_byte;
    uzlib_start_block(&compressor);
    uzlib_lz77_compress(&compressor, input.data, input.length);
    uzlib_finish_block(&compressor);
    checksum = uzlib_adler32(input.data, input.length, 1u);
    zlib_write_u32_be(&writer, checksum);
    if (writer.overflow != 0u) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    result->kind = PIKA_BINDING_VALUE_BYTES;
    result->as.buffer.data = pika_zlib_output;
    result->as.buffer.length = writer.length;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__zlib_decompress(const PikaBindingCall* call,
                                         PikaBindingValue* result) {
    PikaBindingBuffer input;
    int header_bits = 0;
    int header_type;
    int decode_status;
    uint32_t output_length;
    PikaStatus status = zlib_validate(call, result, 1u);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    status = zlib_input(&call->arguments[0], &input);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (input.length < 6u) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    memset(&pika_zlib_decoder, 0, sizeof(pika_zlib_decoder));
    pika_zlib_decoder.source = input.data;
    pika_zlib_decoder.source_limit = input.data + input.length;
    header_type =
        uzlib_parse_zlib_gzip_header(&pika_zlib_decoder, &header_bits);
    if (header_type != UZLIB_HEADER_ZLIB || pika_zlib_decoder.eof ||
        header_bits < 8 || header_bits > 15) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    memset(pika_zlib_history, 0, sizeof(pika_zlib_history));
    uzlib_uncompress_init(
        &pika_zlib_decoder, pika_zlib_history, sizeof(pika_zlib_history));
    pika_zlib_decoder.dest_start = pika_zlib_output;
    pika_zlib_decoder.dest = pika_zlib_output;
    pika_zlib_decoder.dest_limit =
        pika_zlib_output + PIKA_ZLIB_OUTPUT_BYTE_LIMIT + 1u;
    decode_status = uzlib_uncompress_chksum(&pika_zlib_decoder);
    if (decode_status == UZLIB_OK) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    if (decode_status != UZLIB_DONE) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    output_length =
        (uint32_t)(pika_zlib_decoder.dest - pika_zlib_output);
    if (output_length > PIKA_ZLIB_OUTPUT_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    result->kind = PIKA_BINDING_VALUE_BYTES;
    result->as.buffer.data = pika_zlib_output;
    result->as.buffer.length = output_length;
    return PIKA_STATUS_OK;
}

#endif
