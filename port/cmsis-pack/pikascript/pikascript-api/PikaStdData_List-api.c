/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdData_List.h"
#include "PikaStdData_Tuple.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdData_List___init__Method(PikaObj *self, Args *args){
    PikaStdData_List___init__(self);
}

void PikaStdData_List___setitem__Method(PikaObj *self, Args *args){
    Arg* __key = args_getArg(args, "__key");
    Arg* __val = args_getArg(args, "__val");
    PikaStdData_List___setitem__(self, __key, __val);
}

void PikaStdData_List___str__Method(PikaObj *self, Args *args){
    char* res = PikaStdData_List___str__(self);
    method_returnStr(args, res);
}

void PikaStdData_List_appendMethod(PikaObj *self, Args *args){
    Arg* arg = args_getArg(args, "arg");
    PikaStdData_List_append(self, arg);
}

void PikaStdData_List_setMethod(PikaObj *self, Args *args){
    int i = args_getInt(args, "i");
    Arg* arg = args_getArg(args, "arg");
    PikaStdData_List_set(self, i, arg);
}

PikaObj *New_PikaStdData_List(Args *args){
    PikaObj *self = New_PikaStdData_Tuple(args);
    class_defineMethod(self, "__init__()", PikaStdData_List___init__Method);
    class_defineMethod(self, "__setitem__(__key:any,__val:any)", PikaStdData_List___setitem__Method);
    class_defineMethod(self, "__str__()->str", PikaStdData_List___str__Method);
    class_defineMethod(self, "append(arg:any)", PikaStdData_List_appendMethod);
    class_defineMethod(self, "set(i:int,arg:any)", PikaStdData_List_setMethod);
    return self;
}

Arg *PikaStdData_List(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_List);
}
