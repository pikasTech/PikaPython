/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdData_ByteArray.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdData_ByteArray___getitem__Method(PikaObj *self, Args *args){
    int __key = args_getInt(args, "__key");
    int res = PikaStdData_ByteArray___getitem__(self, __key);
    method_returnInt(args, res);
}

void PikaStdData_ByteArray___init__Method(PikaObj *self, Args *args){
    Arg* bytes = args_getArg(args, "bytes");
    PikaStdData_ByteArray___init__(self, bytes);
}

void PikaStdData_ByteArray___iter__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_ByteArray___iter__(self);
    method_returnArg(args, res);
}

void PikaStdData_ByteArray___next__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_ByteArray___next__(self);
    method_returnArg(args, res);
}

void PikaStdData_ByteArray___setitem__Method(PikaObj *self, Args *args){
    int __key = args_getInt(args, "__key");
    int __val = args_getInt(args, "__val");
    PikaStdData_ByteArray___setitem__(self, __key, __val);
}

void PikaStdData_ByteArray___str__Method(PikaObj *self, Args *args){
    char* res = PikaStdData_ByteArray___str__(self);
    method_returnStr(args, res);
}

void PikaStdData_ByteArray_decodeMethod(PikaObj *self, Args *args){
    char* res = PikaStdData_ByteArray_decode(self);
    method_returnStr(args, res);
}

PikaObj *New_PikaStdData_ByteArray(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__getitem__(__key:int)->int", PikaStdData_ByteArray___getitem__Method);
    class_defineMethod(self, "__init__(bytes:any)", PikaStdData_ByteArray___init__Method);
    class_defineMethod(self, "__iter__()->any", PikaStdData_ByteArray___iter__Method);
    class_defineMethod(self, "__next__()->any", PikaStdData_ByteArray___next__Method);
    class_defineMethod(self, "__setitem__(__key:int,__val:int)", PikaStdData_ByteArray___setitem__Method);
    class_defineMethod(self, "__str__()->str", PikaStdData_ByteArray___str__Method);
    class_defineMethod(self, "decode()->str", PikaStdData_ByteArray_decodeMethod);
    return self;
}

Arg *PikaStdData_ByteArray(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_ByteArray);
}
