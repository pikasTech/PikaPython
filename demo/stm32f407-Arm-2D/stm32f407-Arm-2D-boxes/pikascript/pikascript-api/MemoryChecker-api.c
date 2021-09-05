/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "MemoryChecker.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void MemoryChecker_maxMethod(MimiObj *self, Args *args){
    MemoryChecker_max(self);
}

void MemoryChecker_nowMethod(MimiObj *self, Args *args){
    MemoryChecker_now(self);
}

void MemoryChecker_sizeMethod(MimiObj *self, Args *args){
    MemoryChecker_size(self);
}

MimiObj *New_MemoryChecker(Args *args){
    MimiObj *self = New_TinyObj(args);
    class_defineMethod(self, "max()", MemoryChecker_maxMethod);
    class_defineMethod(self, "now()", MemoryChecker_nowMethod);
    class_defineMethod(self, "size()", MemoryChecker_sizeMethod);
    return self;
}
