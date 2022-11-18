#include "_hashlib_Hash.h"

#include "string.h"

#include "mbedtls/md5.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"

enum {
    PIKA_HASHLIB_MD5 = 16,
    PIKA_HASHLIB_SHA1 = 20,
    PIKA_HASHLIB_SHA224 = 28,
    PIKA_HASHLIB_SHA256 = 32,
    PIKA_HASHLIB_SHA384 = 48,
    PIKA_HASHLIB_SHA512 = 64
} pika_hashlib_t;

void to_hex(uint8_t* s, int l, uint8_t* d);

void _hashlib_Hash_md5(PikaObj* self, Arg* data) {
    size_t size = arg_getBytesSize(data);
    uint8_t* data_h = arg_getBytes(data);

    mbedtls_md5_context context;
    mbedtls_md5_init(&context);
    mbedtls_md5_starts(&context);
    if (size != 0) {
        mbedtls_md5_update(&context, data_h, size);
    }
    obj_setStruct(self, "context", context);
    obj_setInt(self, "mode", PIKA_HASHLIB_MD5);
    return;
}

void _hashlib_Hash_sha1(PikaObj* self, Arg* data) {
    size_t size = arg_getBytesSize(data);
    uint8_t* data_h = arg_getBytes(data);

    mbedtls_sha1_context context;
    mbedtls_sha1_init(&context);
    mbedtls_sha1_starts(&context);
    if (size != 0) {
        mbedtls_sha1_update(&context, data_h, size);
    }
    obj_setStruct(self, "context", context);
    obj_setInt(self, "mode", PIKA_HASHLIB_SHA1);
    return;
}

void _hashlib_Hash_sha224(PikaObj* self, Arg* data) {
    size_t size = arg_getBytesSize(data);
    uint8_t* data_h = arg_getBytes(data);

    mbedtls_sha256_context context;
    mbedtls_sha256_init(&context);
    mbedtls_sha256_starts(&context, 1);
    if (size != 0) {
        mbedtls_sha256_update(&context, data_h, size);
    }
    obj_setStruct(self, "context", context);
    obj_setInt(self, "mode", PIKA_HASHLIB_SHA224);
    return;
}

void _hashlib_Hash_sha256(PikaObj* self, Arg* data) {
    size_t size = arg_getBytesSize(data);
    uint8_t* data_h = arg_getBytes(data);

    mbedtls_sha256_context context;
    mbedtls_sha256_init(&context);
    mbedtls_sha256_starts(&context, 0);
    if (size != 0) {
        mbedtls_sha256_update(&context, data_h, size);
    }
    obj_setStruct(self, "context", context);
    obj_setInt(self, "mode", PIKA_HASHLIB_SHA256);
    return;
}

void _hashlib_Hash_sha384(PikaObj* self, Arg* data) {
    size_t size = arg_getBytesSize(data);
    uint8_t* data_h = arg_getBytes(data);

    mbedtls_sha512_context context;
    mbedtls_sha512_init(&context);
    mbedtls_sha512_starts(&context, 1);
    if (size != 0) {
        mbedtls_sha512_update(&context, data_h, size);
    }
    obj_setStruct(self, "context", context);
    obj_setInt(self, "mode", PIKA_HASHLIB_SHA384);
    return;
}

void _hashlib_Hash_sha512(PikaObj* self, Arg* data) {
    size_t size = arg_getBytesSize(data);
    uint8_t* data_h = arg_getBytes(data);

    mbedtls_sha512_context context;
    mbedtls_sha512_init(&context);
    mbedtls_sha512_starts(&context, 0);
    if (size != 0) {
        mbedtls_sha512_update(&context, data_h, size);
    }
    obj_setStruct(self, "context", context);
    obj_setInt(self, "mode", PIKA_HASHLIB_SHA512);
    return;
}

void _hashlib_Hash_new(PikaObj* self, char* mode) {
    if (strcmp(mode, "md5") == 0 || strcmp(mode, "MD5") == 0) {
        mbedtls_md5_context context;
        mbedtls_md5_init(&context);
        mbedtls_md5_starts(&context);
        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HASHLIB_MD5);
    } else if (strcmp(mode, "sha1") == 0 || strcmp(mode, "SHA1") == 0) {
        mbedtls_sha1_context context;
        mbedtls_sha1_init(&context);
        mbedtls_sha1_starts(&context);
        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HASHLIB_SHA1);
    } else if (strcmp(mode, "sha224") == 0 || strcmp(mode, "SHA224") == 0) {
        mbedtls_sha256_context context;
        mbedtls_sha256_init(&context);
        mbedtls_sha256_starts(&context, 1);
        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HASHLIB_SHA224);
    } else if (strcmp(mode, "sha256") == 0 || strcmp(mode, "SHA256") == 0) {
        mbedtls_sha256_context context;
        mbedtls_sha256_init(&context);
        mbedtls_sha256_starts(&context, 0);
        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HASHLIB_SHA256);
    } else if (strcmp(mode, "sha384") == 0 || strcmp(mode, "SHA384") == 0) {
        mbedtls_sha512_context context;
        mbedtls_sha512_init(&context);
        mbedtls_sha512_starts(&context, 1);
        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HASHLIB_SHA384);
    } else if (strcmp(mode, "sha512") == 0 || strcmp(mode, "SHA512") == 0) {
        mbedtls_sha512_context context;
        mbedtls_sha512_init(&context);
        mbedtls_sha512_starts(&context, 0);
        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HASHLIB_SHA512);
    } else {
        obj_setErrorCode(self, -1);  // not support mode
    }
    return;
}

