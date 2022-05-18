/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdLib_PikaObj.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

PikaObj *New_PikaStdLib_PikaObj(Args *args){
    PikaObj *self = New_TinyObj(args);
    return self;
}

Arg *PikaStdLib_PikaObj(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdLib_PikaObj);
}
