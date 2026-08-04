/* SPEC: PJ2026-050111 portable packages v0.3; hashlib binding. */
#include "_hashlib_binding.h"
#include "hash_algorithms.h"
#include "pika_capability_config.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if PIKA_CAPABILITY_LIBRARY_HASHLIB_ENABLE

#ifndef PIKA_HASHLIB_OBJECT_LIMIT
#define PIKA_HASHLIB_OBJECT_LIMIT 4u
#endif

#ifndef PIKA_HASHLIB_INPUT_BYTE_LIMIT
#define PIKA_HASHLIB_INPUT_BYTE_LIMIT 4096u
#endif

typedef struct {
    PikaHashContext context;
    uint8_t digest[64];
    uint8_t hexadecimal[129];
    uint8_t active;
} PikaHashObject;

static PikaHashObject hash_objects[PIKA_HASHLIB_OBJECT_LIMIT];

static PikaHashObject* hash_allocate(void) {
    uint32_t index;
    for (index = 0u; index < PIKA_HASHLIB_OBJECT_LIMIT; ++index) {
        if (hash_objects[index].active == 0u) {
            memset(&hash_objects[index], 0, sizeof(hash_objects[index]));
            hash_objects[index].active = 1u;
            return &hash_objects[index];
        }
    }
    return NULL;
}

static PikaHashObject* hash_from_instance(void* instance) {
    uint32_t index;
    for (index = 0u; index < PIKA_HASHLIB_OBJECT_LIMIT; ++index) {
        if (instance == &hash_objects[index] &&
            hash_objects[index].active != 0u) {
            return &hash_objects[index];
        }
    }
    return NULL;
}

static PikaStatus hash_data_argument(
    const PikaBindingValue* value,
    PikaBindingBuffer* buffer) {
    if (value == NULL || buffer == NULL ||
        (value->kind != PIKA_BINDING_VALUE_BYTES &&
         value->kind != PIKA_BINDING_VALUE_BYTEARRAY)) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    *buffer = value->as.buffer;
    if ((buffer->length > 0u && buffer->data == NULL) ||
        buffer->length > PIKA_HASHLIB_INPUT_BYTE_LIMIT) {
        return buffer->length > PIKA_HASHLIB_INPUT_BYTE_LIMIT
                   ? PIKA_STATUS_STORAGE_TOO_SMALL
                   : PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__hashlib_Hash_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaHashAlgorithm algorithm;
    PikaHashContext initialized;
    PikaBindingBuffer name;
    PikaBindingBuffer data;
    PikaHashObject* object;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 2u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (call->arguments[0].kind != PIKA_BINDING_VALUE_STRING) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    name = call->arguments[0].as.buffer;
    if (name.length > 0u && name.data == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = hash_data_argument(&call->arguments[1], &data);
    if (status != PIKA_STATUS_OK) return status;
    status = pika_hash_algorithm_from_name(
        name.data, name.length, &algorithm);
    if (status == PIKA_STATUS_UNSUPPORTED_FORMAT) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (status != PIKA_STATUS_OK) return status;
    status = pika_hash_initialize(&initialized, algorithm);
    if (status == PIKA_STATUS_OK) {
        status = pika_hash_update(&initialized, data.data, data.length);
    }
    if (status != PIKA_STATUS_OK) return status;
    object = hash_allocate();
    if (object == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    object->context = initialized;
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_OPAQUE;
    result->as.opaque = object;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__hashlib_Hash_update(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaHashObject* object;
    PikaBindingBuffer data;
    PikaStatus status;
    if (call == NULL || result == NULL || call->argument_count != 1u ||
        call->arguments == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = hash_from_instance(call->instance);
    if (object == NULL) return PIKA_STATUS_INVALID_OPERAND;
    status = hash_data_argument(&call->arguments[0], &data);
    if (status != PIKA_STATUS_OK) return status;
    status = pika_hash_update(&object->context, data.data, data.length);
    if (status != PIKA_STATUS_OK) return status;
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_NONE;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__hashlib_Hash_digest(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaHashObject* object;
    uint32_t length;
    PikaStatus status;
    if (call == NULL || result == NULL || call->argument_count != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = hash_from_instance(call->instance);
    if (object == NULL) return PIKA_STATUS_INVALID_OPERAND;
    length = pika_hash_digest_size(object->context.algorithm);
    status = pika_hash_finalize(
        &object->context, object->digest, sizeof(object->digest));
    if (status != PIKA_STATUS_OK) return status;
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_BYTES;
    result->as.buffer.data = object->digest;
    result->as.buffer.length = length;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__hashlib_Hash_hexdigest(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    static const uint8_t hexadecimal[] = "0123456789abcdef";
    PikaHashObject* object;
    uint32_t length;
    uint32_t index;
    PikaStatus status;
    if (call == NULL || result == NULL || call->argument_count != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = hash_from_instance(call->instance);
    if (object == NULL) return PIKA_STATUS_INVALID_OPERAND;
    length = pika_hash_digest_size(object->context.algorithm);
    status = pika_hash_finalize(
        &object->context, object->digest, sizeof(object->digest));
    if (status != PIKA_STATUS_OK) return status;
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

PikaStatus pika_binding__hashlib_Hash_copy(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaHashObject* source;
    PikaHashObject* copy;
    if (call == NULL || result == NULL || call->argument_count != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    source = hash_from_instance(call->instance);
    if (source == NULL) return PIKA_STATUS_INVALID_OPERAND;
    copy = hash_allocate();
    if (copy == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    copy->context = source->context;
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_OBJECT;
    result->as.opaque = copy;
    return PIKA_STATUS_OK;
}

void pika_binding__hashlib_Hash_destroy(
    void* module_context,
    void* instance) {
    PikaHashObject* object = hash_from_instance(instance);
    (void)module_context;
    if (object != NULL) memset(object, 0, sizeof(*object));
}

#endif
