/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdData_String.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdData_String___get__Method(PikaObj *self, Args *args){
    Arg* __key = args_getArg(args, "__key");
    Arg* res = PikaStdData_String___get__(self, __key);
    method_returnArg(args, res);
}

void PikaStdData_String___init__Method(PikaObj *self, Args *args){
    char* s = args_getStr(args, "s");
    PikaStdData_String___init__(self, s);
}

void PikaStdData_String___iter__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_String___iter__(self);
    method_returnArg(args, res);
}

void PikaStdData_String___next__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_String___next__(self);
    method_returnArg(args, res);
}

void PikaStdData_String___set__Method(PikaObj *self, Args *args){
    Arg* __key = args_getArg(args, "__key");
    Arg* __val = args_getArg(args, "__val");
    PikaStdData_String___set__(self, __key, __val);
}

void PikaStdData_String___str__Method(PikaObj *self, Args *args){
    char* res = PikaStdData_String___str__(self);
    method_returnStr(args, res);
}

void PikaStdData_String_endwithMethod(PikaObj *self, Args *args){
    char* suffix = args_getStr(args, "suffix");
    int res = PikaStdData_String_endwith(self, suffix);
    method_returnInt(args, res);
}

void PikaStdData_String_getMethod(PikaObj *self, Args *args){
    char* res = PikaStdData_String_get(self);
    method_returnStr(args, res);
}

void PikaStdData_String_isalnumMethod(PikaObj *self, Args *args){
    int res = PikaStdData_String_isalnum(self);
    method_returnInt(args, res);
}

void PikaStdData_String_isalphaMethod(PikaObj *self, Args *args){
    int res = PikaStdData_String_isalpha(self);
    method_returnInt(args, res);
}

void PikaStdData_String_isdigitMethod(PikaObj *self, Args *args){
    int res = PikaStdData_String_isdigit(self);
    method_returnInt(args, res);
}

void PikaStdData_String_islowerMethod(PikaObj *self, Args *args){
    int res = PikaStdData_String_islower(self);
    method_returnInt(args, res);
}

void PikaStdData_String_isspaceMethod(PikaObj *self, Args *args){
    int res = PikaStdData_String_isspace(self);
    method_returnInt(args, res);
}

void PikaStdData_String_setMethod(PikaObj *self, Args *args){
    char* s = args_getStr(args, "s");
    PikaStdData_String_set(self, s);
}

void PikaStdData_String_startwithMethod(PikaObj *self, Args *args){
    char* prefix = args_getStr(args, "prefix");
    int res = PikaStdData_String_startwith(self, prefix);
    method_returnInt(args, res);
}

PikaObj *New_PikaStdData_String(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__get__(__key:any)->any", PikaStdData_String___get__Method);
    class_defineMethod(self, "__init__(s:str)", PikaStdData_String___init__Method);
    class_defineMethod(self, "__iter__()->any", PikaStdData_String___iter__Method);
    class_defineMethod(self, "__next__()->any", PikaStdData_String___next__Method);
    class_defineMethod(self, "__set__(__key:any,__val:any)", PikaStdData_String___set__Method);
    class_defineMethod(self, "__str__()->str", PikaStdData_String___str__Method);
    class_defineMethod(self, "endwith(suffix:str)->int", PikaStdData_String_endwithMethod);
    class_defineMethod(self, "get()->str", PikaStdData_String_getMethod);
    class_defineMethod(self, "isalnum()->int", PikaStdData_String_isalnumMethod);
    class_defineMethod(self, "isalpha()->int", PikaStdData_String_isalphaMethod);
    class_defineMethod(self, "isdigit()->int", PikaStdData_String_isdigitMethod);
    class_defineMethod(self, "islower()->int", PikaStdData_String_islowerMethod);
    class_defineMethod(self, "isspace()->int", PikaStdData_String_isspaceMethod);
    class_defineMethod(self, "set(s:str)", PikaStdData_String_setMethod);
    class_defineMethod(self, "startwith(prefix:str)->int", PikaStdData_String_startwithMethod);
    return self;
}

Arg *PikaStdData_String(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_String);
}
