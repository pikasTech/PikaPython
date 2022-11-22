#include "_hmac_HMAC.h"

#include "string.h"

#include "mbedtls/md5.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"

enum {
    PIKA_HMAC_MD5 = 16,
    PIKA_HMAC_SHA1 = 20,
    PIKA_HMAC_SHA256 = 32
} pika_hmac_t;

static void hmac_to_hex(uint8_t* s, int l, uint8_t* d);

void _hmac_HMAC_new(PikaObj* self, Arg* key, Arg* msg, char* digestmod) {
    ArgType t;
    t = arg_getType(key);
    if (ARG_TYPE_NONE != t) {
        if (ARG_TYPE_BYTES != t) {
            obj_setErrorCode(self, -2);  // io error
        }
    }
    t = arg_getType(msg);
    if (ARG_TYPE_NONE != t) {
        if (ARG_TYPE_BYTES != t) {
            obj_setErrorCode(self, -2);  // io error
        }
    }

    obj_setInt(self, "_digest_flags", 0);                 // flag
    obj_setBytes(self, "_buff", NULL, PIKA_HMAC_SHA256);  // digest buff
    obj_setBytes(self, "_hexbuff", NULL,
                 PIKA_HMAC_SHA256 * 2);      // hexdigest buff
    obj_setBytes(self, "_k_ipad", NULL, 64);  // key ipad
    obj_setBytes(self, "_k_opad", NULL, 64);  // key opad

    size_t key_len = arg_getBytesSize(key);
    uint8_t* key_data = arg_getBytes(key);
    size_t msg_len = arg_getBytesSize(msg);
    uint8_t* msg_data = arg_getBytes(msg);

    uint8_t* k_ipad = obj_getBytes(self, "_k_ipad");
    uint8_t* k_opad = obj_getBytes(self, "_k_opad");

    if (strcmp(digestmod, "hmac-md5") == 0 ||
        strcmp(digestmod, "HMAC-MD5") == 0) {
        mbedtls_md5_context context;

        if (key_len > 64) {
            mbedtls_md5_init(&context);
            mbedtls_md5_starts(&context);
            mbedtls_md5_update(&context, key_data, key_len);
            mbedtls_md5_finish(&context, k_ipad);
            mbedtls_md5_free(&context);
            memcpy(k_opad, k_ipad, 64);
        } else {
            memcpy(k_ipad, key_data, key_len);
            memcpy(k_opad, key_data, key_len);
        }

        for (size_t i = 0; i < 64; i++) {
            k_ipad[i] ^= 0x36;
            k_opad[i] ^= 0x5c;
        }

        mbedtls_md5_init(&context);
        mbedtls_md5_starts(&context);
        mbedtls_md5_update(&context, k_ipad, 64);
        if (msg_len > 0) {
            mbedtls_md5_update(&context, msg_data, msg_len);
        }

        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HMAC_MD5);
    } else if (strcmp(digestmod, "hmac-sha1") == 0 ||
               strcmp(digestmod, "HMAC-SHA1") == 0) {
        mbedtls_sha1_context context;

        if (key_len > 64) {
            mbedtls_sha1_init(&context);
            mbedtls_sha1_starts(&context);
            mbedtls_sha1_update(&context, key_data, key_len);
            mbedtls_sha1_finish(&context, k_ipad);
            mbedtls_sha1_free(&context);
            memcpy(k_opad, k_ipad, 64);
        } else {
            memcpy(k_ipad, key_data, key_len);
            memcpy(k_opad, key_data, key_len);
        }

        for (size_t i = 0; i < 64; i++) {
            k_ipad[i] ^= 0x36;
            k_opad[i] ^= 0x5c;
        }

        mbedtls_sha1_init(&context);
        mbedtls_sha1_starts(&context);
        mbedtls_sha1_update(&context, k_ipad, 64);
        if (msg_len > 0) {
            mbedtls_sha1_update(&context, msg_data, msg_len);
        }

        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HMAC_SHA1);
    } else if (strcmp(digestmod, "hmac-sha256") == 0 ||
               strcmp(digestmod, "HMAC-SHA256") == 0) {
        mbedtls_sha256_context context;

        if (key_len > 64) {
            mbedtls_sha256_init(&context);
            mbedtls_sha256_starts(&context, 0);
            mbedtls_sha256_update(&context, key_data, key_len);
            mbedtls_sha256_finish(&context, k_ipad);
            mbedtls_sha256_free(&context);
            memcpy(k_opad, k_ipad, 64);
        } else {
            memcpy(k_ipad, key_data, key_len);
            memcpy(k_opad, key_data, key_len);
        }

        for (size_t i = 0; i < 64; i++) {
            k_ipad[i] ^= 0x36;
            k_opad[i] ^= 0x5c;
        }

        mbedtls_sha256_init(&context);
        mbedtls_sha256_starts(&context, 0);
        mbedtls_sha256_update(&context, k_ipad, 64);
        if (msg_len > 0) {
            mbedtls_sha256_update(&context, msg_data, msg_len);
        }

        obj_setStruct(self, "context", context);
        obj_setInt(self, "mode", PIKA_HMAC_SHA256);
    } else {
        obj_setErrorCode(self, -1);  // not support mode
    }
}

