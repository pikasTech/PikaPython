#include "PikaStdData_ByteArray.h"

void PikaStdData_ByteArray___init__(PikaObj* self, Arg* bytes) {
    obj_setArg(self, "raw", bytes);
}

Arg* PikaStdData_ByteArray___iter__(PikaObj* self) {
    obj_setInt(self, "__iter_i", 0);
    return arg_newRef(self);
}

Arg* PikaStdData_ByteArray___next__(PikaObj* self) {
    int __iter_i = args_getInt(self->list, "__iter_i");
    uint8_t* data = obj_getBytes(self, "raw");
    uint16_t len = obj_getBytesSize(self, "raw");
    Arg* res = NULL;
    char char_buff[] = " ";
    if (__iter_i < len) {
        char_buff[0] = data[__iter_i];
        res = arg_newInt(char_buff[0]);
    } else {
        return arg_newNull();
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return res;
}

int PikaStdData_ByteArray___getitem__(PikaObj* self, int __key) {
    uint8_t* data = obj_getBytes(self, "raw");
    uint16_t len = obj_getBytesSize(self, "raw");
    if (__key < len) {
        return data[__key];
    } else {
        return 0;
    }
}

void PikaStdData_ByteArray___setitem__(PikaObj* self, int __key, int __val) {
    uint8_t* data = obj_getBytes(self, "raw");
    uint16_t len = obj_getBytesSize(self, "raw");
    if (__key < len) {
        data[__key] = __val;
    }
}

char* PikaStdData_ByteArray___str__(PikaObj* self) {
    uint8_t* data = obj_getBytes(self, "raw");
    uint16_t len = obj_getBytesSize(self, "raw");
    Arg* str_arg = arg_newStr("");
    str_arg = arg_strAppend(str_arg, "bytearray(b'");
    for (int i = 0; i < len; i++) {
        char u8_str[] = "\\x00";
        uint8_t u8 = data[i];
        __platform_sprintf(u8_str, "\\x%02x", u8);
        str_arg = arg_strAppend(str_arg, u8_str);
    }
    str_arg = arg_strAppend(str_arg, "')");
    obj_removeArg(self, "_buf");
    obj_setStr(self, "_buf", arg_getStr(str_arg));
    arg_deinit(str_arg);
    return obj_getStr(self, "_buf");
}

char* PikaStdData_ByteArray_decode(PikaObj* self) {
    uint8_t* data = obj_getBytes(self, "raw");
    Arg* str_arg = arg_newStr((char*)data);
    obj_removeArg(self, "_buf");
    obj_setStr(self, "_buf", arg_getStr(str_arg));
    arg_deinit(str_arg);
    return obj_getStr(self, "_buf");
}
