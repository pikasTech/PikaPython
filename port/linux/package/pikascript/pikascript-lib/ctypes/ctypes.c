#include "ctypes_Test.h"
#include "ctypes_cUint.h"
#include "ctypes_cWcharP.h"
void ctypes_cUint___init__(PikaObj* self, int value) {
    obj_setInt(self, "value", value);
}

void ctypes_cWcharP___init__(PikaObj* self, char* value) {
    obj_setStr(self, "value", value);
}

int ctypes_Test_add(PikaObj* self, void* c_uint1, void* c_uint2) {
    PikaObj* c_uint1_obj = c_uint1;
    PikaObj* c_uint2_obj = c_uint2;
    return obj_getInt(c_uint1_obj, "value") + obj_getInt(c_uint2_obj, "value");
}
