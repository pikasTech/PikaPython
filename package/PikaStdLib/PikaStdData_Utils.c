#include "PikaStdData_Utils.h"
#include "dataStrs.h"

Arg* PikaStdData_Utils_int_to_bytes(PikaObj* self, int val) {
    if (val > 0xFF) {
        obj_setErrorCode(self, 1);
        __platform_printf(
            "OverflowError: cannot convert value larger than 0xFF to "
            "bytes\r\n");
        return arg_setNull(NULL);
    }
    uint8_t val_bytes = (uint8_t)val;
    return arg_setBytes(NULL, "", &val_bytes, 1);
}

Arg* PikaStdData_Utils_b2a_hex(PikaObj* self, Arg* val) {
    /* assert val is bytes */
    if (arg_getType(val) != ARG_TYPE_BYTES) {
        obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
        __platform_printf("TypeError: cannot convert value to bytes\r\n");
        return arg_setNull(NULL);
    }

    /* b2a_hex */
    Arg* hex_str = arg_setStr(NULL, "", "");
    for (int i = 0; i < (int)arg_getBytesSize(val); i++) {
        uint8_t* bytes = arg_getBytes(val);
        uint8_t byte = bytes[i];
        char hex_byte[3] = {0};
        __platform_sprintf(hex_byte, "%02X", byte);
        hex_str = arg_strAppend(hex_str, hex_byte);
    }
    Arg* hex_bytes = arg_setBytes(NULL, "", (uint8_t*)arg_getStr(hex_str),
                                  strGetSize(arg_getStr(hex_str)));
    arg_deinit(hex_str);
    return hex_bytes;
}
