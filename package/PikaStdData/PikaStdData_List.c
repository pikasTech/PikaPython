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
