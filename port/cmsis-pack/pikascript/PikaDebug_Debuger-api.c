/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaDebug_Debuger.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaDebug_Debuger___init__Method(PikaObj *self, Args *args){
    PikaDebug_Debuger___init__(self);
}

void PikaDebug_Debuger_set_traceMethod(PikaObj *self, Args *args){
    PikaDebug_Debuger_set_trace(self);
}

PikaObj *New_PikaDebug_Debuger(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__init__()", PikaDebug_Debuger___init__Method);
    class_defineMethod(self, "set_trace()", PikaDebug_Debuger_set_traceMethod);
    return self;
}

Arg *PikaDebug_Debuger(PikaObj *self){
    return obj_newObjInPackage(New_PikaDebug_Debuger);
}
