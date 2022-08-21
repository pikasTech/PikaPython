/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdData_FILEIO.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdData_FILEIO_closeMethod(PikaObj *self, Args *args){
    PikaStdData_FILEIO_close(self);
}

void PikaStdData_FILEIO_initMethod(PikaObj *self, Args *args){
    char* path = args_getStr(args, "path");
    char* mode = args_getStr(args, "mode");
    PikaStdData_FILEIO_init(self, path, mode);
}

void PikaStdData_FILEIO_readMethod(PikaObj *self, Args *args){
    int size = args_getInt(args, "size");
    Arg* res = PikaStdData_FILEIO_read(self, size);
    method_returnArg(args, res);
}

void PikaStdData_FILEIO_readlineMethod(PikaObj *self, Args *args){
    char* res = PikaStdData_FILEIO_readline(self);
    method_returnStr(args, res);
}

void PikaStdData_FILEIO_readlinesMethod(PikaObj *self, Args *args){
    PikaObj* res = PikaStdData_FILEIO_readlines(self);
    method_returnObj(args, res);
}

void PikaStdData_FILEIO_seekMethod(PikaObj *self, Args *args){
    int offset = args_getInt(args, "offset");
    PikaTuple* fromwhere = args_getPtr(args, "fromwhere");
    int res = PikaStdData_FILEIO_seek(self, offset, fromwhere);
    method_returnInt(args, res);
}

void PikaStdData_FILEIO_tellMethod(PikaObj *self, Args *args){
    int res = PikaStdData_FILEIO_tell(self);
    method_returnInt(args, res);
}

void PikaStdData_FILEIO_writeMethod(PikaObj *self, Args *args){
    Arg* s = args_getArg(args, "s");
    int res = PikaStdData_FILEIO_write(self, s);
    method_returnInt(args, res);
}

void PikaStdData_FILEIO_writelinesMethod(PikaObj *self, Args *args){
    PikaObj* lines = args_getPtr(args, "lines");
    PikaStdData_FILEIO_writelines(self, lines);
}

PikaObj *New_PikaStdData_FILEIO(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "close()", PikaStdData_FILEIO_closeMethod);
    class_defineMethod(self, "init(path:str,mode:str)", PikaStdData_FILEIO_initMethod);
    class_defineMethod(self, "read(size:int)->any", PikaStdData_FILEIO_readMethod);
    class_defineMethod(self, "readline()->str", PikaStdData_FILEIO_readlineMethod);
    class_defineMethod(self, "readlines()->List", PikaStdData_FILEIO_readlinesMethod);
    class_defineMethod(self, "seek(offset:int,*fromwhere)->int", PikaStdData_FILEIO_seekMethod);
    class_defineMethod(self, "tell()->int", PikaStdData_FILEIO_tellMethod);
    class_defineMethod(self, "write(s:any)->int", PikaStdData_FILEIO_writeMethod);
    class_defineMethod(self, "writelines(lines:List)", PikaStdData_FILEIO_writelinesMethod);
    return self;
}

Arg *PikaStdData_FILEIO(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_FILEIO);
}
