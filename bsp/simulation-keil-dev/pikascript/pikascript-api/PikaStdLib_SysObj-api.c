/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdLib_SysObj.h"
#include "BaseObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdLib_SysObj_floatMethod(PikaObj *self, Args *args){
    Arg * arg = args_getArg(args, "arg");
    float res = PikaStdLib_SysObj_float(self, arg);
    method_returnFloat(args, res);
}

void PikaStdLib_SysObj_intMethod(PikaObj *self, Args *args){
    Arg * arg = args_getArg(args, "arg");
    int res = PikaStdLib_SysObj_int(self, arg);
    method_returnInt(args, res);
}

void PikaStdLib_SysObj_iterMethod(PikaObj *self, Args *args){
    Arg * arg = args_getArg(args, "arg");
    Arg * res = PikaStdLib_SysObj_iter(self, arg);
    method_returnArg(args, res);
}

void PikaStdLib_SysObj_nextMethod(PikaObj *self, Args *args){
    Arg * arg = args_getArg(args, "arg");
    Arg * res = PikaStdLib_SysObj_next(self, arg);
    method_returnArg(args, res);
}

void PikaStdLib_SysObj_rangeMethod(PikaObj *self, Args *args){
    int a1 = args_getInt(args, "a1");
    int a2 = args_getInt(args, "a2");
    int a3 = args_getInt(args, "a3");
    Arg * res = PikaStdLib_SysObj_range(self, a1, a2, a3);
    method_returnArg(args, res);
}

void PikaStdLib_SysObj_removeMethod(PikaObj *self, Args *args){
    char * argPath = args_getStr(args, "argPath");
    PikaStdLib_SysObj_remove(self, argPath);
}

void PikaStdLib_SysObj_typeMethod(PikaObj *self, Args *args){
    Arg * arg = args_getArg(args, "arg");
    PikaStdLib_SysObj_type(self, arg);
}

PikaObj *New_PikaStdLib_SysObj(Args *args){
    PikaObj *self = New_BaseObj(args);
    class_defineMethod(self, "float(arg:any)->float", PikaStdLib_SysObj_floatMethod);
    class_defineMethod(self, "int(arg:any)->int", PikaStdLib_SysObj_intMethod);
    class_defineMethod(self, "iter(arg:any)->any", PikaStdLib_SysObj_iterMethod);
    class_defineMethod(self, "next(arg:any)->any", PikaStdLib_SysObj_nextMethod);
    class_defineMethod(self, "range(a1:int,a2:int,a3:int)->any", PikaStdLib_SysObj_rangeMethod);
    class_defineMethod(self, "remove(argPath:str)", PikaStdLib_SysObj_removeMethod);
    class_defineMethod(self, "type(arg:any)", PikaStdLib_SysObj_typeMethod);
    return self;
}

Arg *PikaStdLib_SysObj(PikaObj *self){
    return arg_setMetaObj("", "PikaStdLib_SysObj", New_PikaStdLib_SysObj);
}
