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

static void hashlib_to_hex(uint8_t* s, int l, uint8_t* d);

void _hashlib_Hash_new(PikaObj* self, char* mode, Arg* data) {
    ArgType t;
    t = arg_getType(data);
    if (ARG_TYPE_NONE != t) {
        if (ARG_TYPE_BYTES != t) {
            obj_setErrorCode(self, -2);  // io error
        }
    }

    obj_setInt(self, "_digest_flags", 0);                    // flag
    obj_setBytes(self, "_buff", NULL, PIKA_HASHLIB_SHA512);  // digest buff
    obj_setBytes(self, "_hexbuff", NULL,
                 PIKA_HASHLIB_SHA512 * 2);  // hexdigest buff

    size_t data_len = arg_getBytesSize(data);
    uint8_t* data_h = arg_getBytes(data);

    if (strcmp(mode, "md5") == 0 || strcmp(mode, "MD5") == 0) {
        mbedtls_md5_context context;
        mbedtls_md5_init(&context);
        mbedtls_md5_starts(&context);
        if (data_len > 0) {
            mbedtls_md5_update(&context, data_h, data_len);
        }
        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HASHLIB_MD5);
    } else if (strcmp(mode, "sha1") == 0 || strcmp(mode, "SHA1") == 0) {
        mbedtls_sha1_context context;
        mbedtls_sha1_init(&context);
        mbedtls_sha1_starts(&context);
        if (data_len > 0) {
            mbedtls_sha1_update(&context, data_h,
                                data_len);
        }
        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HASHLIB_SHA1);
    } else if (strcmp(mode, "sha224") == 0 || strcmp(mode, "SHA224") == 0) {
        mbedtls_sha256_context context;
        mbedtls_sha256_init(&context);
        mbedtls_sha256_starts(&context, 1);
        if (data_len > 0) {
            mbedtls_sha256_update(&context, data_h,
                                  data_len);
        }
        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HASHLIB_SHA224);
    } else if (strcmp(mode, "sha256") == 0 || strcmp(mode, "SHA256") == 0) {
        mbedtls_sha256_context context;
        mbedtls_sha256_init(&context);
        mbedtls_sha256_starts(&context, 0);
        if (data_len > 0) {
            mbedtls_sha256_update(&context, data_h,
                                  data_len);
        }
        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HASHLIB_SHA256);
    } else if (strcmp(mode, "sha384") == 0 || strcmp(mode, "SHA384") == 0) {
        mbedtls_sha512_context context;
        mbedtls_sha512_init(&context);
        mbedtls_sha512_starts(&context, 1);
        if (data_len > 0) {
            mbedtls_sha512_update(&context, data_h,
                                  data_len);
        }
        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HASHLIB_SHA384);
    } else if (strcmp(mode, "sha512") == 0 || strcmp(mode, "SHA512") == 0) {
        mbedtls_sha512_context context;
        mbedtls_sha512_init(&context);
        mbedtls_sha512_starts(&context, 0);
        if (data_len > 0) {
            mbedtls_sha512_update(&context, data_h,
                                  data_len);
        }
        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HASHLIB_SHA512);
    } else {
        obj_setErrorCode(self, -1);  // not support mode
    }
}

void _hashlib_Hash_update(PikaObj* self, Arg* data) {
    if (ARG_TYPE_BYTES != arg_getType(data)) {
        obj_setErrorCode(self, -2);  // io error
    }

    void* context = obj_getStruct(self, "context");
    size_t data_len = arg_getBytesSize(data);
    uint8_t* data_h = arg_getBytes(data);

    if (data_len > 0) {
        switch (obj_getInt(self, "mode")) {
            case PIKA_HASHLIB_MD5:
                mbedtls_md5_update((mbedtls_md5_context*)context, data_h,
                                   data_len);
                break;
            case PIKA_HASHLIB_SHA1:
                mbedtls_sha1_update((mbedtls_sha1_context*)context, data_h,
                                    data_len);
                break;
            case PIKA_HASHLIB_SHA224:
            case PIKA_HASHLIB_SHA256:
                mbedtls_sha256_update((mbedtls_sha256_context*)context, data_h,
                                      data_len);
                break;
            case PIKA_HASHLIB_SHA384:
            case PIKA_HASHLIB_SHA512:
                mbedtls_sha512_update((mbedtls_sha512_context*)context, data_h,
                                      data_len);
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
    uint8_t* buff = obj_getBytes(self, "_buff");
    uint8_t flag = obj_getInt(self, "_digest_flags");

    if (flag & 0x01)  // already digest
    {
        return arg_newBytes(buff, obj_getInt(self, "mode"));
    } else {
        void* context = obj_getStruct(self, "context");
        uint8_t mode = obj_getInt(self, "mode");

        switch (mode) {
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
                return arg_newNone();  // will not actually return to the python
                break;
        }
        obj_setInt(self, "_digest_flags", flag | 0x01);
        return arg_newBytes(buff, mode);
    }
}

char* _hashlib_Hash_hexdigest(PikaObj* self) {
    uint8_t* hexbuff = obj_getBytes(self, "_hexbuff");
    uint8_t* buff = obj_getBytes(self, "_buff");
    uint8_t flag = obj_getInt(self, "_digest_flags");

    if (flag & 0x01) {  // already digest
        hashlib_to_hex(buff, obj_getInt(self, "mode"), hexbuff);
        obj_setInt(self, "_digest_flags", flag | 0x02);  // set hexdigest flag
    } else if (flag & 0x02) {                           // already hexdigest

    } else {
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
        hashlib_to_hex(buff, obj_getInt(self, "mode"), hexbuff);
        obj_setInt(self, "_digest_flags",
                   flag | 0x03);  // set digest and hexdigest flags
    }
    return (char*)hexbuff;
}

void _hashlib_Hash_copy(PikaObj* self) {  // todo
}

static const uint8_t hashlib_hex_table[] = {'0', '1', '2', '3', '4', '5',
                                            '6', '7', '8', '9', 'A', 'B',
                                            'C', 'D', 'E', 'F'};
static void hashlib_to_hex(uint8_t* s, int l, uint8_t* d) {
    while (l--) {
        *(d++) = hashlib_hex_table[*s >> 4];
        *(d++) = hashlib_hex_table[*(s++) & 0x0f];
    }
}
