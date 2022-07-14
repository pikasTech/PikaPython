#include "BaseObj.h"
#include "PikaObj.h"
#include "dataStrs.h"

void PikaStdData_List_append(PikaObj* self, Arg* arg) {
    __vm_List_append(self, arg);
}

int PikaStdData_List_len(PikaObj* self) {
    PikaList* list = obj_getPtr(self, "list");
    return list_getSize(list);
}

Arg* PikaStdData_List_get(PikaObj* self, int i) {
    PikaList* list = obj_getPtr(self, "list");
    return arg_copy(list_getArg(list, i));
}

void PikaStdData_List___init__(PikaObj* self) {
    __vm_List___init__(self);
}

void PikaStdData_List_set(PikaObj* self, Arg* arg, int i) {
    PikaList* list = obj_getPtr(self, "list");
    if (PIKA_RES_OK != list_setArg(list, i, arg)) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error]: index exceeded lengh of list.");
    }
}

Arg* PikaStdData_List___iter__(PikaObj* self) {
    obj_setInt(self, "__iter_i", 0);
    return arg_setRef(NULL, "", self);
}

Arg* PikaStdData_List___next__(PikaObj* self) {
    int __iter_i = args_getInt(self->list, "__iter_i");
    Arg* res = PikaStdData_List_get(self, __iter_i);
    if (NULL == res) {
        return arg_setNull(NULL);
    }
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return res;
}

Arg* PikaStdData_List___get__(PikaObj* self) {
    return PikaStdData_List_get(self, obj_getInt(self, "__key"));
}

void PikaStdData_List___set__(PikaObj* self) {
    PikaStdData_List_set(self, obj_getArg(self, "__val"),
                         obj_getInt(self, "__key"));
}


void PikaStdData_List___del__(PikaObj* self) {
    Args* list = obj_getPtr(self, "list");
    args_deinit(list);
}

char* PikaStdLib_SysObj_str(PikaObj* self, Arg* arg);
char* PikaStdData_List___str__(PikaObj* self) {
    Arg* str_arg = arg_setStr(NULL, "", "[");
    PikaList* list = obj_getPtr(self, "list");

    int i = 0;
    while (PIKA_TRUE) {
        Arg* item = list_getArg(list, i);
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

    str_arg = arg_strAppend(str_arg, "]");
    obj_setStr(self, "_buf", arg_getStr(str_arg));
    arg_deinit(str_arg);
    return obj_getStr(self, "_buf");
}

int PikaStdData_List___len__(PikaObj *self){
    return PikaStdData_List_len(self);
}
