#include "binascii.h"

static int hex2int(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

Arg* binascii_a2b_hex(PikaObj* self, char* val) {
    /* a2b_hex */
    int len = strGetSize(val);
    Arg* ret = arg_newBytes(NULL, len / 2);
    uint8_t* res_hex = arg_getBytes(ret);
    for (int i = 0; i < len; i += 2) {
        int hex_val = hex2int(val[i]);
        if (hex_val == -1) {
            obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
            obj_setSysOut(self,
                          "TypeError: non-hexadecimal number found in "
                          "fromhex() arg at position %d",
                          i + 1);
        } else {
            res_hex[i / 2] = (hex_val << 4) | hex2int(val[i + 1]);
        }
    }
    return ret;
}

Arg* binascii_b2a_hex(PikaObj* self, Arg* val) {
    /* assert val is bytes */
    if (arg_getType(val) != ARG_TYPE_BYTES) {
        obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
        obj_setSysOut(self, "TypeError: cannot convert value to bytes");
        return arg_newNone();
    }

    /* b2a_hex */
    Arg* hex_str = arg_newStr("");
    for (int i = 0; i < (int)arg_getBytesSize(val); i++) {
        uint8_t* bytes = arg_getBytes(val);
        uint8_t byte = bytes[i];
        char hex_byte[3] = {0};
        __platform_sprintf(hex_byte, "%02X", byte);
        hex_str = arg_strAppend(hex_str, hex_byte);
    }
    Arg* hex_bytes = arg_newBytes((uint8_t*)arg_getStr(hex_str),
                                  strGetSize(arg_getStr(hex_str)));
    arg_deinit(hex_str);
    return hex_bytes;
}
