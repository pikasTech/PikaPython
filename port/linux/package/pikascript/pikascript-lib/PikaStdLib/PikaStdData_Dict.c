#include "PikaStdData_Dict.h"
#include "BaseObj.h"
#include "PikaObj.h"
#include "PikaStdData_dict_keys.h"
#include "PikaStdLib_SysObj.h"
#include "dataStrs.h"

Arg* PikaStdData_Dict_get(PikaObj* self, char* key) {
    PikaDict* dict = obj_getPtr(self, "dict");
    return arg_copy(dict_getArg(dict, key));
}

void PikaStdData_Dict___init__(PikaObj* self) {
    __vm_Dict___init__(self);
}

void PikaStdData_Dict_set(PikaObj* self, Arg* arg, char* key) {
    __vm_Dict_set(self, arg, key);
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

char* PikaStdLib_SysObj_str(PikaObj* self, Arg* arg);
char* PikaStdData_dict_keys___str__(PikaObj* self) {
    Arg* str_arg = arg_setStr(NULL, "", "dict_keys([");
    PikaObj* dictptr = obj_getPtr(self, "dictptr");
    PikaDict* keys = obj_getPtr(dictptr, "_keys");

    int i = 0;
    while (PIKA_TRUE) {
        Arg* item = args_getArgByidex(&keys->super, i);
        if (NULL == item) {
            break;
        }
        if (i != 0) {
            str_arg = arg_strAppend(str_arg, ", ");
        }
        char* item_str = PikaStdLib_SysObj_str(self, item);
        str_arg = arg_strAppend(str_arg, item_str);
        i++;
    }

    str_arg = arg_strAppend(str_arg, "])");
    obj_setStr(self, "_buf", arg_getStr(str_arg));
    arg_deinit(str_arg);
    return obj_getStr(self, "_buf");
}

char* PikaStdData_Dict___str__(PikaObj* self) {
    Arg* str_arg = arg_setStr(NULL, "", "{");

    PikaDict* keys = obj_getPtr(self, "_keys");
    PikaDict* dict = obj_getPtr(self, "dict");

    int i = 0;
    while (PIKA_TRUE) {
        Arg* item_key = args_getArgByidex(&keys->super, i);
        Arg* item_val = args_getArgByidex(&dict->super, i);
        if (NULL == item_key) {
            break;
        }
        if (i != 0) {
            str_arg = arg_strAppend(str_arg, ", ");
        }
        char* key_str = PikaStdLib_SysObj_str(self, item_key);
        str_arg = arg_strAppend(str_arg, "'");
        str_arg = arg_strAppend(str_arg, key_str);
        str_arg = arg_strAppend(str_arg, "'");
        str_arg = arg_strAppend(str_arg, ": ");

        char* val_str = PikaStdLib_SysObj_str(self, item_val);
        str_arg = arg_strAppend(str_arg, val_str);
        i++;
    }

    str_arg = arg_strAppend(str_arg, "}");
    obj_setStr(self, "_buf", arg_getStr(str_arg));
    arg_deinit(str_arg);
    return obj_getStr(self, "_buf");
}

int PikaStdData_Dict___len__(PikaObj* self) {
    PikaDict* dict = obj_getPtr(self, "dict");
    return args_getSize(&dict->super);
}


int PikaStdData_dict_keys___len__(PikaObj *self){
    PikaObj* dictptr = obj_getPtr(self, "dictptr");
    PikaDict* keys = obj_getPtr(dictptr, "_keys");
    return args_getSize(&keys->super);
}
