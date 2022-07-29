#include "BaseObj.h"
#include "PikaObj.h"
#include "PikaStdLib_SysObj.h"

Arg* PikaStdData_Dict_get(PikaObj* self, char* key) {
    PikaDict* dict = obj_getPtr(self, "dict");
    return arg_copy(dict_getArg(dict, key));
}

void PikaStdData_Dict___init__(PikaObj* self) {
    PikaDict* dict = New_dict();
    obj_setPtr(self, "dict", dict);
}

void PikaStdData_Dict_set(PikaObj* self, Arg* arg, char* key) {
    PikaDict* dict = obj_getPtr(self, "dict");
    arg_setName(arg, key);
    dict_setArg(dict, arg_copy(arg));
}

void PikaStdData_Dict_remove(PikaObj* self, char* key) {
    PikaDict* dict = obj_getPtr(self, "dict");
    dict_removeArg(dict, dict_getArg(dict, key));
}

Arg* PikaStdData_Dict___iter__(PikaObj* self) {
    obj_setInt(self, "__iter_i", 0);
    return arg_setRef(NULL, "", self);
}

Arg* PikaStdData_Dict___next__(PikaObj* self) {
    int __iter_i = args_getInt(self->list, "__iter_i");
    PikaDict* dict = obj_getPtr(self, "dict");
    Arg* res = arg_copy(args_getArgByidex(&dict->super, __iter_i));
    if (NULL == res) {
        return arg_setNull(NULL);
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return res;
}

void PikaStdData_Dict___setitem__(PikaObj* self) {
    PikaStdData_Dict_set(self, obj_getArg(self, "__val"),
                         obj_getStr(self, "__key"));
}

Arg* PikaStdData_Dict___getitem__(PikaObj* self) {
    return PikaStdData_Dict_get(self, obj_getStr(self, "__key"));
}

void PikaStdData_Dict___del__(PikaObj* self) {
    PikaDict* dict = obj_getPtr(self, "dict");
    dict_deinit(dict);
}
