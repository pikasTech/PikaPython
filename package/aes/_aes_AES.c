#include "_aes_AES.h"

#include "mbedtls/aes.h"

enum {
    MODE_ECB,
    MODE_CBC,
} pika_aes_t;

void _aes_AES___init__(PikaObj* self) {
    obj_setInt(self, "MODE_ECB", MODE_ECB);
    obj_setInt(self, "MODE_CBC", MODE_CBC);
}

void _aes_AES_new(PikaObj* self, Arg* password, int mode, Arg* iv) {
    ArgType t;
    mbedtls_aes_context context;

    t = arg_getType(password);
    if (ARG_TYPE_BYTES != t) {
        obj_setErrorCode(self, -2);  // io error
    }

    t = arg_getType(iv);
    if (ARG_TYPE_NONE != t) {
        if (ARG_TYPE_BYTES != t) {
            obj_setErrorCode(self, -2);  // io error
        } else {
            size_t len = arg_getBytesSize(iv);
            if (len != 16) {
                obj_setErrorCode(self, -2);  // io error
            } else {
                obj_setBytes(self, "_iv", arg_getBytes(iv), len);
            }
        }
    }

    obj_setBytes(self, "_password", arg_getBytes(password),
                 arg_getBytesSize(password));
    obj_setInt(self, "_mode", mode);
    mbedtls_aes_init(&context);
    obj_setStruct(self, "context", context);
}

Arg* _aes_AES_encrypt(PikaObj* self, Arg* msg) {
    ArgType t;

    t = arg_getType(msg);
    if (ARG_TYPE_BYTES != t) {
        obj_setErrorCode(self, -2);  // io error
    }

    uint8_t* data = arg_getBytes(msg);
    uint8_t data_len = arg_getBytesSize(msg);
    uint8_t mode = obj_getInt(self, "_mode");
    void* context = obj_getStruct(self, "context");
    uint8_t* password = obj_getBytes(self, "_password");
    uint8_t password_len = obj_getBytesSize(self, "_password");
    uint8_t* iv = obj_getBytes(self, "_iv");
    uint8_t iv_in[16];

    if (obj_getBytes(self, "_buff") != NULL) {
        obj_removeArg(self, "_buff");
    }

    obj_setBytes(self, "_buff", NULL, data_len * sizeof(uint8_t));
    uint8_t* buff = obj_getBytes(self, "_buff");

    if (mode == MODE_CBC) {
        if (iv != NULL) {
            __platform_memcpy(iv_in, iv, 16);
        } else {
            obj_setErrorCode(self, -2);  // io error
        }
    }

    mbedtls_aes_setkey_enc((mbedtls_aes_context*)context, password,
                           password_len * 8);

    switch (mode) {
        case MODE_ECB:
            mbedtls_aes_crypt_ecb((mbedtls_aes_context*)context,
                                  MBEDTLS_AES_ENCRYPT, data, buff);
            break;
        case MODE_CBC:
            mbedtls_aes_crypt_cbc((mbedtls_aes_context*)context,
                                  MBEDTLS_AES_ENCRYPT, data_len, iv_in, data,
                                  buff);
            break;
        default:
            __platform_printf("[%s]not support mode\r\n", __FUNCTION__);
            obj_setErrorCode(self, -3);  // not support now
            break;
    }
    return arg_newBytes(buff, data_len);
}

Arg* _aes_AES_decrypt(PikaObj* self, Arg* msg) {
    ArgType t;

    t = arg_getType(msg);
    if (ARG_TYPE_BYTES != t) {
        obj_setErrorCode(self, -2);  // io error
    }

    uint8_t* data = arg_getBytes(msg);
    uint8_t data_len = arg_getBytesSize(msg);
    void* context = obj_getStruct(self, "context");
    uint8_t mode = obj_getInt(self, "_mode");
    uint8_t* password = obj_getBytes(self, "_password");
    uint8_t password_len = obj_getBytesSize(self, "_password");
    uint8_t* iv = obj_getBytes(self, "_iv");
    uint8_t iv_in[16];

    if (obj_getBytes(self, "_buff") != NULL) {
        obj_removeArg(self, "_buff");
    }

    obj_setBytes(self, "_buff", NULL, data_len * sizeof(uint8_t));
    uint8_t* buff = obj_getBytes(self, "_buff");

    if (mode == MODE_CBC) {
        if (iv != NULL) {
            __platform_memcpy(iv_in, iv, 16);
        } else {
            obj_setErrorCode(self, -2);  // io error
        }
    }

    mbedtls_aes_setkey_dec((mbedtls_aes_context*)context, password,
                           password_len * 8);

    switch (mode) {
        case MODE_ECB:
            mbedtls_aes_crypt_ecb((mbedtls_aes_context*)context,
                                  MBEDTLS_AES_DECRYPT, data, buff);
            break;
        case MODE_CBC:
            mbedtls_aes_crypt_cbc((mbedtls_aes_context*)context,
                                  MBEDTLS_AES_DECRYPT, data_len, iv_in, data,
                                  buff);
            break;
        default:
            __platform_printf("[%s]not support mode\r\n", __FUNCTION__);
            obj_setErrorCode(self, -3);  // not support now
            break;
    }
    return arg_newBytes(buff, data_len);
}