#include "ctypes_Test.h"
#include "ctypes_c_uint.h"
#include "ctypes_c_wchar_p.h"

void ctypes_c_uint___init__(PikaObj* self, int value) {
    obj_setInt(self, "value", value);
}

void ctypes_c_wchar_p___init__(PikaObj* self, char* value) {
    obj_setStr(self, "value", value);
}

int ctypes_Test_add(PikaObj* self, PikaObj* c_uint1, PikaObj* c_uint2) {
    return obj_getInt(c_uint1, "value") + obj_getInt(c_uint2, "value");
}