void _hmac_HMAC_update(PikaObj* self, Arg* msg) {
    ArgType t = arg_getType(msg);
    if (ARG_TYPE_BYTES != t) {
        obj_setErrorCode(self, -2);  // io error
    }

    size_t msg_len = arg_getBytesSize(msg);
    uint8_t* msg_data = arg_getBytes(msg);
    void* context = obj_getStruct(self, "context");

    if (msg_len > 0) {
        switch (obj_getInt(self, "mode")) {
            case PIKA_HMAC_MD5:
                mbedtls_md5_update((mbedtls_md5_context*)context, msg_data,
                                   msg_len);
                break;
            case PIKA_HMAC_SHA1:
                mbedtls_sha1_update((mbedtls_sha1_context*)context, msg_data,
                                    msg_len);
                break;
            case PIKA_HMAC_SHA256:
                mbedtls_sha256_update((mbedtls_sha256_context*)context,
                                      msg_data, msg_len);
                break;
            default:
                break;
        }
    } else {
        obj_setErrorCode(self, -2);  // io error
    }
}

Arg* _hmac_HMAC_digest(PikaObj* self) {
    uint8_t* k_opad = obj_getBytes(self, "_k_opad");
    uint8_t* buff = obj_getBytes(self, "_buff");
    uint8_t flag = obj_getInt(self, "_digest_flags");

    if (flag & 0x01)  // already digest
    {
        return arg_newBytes(buff, obj_getInt(self, "mode"));
    } else {
        void* context = obj_getStruct(self, "context");
        uint8_t mode = obj_getInt(self, "mode");
        switch (mode) {
            case PIKA_HMAC_MD5:
                mbedtls_md5_finish((mbedtls_md5_context*)context, buff);
                mbedtls_md5_free((mbedtls_md5_context*)context);

                mbedtls_md5_init((mbedtls_md5_context*)context);
                mbedtls_md5_starts((mbedtls_md5_context*)context);
                mbedtls_md5_update((mbedtls_md5_context*)context, k_opad, 64);
                mbedtls_md5_update((mbedtls_md5_context*)context, buff,
                                   PIKA_HMAC_MD5);
                mbedtls_md5_finish((mbedtls_md5_context*)context, buff);
                mbedtls_md5_free((mbedtls_md5_context*)context);
                break;
            case PIKA_HMAC_SHA1:
                mbedtls_sha1_finish((mbedtls_sha1_context*)context, buff);
                mbedtls_sha1_free((mbedtls_sha1_context*)context);

                mbedtls_sha1_init((mbedtls_sha1_context*)context);
                mbedtls_sha1_starts((mbedtls_sha1_context*)context);
                mbedtls_sha1_update((mbedtls_sha1_context*)context, k_opad, 64);
                mbedtls_sha1_update((mbedtls_sha1_context*)context, buff,
                                    PIKA_HMAC_SHA1);
                mbedtls_sha1_finish((mbedtls_sha1_context*)context, buff);
                mbedtls_sha1_free((mbedtls_sha1_context*)context);
                break;
            case PIKA_HMAC_SHA256:
                mbedtls_sha256_finish((mbedtls_sha256_context*)context, buff);
                mbedtls_sha256_free((mbedtls_sha256_context*)context);

                mbedtls_sha256_init((mbedtls_sha256_context*)context);
                mbedtls_sha256_starts((mbedtls_sha256_context*)context, 0);
                mbedtls_sha256_update((mbedtls_sha256_context*)context, k_opad,
                                      64);
                mbedtls_sha256_update((mbedtls_sha256_context*)context, buff,
                                      PIKA_HMAC_SHA256);
                mbedtls_sha256_finish((mbedtls_sha256_context*)context, buff);
                mbedtls_sha256_free((mbedtls_sha256_context*)context);
                break;
            default:
                obj_setErrorCode(self, -1);  // not support mode
                return arg_newNull();  // will not actually return to the python
                break;
        }
        obj_setInt(self, "_digest_flags", flag | 0x01);
        return arg_newBytes(buff, mode);
    }
}

