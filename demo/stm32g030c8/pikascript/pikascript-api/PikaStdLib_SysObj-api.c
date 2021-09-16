/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdLib_SysObj.h"
#include "BaseObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdLib_SysObj_lsMethod(PikaObj *self, Args *args){
    char * objPath = args_getStr(args, "objPath");
    PikaStdLib_SysObj_ls(self, objPath);
}

void PikaStdLib_SysObj_newMethod(PikaObj *self, Args *args){
    char * classPath = args_getStr(args, "classPath");
    char * objPath = args_getStr(args, "objPath");
    PikaStdLib_SysObj_new(self, classPath, objPath);
}

void PikaStdLib_SysObj_removeMethod(PikaObj *self, Args *args){
    char * argPath = args_getStr(args, "argPath");
    PikaStdLib_SysObj_remove(self, argPath);
}

void PikaStdLib_SysObj_typeMethod(PikaObj *self, Args *args){
    char * argPath = args_getStr(args, "argPath");
    PikaStdLib_SysObj_type(self, argPath);
}

PikaObj *New_PikaStdLib_SysObj(Args *args){
    PikaObj *self = New_BaseObj(args);
    class_defineMethod(self, "ls(objPath:str)", PikaStdLib_SysObj_lsMethod);
    class_defineMethod(self, "new(objPath:str,classPath:str)", PikaStdLib_SysObj_newMethod);
    class_defineMethod(self, "remove(argPath:str)", PikaStdLib_SysObj_removeMethod);
    class_defineMethod(self, "type(argPath:str)", PikaStdLib_SysObj_typeMethod);
    return self;
}
