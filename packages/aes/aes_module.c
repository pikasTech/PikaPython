/* SPEC: PJ2026-050111 portable packages v0.3; AES binding. */
#include "_aes_binding.h"
#include "aes_algorithm.h"
#include "pika_capability_config.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if PIKA_CAPABILITY_LIBRARY_AES_ENABLE

#define PIKA_AES_MODE_ECB 0
#define PIKA_AES_MODE_CBC 1

#ifndef PIKA_AES_OBJECT_LIMIT
#define PIKA_AES_OBJECT_LIMIT 4u
#endif

#ifndef PIKA_AES_INPUT_BYTE_LIMIT
#define PIKA_AES_INPUT_BYTE_LIMIT PIKA_BINDING_RESULT_BYTE_LIMIT
#endif

#if PIKA_AES_INPUT_BYTE_LIMIT > PIKA_BINDING_RESULT_BYTE_LIMIT
#error "PIKA_AES_INPUT_BYTE_LIMIT exceeds PIKA_BINDING_RESULT_BYTE_LIMIT"
#endif

typedef struct {
    PikaAesContext context;
    uint8_t initialization_vector[16];
    uint8_t mode;
    uint8_t active;
} PikaAesObject;

static PikaAesObject aes_objects[PIKA_AES_OBJECT_LIMIT];
static uint8_t aes_output[PIKA_AES_INPUT_BYTE_LIMIT];

static PikaAesObject* aes_allocate(void) {
    uint32_t index;
    for (index = 0u; index < PIKA_AES_OBJECT_LIMIT; ++index) {
        if (aes_objects[index].active == 0u) {
            memset(&aes_objects[index], 0, sizeof(aes_objects[index]));
            aes_objects[index].active = 1u;
            return &aes_objects[index];
        }
    }
    return NULL;
}

static PikaAesObject* aes_from_instance(void* instance) {
    uint32_t index;
    for (index = 0u; index < PIKA_AES_OBJECT_LIMIT; ++index) {
        if (instance == &aes_objects[index] &&
            aes_objects[index].active != 0u) {
            return &aes_objects[index];
        }
    }
    return NULL;
}

static PikaStatus aes_bytes(
    const PikaBindingValue* value,
    PikaBindingBuffer* buffer) {
    if (value == NULL || buffer == NULL ||
        value->kind != PIKA_BINDING_VALUE_BYTES) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    *buffer = value->as.buffer;
    if (buffer->length > 0u && buffer->data == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__aes_AES_init(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    PikaBindingBuffer key;
    PikaAesContext initialized;
    PikaAesObject* object;
    int64_t mode;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 3u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = aes_bytes(&call->arguments[0], &key);
    if (status != PIKA_STATUS_OK) return status;
    if (call->arguments[1].kind != PIKA_BINDING_VALUE_INTEGER) {
        return PIKA_STATUS_TYPE_MISMATCH;
    }
    mode = call->arguments[1].as.integer;
    if (mode != PIKA_AES_MODE_ECB && mode != PIKA_AES_MODE_CBC) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (mode == PIKA_AES_MODE_ECB) {
        if (call->arguments[2].kind != PIKA_BINDING_VALUE_NONE) {
            return PIKA_STATUS_VALUE_ERROR;
        }
    } else {
        PikaBindingBuffer iv;
        if (call->arguments[2].kind == PIKA_BINDING_VALUE_NONE) {
            return PIKA_STATUS_VALUE_ERROR;
        }
        status = aes_bytes(&call->arguments[2], &iv);
        if (status != PIKA_STATUS_OK) return status;
        if (iv.length != 16u) return PIKA_STATUS_VALUE_ERROR;
    }
    status = pika_aes_initialize(&initialized, key.data, key.length);
    if (status != PIKA_STATUS_OK) return status;
    object = aes_allocate();
    if (object == NULL) return PIKA_STATUS_STORAGE_TOO_SMALL;
    object->context = initialized;
    object->mode = (uint8_t)mode;
    if (mode == PIKA_AES_MODE_CBC) {
        memcpy(object->initialization_vector,
               call->arguments[2].as.buffer.data, 16u);
    }
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_OPAQUE;
    result->as.opaque = object;
    return PIKA_STATUS_OK;
}

static PikaStatus aes_transform(
    const PikaBindingCall* call,
    PikaBindingValue* result,
    int decrypt) {
    PikaAesObject* object;
    PikaBindingBuffer input;
    uint8_t vector[16] = {0};
    uint32_t offset;
    PikaStatus status;
    if (call == NULL || result == NULL || call->arguments == NULL ||
        call->argument_count != 1u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object = aes_from_instance(call->instance);
    if (object == NULL) return PIKA_STATUS_INVALID_OPERAND;
    status = aes_bytes(&call->arguments[0], &input);
    if (status != PIKA_STATUS_OK) return status;
    if (input.length > PIKA_AES_INPUT_BYTE_LIMIT) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    if ((input.length & 15u) != 0u) return PIKA_STATUS_VALUE_ERROR;
    if (input.length > 0u) {
        memcpy(aes_output, input.data, input.length);
    }
    if (object->mode == PIKA_AES_MODE_CBC) {
        memcpy(vector, object->initialization_vector, sizeof(vector));
    }
    for (offset = 0u; offset < input.length; offset += 16u) {
        uint8_t* block = aes_output + offset;
        uint32_t index;
        if (object->mode == PIKA_AES_MODE_ECB) {
            if (decrypt != 0) {
                pika_aes_decrypt_block(&object->context, block);
            } else {
                pika_aes_encrypt_block(&object->context, block);
            }
        } else if (decrypt != 0) {
            uint8_t encrypted[16];
            memcpy(encrypted, block, sizeof(encrypted));
            pika_aes_decrypt_block(&object->context, block);
            for (index = 0u; index < 16u; ++index) {
                block[index] ^= vector[index];
            }
            memcpy(vector, encrypted, sizeof(vector));
        } else {
            for (index = 0u; index < 16u; ++index) {
                block[index] ^= vector[index];
            }
            pika_aes_encrypt_block(&object->context, block);
            memcpy(vector, block, sizeof(vector));
        }
    }
    memset(result, 0, sizeof(*result));
    result->kind = PIKA_BINDING_VALUE_BYTES;
    result->as.buffer.data = aes_output;
    result->as.buffer.length = input.length;
    return PIKA_STATUS_OK;
}

PikaStatus pika_binding__aes_AES_encrypt(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    return aes_transform(call, result, 0);
}

PikaStatus pika_binding__aes_AES_decrypt(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    return aes_transform(call, result, 1);
}

void pika_binding__aes_AES_destroy(
    void* module_context,
    void* instance) {
    PikaAesObject* object = aes_from_instance(instance);
    (void)module_context;
    if (object != NULL) memset(object, 0, sizeof(*object));
}

#endif
