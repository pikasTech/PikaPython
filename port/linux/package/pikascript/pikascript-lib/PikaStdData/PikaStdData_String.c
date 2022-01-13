#include "PikaObj.h"

Arg* PikaStdData_String___iter__(PikaObj* self) {
    obj_setInt(self, "__iter_i", 0);
    return arg_setPtr(NULL, "", TYPE_POINTER, self);
}

void PikaStdData_String_set(PikaObj* self, char* s) {
    obj_setStr(self, "str", s);
}

char* PikaStdData_String_get(PikaObj* self) {
    return obj_getStr(self, "str");
}

Arg* PikaStdData_String___next__(PikaObj* self) {
    int __iter_i = args_getInt(self->list, "__iter_i");
    char* str = obj_getStr(self, "str");
    uint16_t len = strGetSize(str);
    Arg* res = NULL;
    char char_buff[] = " ";
    if (__iter_i < len) {
        char_buff[0] = str[__iter_i];
        res = arg_setStr(NULL, "", (char*)char_buff);
    } else {
        return arg_setNull(NULL);
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return res;
}
