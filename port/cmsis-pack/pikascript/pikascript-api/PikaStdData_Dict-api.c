/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdData_Dict.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdData_Dict___del__Method(PikaObj *self, Args *args){
    PikaStdData_Dict___del__(self);
}

void PikaStdData_Dict___getitem__Method(PikaObj *self, Args *args){
    Arg* __key = args_getArg(args, "__key");
    Arg* res = PikaStdData_Dict___getitem__(self, __key);
    method_returnArg(args, res);
}

void PikaStdData_Dict___init__Method(PikaObj *self, Args *args){
    PikaStdData_Dict___init__(self);
}

void PikaStdData_Dict___iter__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Dict___iter__(self);
    method_returnArg(args, res);
}

void PikaStdData_Dict___len__Method(PikaObj *self, Args *args){
    int res = PikaStdData_Dict___len__(self);
    method_returnInt(args, res);
}

void PikaStdData_Dict___next__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Dict___next__(self);
    method_returnArg(args, res);
}

void PikaStdData_Dict___setitem__Method(PikaObj *self, Args *args){
    Arg* __key = args_getArg(args, "__key");
    Arg* __val = args_getArg(args, "__val");
    PikaStdData_Dict___setitem__(self, __key, __val);
}

void PikaStdData_Dict___str__Method(PikaObj *self, Args *args){
    char* res = PikaStdData_Dict___str__(self);
    method_returnStr(args, res);
}

void PikaStdData_Dict_getMethod(PikaObj *self, Args *args){
    char* key = args_getStr(args, "key");
    Arg* res = PikaStdData_Dict_get(self, key);
    method_returnArg(args, res);
}

void PikaStdData_Dict_keysMethod(PikaObj *self, Args *args){
    PikaObj* res = PikaStdData_Dict_keys(self);
    method_returnObj(args, res);
}

void PikaStdData_Dict_removeMethod(PikaObj *self, Args *args){
    char* key = args_getStr(args, "key");
    PikaStdData_Dict_remove(self, key);
}

void PikaStdData_Dict_setMethod(PikaObj *self, Args *args){
    char* key = args_getStr(args, "key");
    Arg* arg = args_getArg(args, "arg");
    PikaStdData_Dict_set(self, key, arg);
}

PikaObj *New_PikaStdData_Dict(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__del__()", PikaStdData_Dict___del__Method);
    class_defineMethod(self, "__getitem__(__key:any)->any", PikaStdData_Dict___getitem__Method);
    class_defineMethod(self, "__init__()", PikaStdData_Dict___init__Method);
    class_defineMethod(self, "__iter__()->any", PikaStdData_Dict___iter__Method);
    class_defineMethod(self, "__len__()->int", PikaStdData_Dict___len__Method);
    class_defineMethod(self, "__next__()->any", PikaStdData_Dict___next__Method);
    class_defineMethod(self, "__setitem__(__key:any,__val:any)", PikaStdData_Dict___setitem__Method);
    class_defineMethod(self, "__str__()->str", PikaStdData_Dict___str__Method);
    class_defineMethod(self, "get(key:str)->any", PikaStdData_Dict_getMethod);
    class_defineMethod(self, "keys()->dict_keys", PikaStdData_Dict_keysMethod);
    class_defineMethod(self, "remove(key:str)", PikaStdData_Dict_removeMethod);
    class_defineMethod(self, "set(key:str,arg:any)", PikaStdData_Dict_setMethod);
    return self;
}

Arg *PikaStdData_Dict(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_Dict);
}
