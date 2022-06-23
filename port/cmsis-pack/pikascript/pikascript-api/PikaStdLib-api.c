/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdLib.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdLib_MemCheckerMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdLib_MemChecker(self);
    method_returnArg(args, res);
}

void PikaStdLib_PikaObjMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdLib_PikaObj(self);
    method_returnArg(args, res);
}

void PikaStdLib_RangeObjMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdLib_RangeObj(self);
    method_returnArg(args, res);
}

void PikaStdLib_StringObjMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdLib_StringObj(self);
    method_returnArg(args, res);
}

void PikaStdLib_SysObjMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdLib_SysObj(self);
    method_returnArg(args, res);
}

PikaObj *New_PikaStdLib(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineConstructor(self, "MemChecker()->any", PikaStdLib_MemCheckerMethod);
    class_defineConstructor(self, "PikaObj()->any", PikaStdLib_PikaObjMethod);
    class_defineConstructor(self, "RangeObj()->any", PikaStdLib_RangeObjMethod);
    class_defineConstructor(self, "StringObj()->any", PikaStdLib_StringObjMethod);
    class_defineConstructor(self, "SysObj()->any", PikaStdLib_SysObjMethod);
    return self;
}

