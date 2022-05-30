/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdData_ByteArray.h"
#include "PikaStdData_List.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdData_ByteArray_fromStringMethod(PikaObj *self, Args *args){
    char* s = args_getStr(args, "s");
    PikaStdData_ByteArray_fromString(self, s);
}

PikaObj *New_PikaStdData_ByteArray(Args *args){
    PikaObj *self = New_PikaStdData_List(args);
    class_defineMethod(self, "fromString(s:str)", PikaStdData_ByteArray_fromStringMethod);
    return self;
}

Arg *PikaStdData_ByteArray(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_ByteArray);
}
