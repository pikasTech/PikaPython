#include "PikaObj.h"
#include "PikaStdLib_SysObj.h"
#include "PikaStdData_Dict.h"
#include "BaseObj.h"

Arg* PikaStdData_Dict_get(PikaObj* self, char* key) {
    PikaObj* pyload = obj_getObj(self, "pyload", 0);
    return arg_copy(obj_getArg(pyload, key));
}

void PikaStdData_Dict___init__(PikaObj* self) {
    obj_newObj(self, "pyload", "", New_TinyObj);
}

void PikaStdData_Dict_set(PikaObj* self, Arg* arg, char* key) {
    PikaObj* pyload = obj_getObj(self, "pyload", 0);
    obj_setArg(pyload, key, arg);
}

void PikaStdData_Dict_remove(PikaObj* self, char* key) {
    PikaObj* pyload = obj_getObj(self, "pyload", 0);
    PikaStdLib_SysObj_remove(pyload, key);
}

Arg * PikaStdData_Dict___iter__(PikaObj *self){
    obj_setInt(self, "__iter_i", 0);
    return arg_setPtr(NULL, "", TYPE_POINTER, self);
}
Arg * PikaStdData_Dict___next__(PikaObj *self){
    return NULL;
}