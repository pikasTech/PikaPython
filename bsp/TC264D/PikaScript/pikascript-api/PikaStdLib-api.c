/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdLib.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdLib_MemCheckerMethod(PikaObj *self, Args *args){
    Arg * res = PikaStdLib_MemChecker(self);
    method_returnArg(args, res);
}

void PikaStdLib_RangeObjMethod(PikaObj *self, Args *args){
    Arg * res = PikaStdLib_RangeObj(self);
    method_returnArg(args, res);
}

void PikaStdLib_StringObjMethod(PikaObj *self, Args *args){
    Arg * res = PikaStdLib_StringObj(self);
    method_returnArg(args, res);
}

void PikaStdLib_SysObjMethod(PikaObj *self, Args *args){
    Arg * res = PikaStdLib_SysObj(self);
    method_returnArg(args, res);
}

PikaObj *New_PikaStdLib(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "MemChecker()->any", PikaStdLib_MemCheckerMethod);
    class_defineMethod(self, "RangeObj()->any", PikaStdLib_RangeObjMethod);
    class_defineMethod(self, "StringObj()->any", PikaStdLib_StringObjMethod);
    class_defineMethod(self, "SysObj()->any", PikaStdLib_SysObjMethod);
    return self;
}

