/* SPEC: PJ2026-050111 portable packages v0.3; HMAC binding. */
#include "_hmac_binding.h"
#include "hash_algorithms.h"
#include "pika_capability_config.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if PIKA_CAPABILITY_LIBRARY_HMAC_ENABLE

#ifndef PIKA_HMAC_OBJECT_LIMIT
#define PIKA_HMAC_OBJECT_LIMIT 4u
#endif

#ifndef PIKA_HMAC_INPUT_BYTE_LIMIT
#define PIKA_HMAC_INPUT_BYTE_LIMIT 4096u
#endif

#ifndef PIKA_HMAC_KEY_BYTE_LIMIT
#define PIKA_HMAC_KEY_BYTE_LIMIT 1024u
#endif

typedef struct {
    PikaHashContext inner;
    PikaHashContext outer;
    uint8_t digest[64];
    uint8_t hexadecimal[129];
    uint8_t active;
} PikaHmacObject;

static PikaHmacObject hmac_objects[PIKA_HMAC_OBJECT_LIMIT];

static PikaHmacObject* hmac_allocate(void) {
    uint32_t index;
    for (index = 0u; index < PIKA_HMAC_OBJECT_LIMIT; ++index) {
        if (hmac_objects[index].active == 0u) {
            memset(&hmac_objects[index], 0, sizeof(hmac_objects[index]));
            hmac_objects[index].active = 1u;
            return &hmac_objects[index];
        }
    }
    return NULL;
}

static PikaHmacObject* hmac_from_instance(void* instance) {
    uint32_t index;
    for (index = 0u; index < PIKA_HMAC_OBJECT_LIMIT; ++index) {
        if (instance == &hmac_objects[index] &&
            hmac_objects[index].active != 0u) {
            return &hmac_objects[index];
        }
    }
    return NULL;
}

