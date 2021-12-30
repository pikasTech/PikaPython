/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdLib_MemChecker.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdLib_MemChecker_getMaxMethod(PikaObj *self, Args *args){
    float res = PikaStdLib_MemChecker_getMax(self);
    method_returnFloat(args, res);
}

void PikaStdLib_MemChecker_getNowMethod(PikaObj *self, Args *args){
    float res = PikaStdLib_MemChecker_getNow(self);
    method_returnFloat(args, res);
}

void PikaStdLib_MemChecker_maxMethod(PikaObj *self, Args *args){
    PikaStdLib_MemChecker_max(self);
}

void PikaStdLib_MemChecker_nowMethod(PikaObj *self, Args *args){
    PikaStdLib_MemChecker_now(self);
}

void PikaStdLib_MemChecker_resetMaxMethod(PikaObj *self, Args *args){
    PikaStdLib_MemChecker_resetMax(self);
}

PikaObj *New_PikaStdLib_MemChecker(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "getMax()->float", PikaStdLib_MemChecker_getMaxMethod);
    class_defineMethod(self, "getNow()->float", PikaStdLib_MemChecker_getNowMethod);
    class_defineMethod(self, "max()", PikaStdLib_MemChecker_maxMethod);
    class_defineMethod(self, "now()", PikaStdLib_MemChecker_nowMethod);
    class_defineMethod(self, "resetMax()", PikaStdLib_MemChecker_resetMaxMethod);
    return self;
}

Arg *PikaStdLib_MemChecker(PikaObj *self){
    return arg_setMetaObj("", "PikaStdLib_MemChecker", New_PikaStdLib_MemChecker);
}
