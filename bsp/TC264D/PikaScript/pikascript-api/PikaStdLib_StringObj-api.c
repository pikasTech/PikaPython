/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdLib_StringObj.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdLib_StringObj___next__Method(PikaObj *self, Args *args){
    Arg * res = PikaStdLib_StringObj___next__(self);
    method_returnArg(args, res);
}

PikaObj *New_PikaStdLib_StringObj(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__next__()->any", PikaStdLib_StringObj___next__Method);
    return self;
}

Arg *PikaStdLib_StringObj(PikaObj *self){
    return arg_setMetaObj("", "PikaStdLib_StringObj", New_PikaStdLib_StringObj);
}
