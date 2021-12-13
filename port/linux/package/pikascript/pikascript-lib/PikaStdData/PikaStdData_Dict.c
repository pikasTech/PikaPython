#include "PikaObj.h"
#include "PikaStdLib_SysObj.h"
#include "PikaStdData_Dict.h"

Arg* PikaStdData_Dict_get(PikaObj* self, char* key) {
    return arg_copy(obj_getArg(self, key));
}

void PikaStdData_Dict___init__(PikaObj* self) {}

void PikaStdData_Dict_set(PikaObj* self, Arg* arg, char* key) {
    obj_setArg(self, key, arg);
}

void PikaStdData_Dict_remove(PikaObj* self, char* key) {
    PikaStdLib_SysObj_remove(self, key);
}