static PikaStatus hmac_bytes(
    const PikaBindingValue* value,
    uint32_t limit,
    PikaBindingBuffer* buffer) {
    if (value == NULL || buffer == NULL ||
        (value->kind != PIKA_BINDING_VALUE_BYTES &&
         value->kind != PIKA_BINDING_VALUE_BYTEARRAY)) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    *buffer = value->as.buffer;
    if (buffer->length > limit) return PIKA_STATUS_STORAGE_TOO_SMALL;
    if (buffer->length > 0u && buffer->data == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

static PikaStatus hmac_initialize(
    PikaHmacObject* object,
    PikaHashAlgorithm algorithm,
    PikaBindingBuffer key,
    PikaBindingBuffer message) {
    uint8_t key_block[128];
    uint8_t inner_pad[128];
    uint8_t outer_pad[128];
    uint8_t reduced_key[64];
    uint32_t block_size = pika_hash_block_size(algorithm);
    uint32_t index;
    PikaStatus status;
    if (object == NULL || block_size == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    memset(key_block, 0, sizeof(key_block));
    if (key.length > block_size) {
        PikaHashContext key_hash;
        uint32_t reduced_length = pika_hash_digest_size(algorithm);
        status = pika_hash_initialize(&key_hash, algorithm);
        if (status == PIKA_STATUS_OK) {
            status = pika_hash_update(&key_hash, key.data, key.length);
        }
        if (status == PIKA_STATUS_OK) {
            status = pika_hash_finalize(
                &key_hash, reduced_key, sizeof(reduced_key));
        }
        if (status != PIKA_STATUS_OK) return status;
        memcpy(key_block, reduced_key, reduced_length);
    } else if (key.length > 0u) {
        memcpy(key_block, key.data, key.length);
    }
    for (index = 0u; index < block_size; ++index) {
        inner_pad[index] = (uint8_t)(key_block[index] ^ 0x36u);
        outer_pad[index] = (uint8_t)(key_block[index] ^ 0x5cu);
    }
    status = pika_hash_initialize(&object->inner, algorithm);
    if (status == PIKA_STATUS_OK) {
        status = pika_hash_update(&object->inner, inner_pad, block_size);
    }
    if (status == PIKA_STATUS_OK) {
        status = pika_hash_update(
            &object->inner, message.data, message.length);
    }
    if (status == PIKA_STATUS_OK) {
        status = pika_hash_initialize(&object->outer, algorithm);
    }
    if (status == PIKA_STATUS_OK) {
        status = pika_hash_update(&object->outer, outer_pad, block_size);
    }
    memset(key_block, 0, sizeof(key_block));
    memset(inner_pad, 0, sizeof(inner_pad));
    memset(outer_pad, 0, sizeof(outer_pad));
    memset(reduced_key, 0, sizeof(reduced_key));
    return status;
}

static PikaStatus hmac_digest(PikaHmacObject* object) {
    PikaHashContext outer;
    uint8_t inner_digest[64];
    uint32_t length;
    PikaStatus status;
    if (object == NULL) return PIKA_STATUS_INVALID_ARGUMENT;
    length = pika_hash_digest_size(object->inner.algorithm);
    status = pika_hash_finalize(
        &object->inner, inner_digest, sizeof(inner_digest));
    if (status != PIKA_STATUS_OK) return status;
    outer = object->outer;
    status = pika_hash_update(&outer, inner_digest, length);
    if (status == PIKA_STATUS_OK) {
        status = pika_hash_finalize(
            &outer, object->digest, sizeof(object->digest));
    }
    memset(inner_digest, 0, sizeof(inner_digest));
    return status;
}

PikaStatus pika_binding__hmac_HMAC_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaBindingBuffer key;
    PikaBindingBuffer message;
    PikaBindingBuffer name;
    PikaHashAlgorithm algorithm;
    PikaHmacObject initialized;
    PikaHmacObject* object;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 3u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = hmac_bytes(
        &call->arguments[0], PIKA_HMAC_KEY_BYTE_LIMIT, &key);
    if (status == PIKA_STATUS_OK) {
        status = hmac_bytes(
            &call->arguments[1], PIKA_HMAC_INPUT_BYTE_LIMIT, &message);
    }
    if (status != PIKA_STATUS_OK) return status;
    if (call->arguments[2].kind != PIKA_BINDING_VALUE_STRING) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    name = call->arguments[2].as.buffer;
    if (name.length > 0u && name.data == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = pika_hash_algorithm_from_name(
        name.data, name.length, &algorithm);
    if (status == PIKA_STATUS_UNSUPPORTED_FORMAT) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (status != PIKA_STATUS_OK) return status;
    memset(&initialized, 0, sizeof(initialized));
    status = hmac_initialize(&initialized, algorithm, key, message);
    if (status != PIKA_STATUS_OK) return status;
    object = hmac_allocate();
    if (object == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    object->inner = initialized.inner;
    object->outer = initialized.outer;
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_OPAQUE;
    result->as.opaque = object;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__hmac_HMAC_update(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaHmacObject* object;
    PikaBindingBuffer message;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 1u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = hmac_from_instance(call->instance);
    if (object == NULL) return PIKA_STATUS_INVALID_OPERAND;
    status = hmac_bytes(
        &call->arguments[0], PIKA_HMAC_INPUT_BYTE_LIMIT, &message);
    if (status == PIKA_STATUS_OK) {
        status = pika_hash_update(
            &object->inner, message.data, message.length);
    }
    if (status != PIKA_STATUS_OK) return status;
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__hmac_HMAC_digest(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaHmacObject* object;
    PikaStatus status;
    if (call == NULL || result == NULL || call->argument_count != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = hmac_from_instance(call->instance);
    if (object == NULL) return PIKA_STATUS_INVALID_OPERAND;
    status = hmac_digest(object);
    if (status != PIKA_STATUS_OK) return status;
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_BYTES;
    result->as.buffer.data = object->digest;
    result->as.buffer.length =
        pika_hash_digest_size(object->inner.algorithm);
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__hmac_HMAC_hexdigest(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    static const uint8_t hexadecimal[] = "0123456789abcdef";
    PikaHmacObject* object;
    uint32_t length;
    uint32_t index;
    PikaStatus status;
    if (call == NULL || result == NULL || call->argument_count != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = hmac_from_instance(call->instance);
    if (object == NULL) return PIKA_STATUS_INVALID_OPERAND;
    status = hmac_digest(object);
    if (status != PIKA_STATUS_OK) return status;
    length = pika_hash_digest_size(object->inner.algorithm);
    for (index = 0u; index < length; ++index) {
        object->hexadecimal[index * 2u] =
            hexadecimal[object->digest[index] >> 4u];
        object->hexadecimal[index * 2u + 1u] =
            hexadecimal[object->digest[index] & 0x0fu];
    }
    object->hexadecimal[length * 2u] = 0u;
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_STRING;
    result->as.buffer.data = object->hexadecimal;
    result->as.buffer.length = length * 2u;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__hmac_HMAC_copy(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaHmacObject* source;
    PikaHmacObject* copy;
    if (call == NULL || result == NULL || call->argument_count != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    source = hmac_from_instance(call->instance);
    if (source == NULL) return PIKA_STATUS_INVALID_OPERAND;
    copy = hmac_allocate();
    if (copy == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    copy->inner = source->inner;
    copy->outer = source->outer;
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_OBJECT;
    result->as.opaque = copy;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__hmac_compare_digest(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaBindingBuffer left;
    PikaBindingBuffer right;
    uint8_t difference = 0u;
    uint32_t index;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 2u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = hmac_bytes(
        &call->arguments[0], PIKA_HMAC_INPUT_BYTE_LIMIT, &left);
    if (status == PIKA_STATUS_OK) {
        status = hmac_bytes(
            &call->arguments[1], PIKA_HMAC_INPUT_BYTE_LIMIT, &right);
    }
    if (status != PIKA_STATUS_OK) return status;
    if (left.length == right.length) {
        for (index = 0u; index < left.length; ++index) {
            difference |= (uint8_t)(left.data[index] ^ right.data[index]);
        }
    } else {
        difference = 1u;
    }
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_BOOLEAN;
    result->as.boolean = difference == 0u ? 1u : 0u;
    return PIKA_STATUS_OK;
}

void pika_binding__hmac_HMAC_destroy(
    void* module_context,
    void* instance) {
    PikaHmacObject* object = hmac_from_instance(instance);
    (void)module_context;
    if (object != NULL) memset(object, 0, sizeof(*object));
}

#endif
