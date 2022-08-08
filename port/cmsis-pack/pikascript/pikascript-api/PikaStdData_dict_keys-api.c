/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdData_dict_keys.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdData_dict_keys___iter__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_dict_keys___iter__(self);
    method_returnArg(args, res);
}

void PikaStdData_dict_keys___len__Method(PikaObj *self, Args *args){
    int res = PikaStdData_dict_keys___len__(self);
    method_returnInt(args, res);
}

void PikaStdData_dict_keys___next__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_dict_keys___next__(self);
    method_returnArg(args, res);
}

void PikaStdData_dict_keys___str__Method(PikaObj *self, Args *args){
    char* res = PikaStdData_dict_keys___str__(self);
    method_returnStr(args, res);
}

PikaObj *New_PikaStdData_dict_keys(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__iter__()->any", PikaStdData_dict_keys___iter__Method);
    class_defineMethod(self, "__len__()->int", PikaStdData_dict_keys___len__Method);
    class_defineMethod(self, "__next__()->any", PikaStdData_dict_keys___next__Method);
    class_defineMethod(self, "__str__()->str", PikaStdData_dict_keys___str__Method);
    return self;
}

Arg *PikaStdData_dict_keys(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_dict_keys);
}
