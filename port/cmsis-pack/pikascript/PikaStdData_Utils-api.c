/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdData_Utils.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdData_Utils_int_to_bytesMethod(PikaObj *self, Args *args){
    int val = args_getInt(args, "val");
    Arg* res = PikaStdData_Utils_int_to_bytes(self, val);
    method_returnArg(args, res);
}

PikaObj *New_PikaStdData_Utils(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "int_to_bytes(val:int)->bytes", PikaStdData_Utils_int_to_bytesMethod);
    return self;
}

Arg *PikaStdData_Utils(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_Utils);
}