void _hashlib_Hash_update(PikaObj* self, Arg* data) {
    size_t size = arg_getBytesSize(data);
    uint8_t* data_h = arg_getBytes(data);

    if (size != 0) {
        void* context = obj_getStruct(self, "context");
        switch (obj_getInt(self, "mode")) {
            case PIKA_HASHLIB_MD5:
                mbedtls_md5_update((mbedtls_md5_context*)context, data_h, size);
                break;
            case PIKA_HASHLIB_SHA1:
                mbedtls_sha1_update((mbedtls_sha1_context*)context, data_h,
                                    size);
                break;
            case PIKA_HASHLIB_SHA224:
            case PIKA_HASHLIB_SHA256:
                mbedtls_sha256_update((mbedtls_sha256_context*)context, data_h,
                                      size);
                break;
            case PIKA_HASHLIB_SHA384:
            case PIKA_HASHLIB_SHA512:
                mbedtls_sha512_update((mbedtls_sha512_context*)context, data_h,
                                      size);
                break;
            default:
                obj_setErrorCode(self, -1);  // not support mode
                break;
        }
    } else {
        obj_setErrorCode(self, -2);  // io error
    }
}

Arg* _hashlib_Hash_digest(PikaObj* self) {
    obj_setBytes(self, "buff", NULL, 64);
    uint8_t* buff = obj_getBytes(self, "buff");
    void* context = obj_getStruct(self, "context");

    switch (obj_getInt(self, "mode")) {
        case PIKA_HASHLIB_MD5:
            mbedtls_md5_finish((mbedtls_md5_context*)context, buff);
            return arg_newBytes(buff, PIKA_HASHLIB_MD5);
            break;
        case PIKA_HASHLIB_SHA1:
            mbedtls_sha1_finish((mbedtls_sha1_context*)context, buff);
            return arg_newBytes(buff, PIKA_HASHLIB_SHA1);
            break;
        case PIKA_HASHLIB_SHA224:
            mbedtls_sha256_finish((mbedtls_sha256_context*)context, buff);
            return arg_newBytes(buff, PIKA_HASHLIB_SHA224);
            break;
        case PIKA_HASHLIB_SHA256:
            mbedtls_sha256_finish((mbedtls_sha256_context*)context, buff);
            return arg_newBytes(buff, PIKA_HASHLIB_SHA256);
            break;
        case PIKA_HASHLIB_SHA384:
            mbedtls_sha512_finish((mbedtls_sha512_context*)context, buff);
            return arg_newBytes(buff, PIKA_HASHLIB_SHA384);
            break;
        case PIKA_HASHLIB_SHA512:
            mbedtls_sha512_finish((mbedtls_sha512_context*)context, buff);
            return arg_newBytes(buff, PIKA_HASHLIB_SHA512);
            break;
        default:
            obj_setErrorCode(self, -1);  // not support mode
            return arg_newNull();
            break;
    }
}

char* _hashlib_Hash_hexdigest(PikaObj* self) {
    obj_setBytes(self, "hexbuff", NULL, 128);
    uint8_t* hexbuff = obj_getBytes(self, "hexbuff");
    uint8_t* buff = obj_getBytes(self, "buff");

    if (buff == NULL) {
        obj_setBytes(self, "buff", NULL, 64);
        uint8_t* buff = obj_getBytes(self, "buff");
        void* context = obj_getStruct(self, "context");

        switch (obj_getInt(self, "mode")) {
            case PIKA_HASHLIB_MD5:
                mbedtls_md5_finish((mbedtls_md5_context*)context, buff);
                break;
            case PIKA_HASHLIB_SHA1:
                mbedtls_sha1_finish((mbedtls_sha1_context*)context, buff);
                break;
            case PIKA_HASHLIB_SHA224:
                mbedtls_sha256_finish((mbedtls_sha256_context*)context, buff);
                break;
            case PIKA_HASHLIB_SHA256:
                mbedtls_sha256_finish((mbedtls_sha256_context*)context, buff);
                break;
            case PIKA_HASHLIB_SHA384:
                mbedtls_sha512_finish((mbedtls_sha512_context*)context, buff);
                break;
            case PIKA_HASHLIB_SHA512:
                mbedtls_sha512_finish((mbedtls_sha512_context*)context, buff);
                break;
            default:
                obj_setErrorCode(self, -1);  // not support mode
                break;
        }
        to_hex(buff, obj_getInt(self, "mode"), hexbuff);
    } else {
        to_hex(buff, obj_getInt(self, "mode"), hexbuff);
    }
    return (char*)hexbuff;
}

void _hashlib_Hash_copy(PikaObj* self) {}

const uint8_t hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                             '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
void to_hex(uint8_t* s, int l, uint8_t* d) {
    while (l--) {
        *(d++) = hex_table[*s >> 4];
        *(d++) = hex_table[*(s++) & 0x0f];
    }
}
