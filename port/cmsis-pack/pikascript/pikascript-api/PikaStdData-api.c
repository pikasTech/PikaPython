/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdData.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdData_ByteArrayMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_ByteArray(self);
    method_returnArg(args, res);
}

void PikaStdData_DictMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Dict(self);
    method_returnArg(args, res);
}

void PikaStdData_FILEIOMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_FILEIO(self);
    method_returnArg(args, res);
}

void PikaStdData_ListMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_List(self);
    method_returnArg(args, res);
}

void PikaStdData_StringMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_String(self);
    method_returnArg(args, res);
}

void PikaStdData_TupleMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Tuple(self);
    method_returnArg(args, res);
}

void PikaStdData_UtilsMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Utils(self);
    method_returnArg(args, res);
}

void PikaStdData_dict_keysMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_dict_keys(self);
    method_returnArg(args, res);
}

PikaObj *New_PikaStdData(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineConstructor(self, "ByteArray()->any", PikaStdData_ByteArrayMethod);
    class_defineConstructor(self, "Dict()->any", PikaStdData_DictMethod);
    class_defineConstructor(self, "FILEIO()->any", PikaStdData_FILEIOMethod);
    class_defineConstructor(self, "List()->any", PikaStdData_ListMethod);
    class_defineConstructor(self, "String()->any", PikaStdData_StringMethod);
    class_defineConstructor(self, "Tuple()->any", PikaStdData_TupleMethod);
    class_defineConstructor(self, "Utils()->any", PikaStdData_UtilsMethod);
    class_defineConstructor(self, "dict_keys()->any", PikaStdData_dict_keysMethod);
    return self;
}

