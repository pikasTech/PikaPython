#include "BaseObj.h"
#include "PikaObj.h"
#include "PikaStdLib_SysObj.h"

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
    obj_removeArg(pyload, key);
}

Arg* PikaStdData_Dict___iter__(PikaObj* self) {
    obj_setInt(self, "__iter_i", 0);
    return arg_setPtr(NULL, "", ARG_TYPE_POINTER, self);
}

Arg* PikaStdData_Dict___next__(PikaObj* self) {
    int __iter_i = args_getInt(self->list, "__iter_i");
    PikaObj* pyload = obj_getObj(self, "pyload", 0);
    Arg* res = arg_copy(args_getArg_index(pyload->list, __iter_i));
    if (ARG_TYPE_POINTER == arg_getType(res)) {
        arg_deinit(res);
        return arg_setNull(NULL);
    }
    if (NULL == res) {
        return arg_setNull(NULL);
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return res;
}

void PikaStdData_Dict___set__(PikaObj* self) {
    PikaStdData_Dict_set(self, obj_getArg(self, "__val"),
                         obj_getStr(self, "__key"));
}
Arg* PikaStdData_Dict___get__(PikaObj* self) {
    return PikaStdData_Dict_get(self, obj_getStr(self, "__key"));
}
