#include "PikaObj.h"

void PikaStdData_List_append(PikaObj* self, Arg* arg) {
    int top = obj_getInt(self, "top");
    char buff[11];
    char* topStr = fast_itoa(buff, top);
    obj_setArg(self, topStr, arg);
    /* top++ */
    obj_setInt(self, "top", top + 1);
}

int PikaStdData_List_len(PikaObj* self) {
    return obj_getInt(self, "top");
}

Arg* PikaStdData_List_get(PikaObj* self, int i) {
    char buff[11];
    char* index = fast_itoa(buff, i);
    return arg_copy(obj_getArg(self, index));
}
void PikaStdData_List___init__(PikaObj* self) {
    /* set top index for append */
    obj_setInt(self, "top", 0);
}

void PikaStdData_List_set(PikaObj* self, Arg* arg, int i) {
    char buff[11];
    char* i_str = fast_itoa(buff, i);
    int top = obj_getInt(self, "top");
    if (i > top) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error]: index exceeded lengh of list.");
    }
    obj_setArg(self, i_str, arg);
}

Arg* PikaStdData_List___iter__(PikaObj* self) {
    obj_setInt(self, "__iter_i", 0);
    return arg_setPtr(NULL, "", TYPE_POINTER, self);
}

Arg* PikaStdData_List___next__(PikaObj* self) {
    int __iter_i = args_getInt(self->list, "__iter_i");
    Arg* res = PikaStdData_List_get(self, __iter_i);
    args_setInt(self->list, "__iter_i", __iter_i + 1);
    return arg_copy(res);
}