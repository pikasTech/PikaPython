#include "PikaObj.h"

Arg* PikaStdLib_RangeObj___iter__(PikaObj* self) {
    return arg_setPtr(NULL, "", TYPE_OBJECT, self);;
}
Arg* PikaStdLib_RangeObj___next__(PikaObj* self) {
    return NULL;
}