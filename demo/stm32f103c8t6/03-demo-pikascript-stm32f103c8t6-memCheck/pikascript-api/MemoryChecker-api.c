/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "MemoryChecker.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void MemoryChecker_maxMethod(PikaObj *self, Args *args){
    MemoryChecker_max(self);
}

void MemoryChecker_nowMethod(PikaObj *self, Args *args){
    MemoryChecker_now(self);
}

PikaObj *New_MemoryChecker(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "max()", MemoryChecker_maxMethod);
    class_defineMethod(self, "now()", MemoryChecker_nowMethod);
    return self;
}