char* _hmac_HMAC_hexdigest(PikaObj* self) {
    uint8_t* buff = obj_getBytes(self, "_buff");
    uint8_t* hexbuff = obj_getBytes(self, "_hexbuff");
    uint8_t flag = obj_getInt(self, "_digest_flags");

    if (flag & 0x01) {  // already digest
        hmac_to_hex(buff, obj_getInt(self, "mode"), hexbuff);
        obj_setInt(self, "_digest_flags", flag | 0x02);  // set hexdigest flag
    } else if (flag & 0x02) {                           // already hexdigest

    } else {
        void* context = obj_getStruct(self, "context");
        uint8_t* k_opad = obj_getBytes(self, "_k_opad");

        switch (obj_getInt(self, "mode")) {
            case PIKA_HMAC_MD5:
                mbedtls_md5_finish((mbedtls_md5_context*)context, buff);
                mbedtls_md5_free((mbedtls_md5_context*)context);

                mbedtls_md5_init((mbedtls_md5_context*)context);
                mbedtls_md5_starts((mbedtls_md5_context*)context);
                mbedtls_md5_update((mbedtls_md5_context*)context, k_opad, 64);
                mbedtls_md5_update((mbedtls_md5_context*)context, buff,
                                   PIKA_HMAC_MD5);
                mbedtls_md5_finish((mbedtls_md5_context*)context, buff);
                mbedtls_md5_free((mbedtls_md5_context*)context);
                break;
            case PIKA_HMAC_SHA1:
                mbedtls_sha1_finish((mbedtls_sha1_context*)context, buff);
                mbedtls_sha1_free((mbedtls_sha1_context*)context);

                mbedtls_sha1_init((mbedtls_sha1_context*)context);
                mbedtls_sha1_starts((mbedtls_sha1_context*)context);
                mbedtls_sha1_update((mbedtls_sha1_context*)context, k_opad, 64);
                mbedtls_sha1_update((mbedtls_sha1_context*)context, buff,
                                    PIKA_HMAC_SHA1);
                mbedtls_sha1_finish((mbedtls_sha1_context*)context, buff);
                mbedtls_sha1_free((mbedtls_sha1_context*)context);
                break;
            case PIKA_HMAC_SHA256:
                mbedtls_sha256_finish((mbedtls_sha256_context*)context, buff);
                mbedtls_sha256_free((mbedtls_sha256_context*)context);

                mbedtls_sha256_init((mbedtls_sha256_context*)context);
                mbedtls_sha256_starts((mbedtls_sha256_context*)context, 0);
                mbedtls_sha256_update((mbedtls_sha256_context*)context, k_opad,
                                      64);
                mbedtls_sha256_update((mbedtls_sha256_context*)context, buff,
                                      PIKA_HMAC_SHA256);
                mbedtls_sha256_finish((mbedtls_sha256_context*)context, buff);
                mbedtls_sha256_free((mbedtls_sha256_context*)context);
                break;
            default:
                obj_setErrorCode(self, -1);  // not support mode
                break;
        }
        hmac_to_hex(buff, PIKA_HMAC_MD5, hexbuff);
        obj_setInt(self, "_digest_flags",
                   flag | 0x03);  // set digest and hexdigest flags
    }
    return (char*)hexbuff;
}

void _hmac_HMAC_copy(PikaObj* self) {}

static const uint8_t hmac_hex_table[] = {'0', '1', '2', '3', '4', '5',
                                         '6', '7', '8', '9', 'A', 'B',
                                         'C', 'D', 'E', 'F'};
static void hmac_to_hex(uint8_t* s, int l, uint8_t* d) {
    while (l--) {
        *(d++) = hmac_hex_table[*s >> 4];
        *(d++) = hmac_hex_table[*(s++) & 0x0f];
    }
}
