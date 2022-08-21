/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdData_Tuple.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdData_Tuple___del__Method(PikaObj *self, Args *args){
    PikaStdData_Tuple___del__(self);
}

void PikaStdData_Tuple___getitem__Method(PikaObj *self, Args *args){
    Arg* __key = args_getArg(args, "__key");
    Arg* res = PikaStdData_Tuple___getitem__(self, __key);
    method_returnArg(args, res);
}

void PikaStdData_Tuple___init__Method(PikaObj *self, Args *args){
    PikaStdData_Tuple___init__(self);
}

void PikaStdData_Tuple___iter__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Tuple___iter__(self);
    method_returnArg(args, res);
}

void PikaStdData_Tuple___len__Method(PikaObj *self, Args *args){
    int res = PikaStdData_Tuple___len__(self);
    method_returnInt(args, res);
}

void PikaStdData_Tuple___next__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Tuple___next__(self);
    method_returnArg(args, res);
}

void PikaStdData_Tuple___str__Method(PikaObj *self, Args *args){
    char* res = PikaStdData_Tuple___str__(self);
    method_returnStr(args, res);
}

void PikaStdData_Tuple_getMethod(PikaObj *self, Args *args){
    int i = args_getInt(args, "i");
    Arg* res = PikaStdData_Tuple_get(self, i);
    method_returnArg(args, res);
}

void PikaStdData_Tuple_lenMethod(PikaObj *self, Args *args){
    int res = PikaStdData_Tuple_len(self);
    method_returnInt(args, res);
}

PikaObj *New_PikaStdData_Tuple(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__del__()", PikaStdData_Tuple___del__Method);
    class_defineMethod(self, "__getitem__(__key:any)->any", PikaStdData_Tuple___getitem__Method);
    class_defineMethod(self, "__init__()", PikaStdData_Tuple___init__Method);
    class_defineMethod(self, "__iter__()->any", PikaStdData_Tuple___iter__Method);
    class_defineMethod(self, "__len__()->int", PikaStdData_Tuple___len__Method);
    class_defineMethod(self, "__next__()->any", PikaStdData_Tuple___next__Method);
    class_defineMethod(self, "__str__()->str", PikaStdData_Tuple___str__Method);
    class_defineMethod(self, "get(i:int)->any", PikaStdData_Tuple_getMethod);
    class_defineMethod(self, "len()->int", PikaStdData_Tuple_lenMethod);
    return self;
}

Arg *PikaStdData_Tuple(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_Tuple);
}
