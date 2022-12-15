#include "PikaStdData_Tuple.h"
#include "PikaVM.h"
#include "dataStrs.h"

int PikaStdData_Tuple_len(PikaObj* self) {
    PikaList* list = obj_getPtr(self, "list");
    return pikaList_getSize(list);
}

Arg* PikaStdData_Tuple_get(PikaObj* self, int i) {
    PikaList* list = obj_getPtr(self, "list");
    return arg_copy(pikaList_getArg(list, i));
}

void PikaStdData_Tuple___init__(PikaObj* self) {
    __vm_List___init__(self);
}

Arg* PikaStdData_Tuple___iter__(PikaObj* self) {
    obj_setInt(self, "__iter_i", 0);
    return arg_newRef(self);
}

Arg* PikaStdData_Tuple___next__(PikaObj* self) {
    int __iter_i = args_getInt(self->list, "__iter_i");
    Arg* res = PikaStdData_Tuple_get(self, __iter_i);
    if (NULL == res) {
        return arg_newNull();
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return res;
}

Arg* PikaStdData_Tuple___getitem__(PikaObj* self, Arg* __key) {
    return PikaStdData_Tuple_get(self, arg_getInt(__key));
}

void PikaStdData_Tuple___del__(PikaObj* self) {
    Args* list = obj_getPtr(self, "list");
    args_deinit(list);
}

char* PikaStdLib_SysObj_str(PikaObj* self, Arg* arg);
char* PikaStdData_Tuple___str__(PikaObj* self) {
    Arg* str_arg = arg_newStr("(");
    PikaList* list = obj_getPtr(self, "list");

    int i = 0;
    while (PIKA_TRUE) {
        Arg* item = pikaList_getArg(list, i);
        if (NULL == item) {
            break;
        }
        if (i != 0) {
            str_arg = arg_strAppend(str_arg, ", ");
        }
        char* item_str = PikaStdLib_SysObj_str(self, item);
        if (arg_getType(item) == ARG_TYPE_STRING) {
            str_arg = arg_strAppend(str_arg, "'");
        }
        str_arg = arg_strAppend(str_arg, item_str);
        if (arg_getType(item) == ARG_TYPE_STRING) {
            str_arg = arg_strAppend(str_arg, "'");
        }
        i++;
    }

    str_arg = arg_strAppend(str_arg, ")");
    obj_setStr(self, "_buf", arg_getStr(str_arg));
    arg_deinit(str_arg);
    return obj_getStr(self, "_buf");
}

int PikaStdData_Tuple___len__(PikaObj* self) {
    return PikaStdData_Tuple_len(self);
}

int PikaStdData_Tuple___contains__(PikaObj* self, Arg* val) {
    PikaList* list = obj_getPtr(self, "list");
    for (size_t i = 0; i < pikaList_getSize(list); i++) {
        Arg* arg = pikaList_getArg(list, i);
        if (arg_isEqual(arg, val)) {
            return 1;
        }
    }
    return 0;
}
