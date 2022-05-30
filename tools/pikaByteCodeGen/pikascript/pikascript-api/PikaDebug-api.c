/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaDebug.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaDebug_DebugerMethod(PikaObj *self, Args *args){
    Arg* res = PikaDebug_Debuger(self);
    method_returnArg(args, res);
}

PikaObj *New_PikaDebug(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineConstructor(self, "Debuger()->any", PikaDebug_DebugerMethod);
    return self;
}

