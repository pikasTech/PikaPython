#include "PikaObj.h"

Arg* PikaStdLib_RangeObj___iter__(PikaObj* self) {
    int a1 = obj_getInt(self, "__range_a1");
    int a2 = obj_getInt(self, "__range_a2");
    int a3 = obj_getInt(self, "__range_a3");
    return arg_setNull(NULL);
}
Arg* PikaStdLib_RangeObj___next__(PikaObj* self) {
    return NULL;
}