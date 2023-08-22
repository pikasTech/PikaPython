#include "_hmac_HMAC.h"
#include "mbedtls/md.h"
#include "string.h"

enum {
    PIKA_HMAC_MD5 = 16,
    PIKA_HMAC_SHA1 = 20,
    PIKA_HMAC_SHA256 = 32
} pika_hmac_t;

static void hmac_to_hex(uint8_t* s, int l, uint8_t* d);

static void init_buff(PikaObj* self, size_t h) {
    obj_setBytes(self, "_buff", NULL, h);
    obj_setBytes(self, "_hexbuff", NULL, (h * 2));
    memset(obj_getBytes(self, "_buff"), 0, h);
    memset(obj_getBytes(self, "_hexbuff"), 0, (h * 2));
}

void _hmac_HMAC_new(PikaObj* self, Arg* key, Arg* msg, char* digestmod) {
    ArgType t;
    t = arg_getType(key);
    if (ARG_TYPE_BYTES != t) {
        obj_setErrorCode(self, -2);  // io error
        obj_setSysOut(self, "hmac.new() key type error");
    }

    t = arg_getType(msg);
    if (ARG_TYPE_NONE != t) {
        if (ARG_TYPE_BYTES != t) {
            obj_setErrorCode(self, -2);  // io error
            obj_setSysOut(self, "hmac.new() msg type error");
        }
    }

    size_t key_len = arg_getBytesSize(key);
    uint8_t* key_data = arg_getBytes(key);
    size_t msg_len = arg_getBytesSize(msg);
    uint8_t* msg_data = arg_getBytes(msg);
    obj_setInt(self, "_digest_flags", 0);  // flag
    mbedtls_md_context_t ctx;
    mbedtls_md_init(&ctx);

    if (strcmp(digestmod, "md5") == 0 || strcmp(digestmod, "MD5") == 0) {
        mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_MD5), 1);
        obj_setInt(self, "_mode", PIKA_HMAC_MD5);
        init_buff(self, PIKA_HMAC_MD5);
    } else if (strcmp(digestmod, "sha1") == 0 ||
               strcmp(digestmod, "SHA1") == 0) {
        mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), 1);
        obj_setInt(self, "_mode", PIKA_HMAC_SHA1);
        init_buff(self, PIKA_HMAC_SHA1);
    } else if (strcmp(digestmod, "sha256") == 0 ||
               strcmp(digestmod, "SHA256") == 0) {
        mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
        obj_setInt(self, "_mode", PIKA_HMAC_SHA256);
        init_buff(self, PIKA_HMAC_SHA256);
    } else {
        obj_setErrorCode(self, -2);  // io error
        obj_setSysOut(self, "hmac.new() not support mode");
    }
    mbedtls_md_hmac_starts(&ctx, key_data, key_len);
    if (msg_len > 0) {
        mbedtls_md_hmac_update(&ctx, msg_data, msg_len);
    }
    obj_setStruct(self, "_context", ctx);
}

void _hmac_HMAC_update(PikaObj* self, Arg* msg) {
    ArgType t = arg_getType(msg);
    if (ARG_TYPE_BYTES != t) {
        obj_setErrorCode(self, -2);  // io error
        obj_setSysOut(self, "hmac.update() msg type error");
    }

    size_t msg_len = arg_getBytesSize(msg);
    uint8_t* msg_data = arg_getBytes(msg);
    mbedtls_md_context_t* ctx = obj_getStruct(self, "_context");
    mbedtls_md_hmac_update(ctx, msg_data, msg_len);
}

Arg* _hmac_HMAC_digest(PikaObj* self) {
    uint8_t* buff = obj_getBytes(self, "_buff");
    uint8_t flag = obj_getInt(self, "_digest_flags");

    if (flag & 0x01)  // already digest
    {
        goto __exit;
    } else {
        mbedtls_md_context_t* ctx = obj_getStruct(self, "_context");
        mbedtls_md_hmac_finish(ctx, buff);
        obj_setInt(self, "_digest_flags", flag | 0x01);
        goto __exit;
    }
__exit:
    return arg_newBytes(buff, obj_getInt(self, "_mode"));
}

char* _hmac_HMAC_hexdigest(PikaObj* self) {
    uint8_t* buff = obj_getBytes(self, "_buff");
    uint8_t* hexbuff = obj_getBytes(self, "_hexbuff");
    uint8_t flag = obj_getInt(self, "_digest_flags");

    if (flag & 0x01) {                                   // already digest
        obj_setInt(self, "_digest_flags", flag | 0x02);  // set hexdigest flag
        goto __exit;
    } else if (flag & 0x02) {  // already hexdigest
        return (char*)hexbuff;
    } else {
        mbedtls_md_context_t* ctx = obj_getStruct(self, "_context");
        mbedtls_md_hmac_finish(ctx, buff);
        // set digest and hexdigest flags
        obj_setInt(self, "_digest_flags", flag | 0x03);
        goto __exit;
    }
__exit:
    hmac_to_hex(buff, obj_getInt(self, "_mode"), hexbuff);
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
