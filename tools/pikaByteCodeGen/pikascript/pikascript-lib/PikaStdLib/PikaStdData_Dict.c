#include "PikaStdData_Dict.h"
#include "BaseObj.h"
#include "PikaObj.h"
#include "PikaStdData_dict_keys.h"
#include "PikaStdLib_SysObj.h"

Arg* PikaStdData_Dict_get(PikaObj* self, char* key) {
    PikaDict* dict = obj_getPtr(self, "dict");
    return arg_copy(dict_getArg(dict, key));
}

void PikaStdData_Dict___init__(PikaObj* self) {
    PikaDict* dict = New_dict();
    PikaDict* keys = New_dict();
    obj_setPtr(self, "dict", dict);
    obj_setPtr(self, "_keys", keys);
}

void PikaStdData_Dict_set(PikaObj* self, Arg* arg, char* key) {
    PikaDict* dict = obj_getPtr(self, "dict");
    PikaDict* keys = obj_getPtr(self, "_keys");
    Arg* arg_key = arg_setStr(NULL, key, key);
    Arg* arg_new = arg_copy(arg);
    arg_setName(arg_new, key);
    dict_setArg(dict, arg_new);
    dict_setArg(keys, arg_key);
}

void PikaStdData_Dict_remove(PikaObj* self, char* key) {
    PikaDict* dict = obj_getPtr(self, "dict");
    PikaDict* keys = obj_getPtr(self, "_keys");
    dict_removeArg(dict, dict_getArg(dict, key));
    dict_removeArg(keys, dict_getArg(keys, key));
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

void PikaStdData_Dict___set__(PikaObj* self, Arg* __key, Arg* __val) {
    PikaStdData_Dict_set(self, obj_getArg(self, "__val"),
                         obj_getStr(self, "__key"));
}

Arg* PikaStdData_Dict___get__(PikaObj* self, Arg* __key) {
    return PikaStdData_Dict_get(self, obj_getStr(self, "__key"));
}

void PikaStdData_Dict___del__(PikaObj* self) {
    PikaDict* dict = obj_getPtr(self, "dict");
    PikaDict* keys = obj_getPtr(self, "_keys");
    dict_deinit(dict);
    dict_deinit(keys);
}

void PikaStdData_dict_keys___init__(PikaObj* self, PikaObj* dict) {
    obj_setPtr(self, "dictptr", dict);
}

PikaObj* PikaStdData_Dict_keys(PikaObj* self) {
    PikaObj* dict_keys = newNormalObj(New_PikaStdData_dict_keys);
    PikaStdData_dict_keys___init__(dict_keys, self);
    return dict_keys;
}

Arg* PikaStdData_dict_keys___iter__(PikaObj* self) {
    obj_setInt(self, "__iter_i", 0);
    return arg_setRef(NULL, "", self);
}

Arg* PikaStdData_dict_keys___next__(PikaObj* self) {
    int __iter_i = args_getInt(self->list, "__iter_i");
    PikaObj* dictptr = obj_getPtr(self, "dictptr");
    PikaDict* keys = obj_getPtr(dictptr, "_keys");
    Arg* res = arg_copy(args_getArgByidex(&keys->super, __iter_i));
    if (NULL == res) {
        return arg_setNull(NULL);
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return res;
}
