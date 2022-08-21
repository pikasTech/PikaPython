/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdLib_SysObj.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdLib_SysObj___getitem__Method(PikaObj *self, Args *args){
    Arg* obj = args_getArg(args, "obj");
    Arg* key = args_getArg(args, "key");
    Arg* res = PikaStdLib_SysObj___getitem__(self, obj, key);
    method_returnArg(args, res);
}

void PikaStdLib_SysObj___setitem__Method(PikaObj *self, Args *args){
    Arg* obj = args_getArg(args, "obj");
    Arg* key = args_getArg(args, "key");
    Arg* val = args_getArg(args, "val");
    Arg* res = PikaStdLib_SysObj___setitem__(self, obj, key, val);
    method_returnArg(args, res);
}

void PikaStdLib_SysObj___slice__Method(PikaObj *self, Args *args){
    Arg* obj = args_getArg(args, "obj");
    Arg* start = args_getArg(args, "start");
    Arg* end = args_getArg(args, "end");
    int step = args_getInt(args, "step");
    Arg* res = PikaStdLib_SysObj___slice__(self, obj, start, end, step);
    method_returnArg(args, res);
}

void PikaStdLib_SysObj_bytesMethod(PikaObj *self, Args *args){
    Arg* val = args_getArg(args, "val");
    Arg* res = PikaStdLib_SysObj_bytes(self, val);
    method_returnArg(args, res);
}

void PikaStdLib_SysObj_cformatMethod(PikaObj *self, Args *args){
    char* fmt = args_getStr(args, "fmt");
    PikaTuple* var = args_getPtr(args, "var");
    char* res = PikaStdLib_SysObj_cformat(self, fmt, var);
    method_returnStr(args, res);
}

void PikaStdLib_SysObj_chrMethod(PikaObj *self, Args *args){
    int val = args_getInt(args, "val");
    char* res = PikaStdLib_SysObj_chr(self, val);
    method_returnStr(args, res);
}

void PikaStdLib_SysObj_dictMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdLib_SysObj_dict(self);
    method_returnArg(args, res);
}

void PikaStdLib_SysObj_floatMethod(PikaObj *self, Args *args){
    Arg* arg = args_getArg(args, "arg");
    double res = PikaStdLib_SysObj_float(self, arg);
    method_returnFloat(args, res);
}

void PikaStdLib_SysObj_hexMethod(PikaObj *self, Args *args){
    int val = args_getInt(args, "val");
    char* res = PikaStdLib_SysObj_hex(self, val);
    method_returnStr(args, res);
}

void PikaStdLib_SysObj_idMethod(PikaObj *self, Args *args){
    Arg* obj = args_getArg(args, "obj");
    int res = PikaStdLib_SysObj_id(self, obj);
    method_returnInt(args, res);
}

void PikaStdLib_SysObj_intMethod(PikaObj *self, Args *args){
    Arg* arg = args_getArg(args, "arg");
    int res = PikaStdLib_SysObj_int(self, arg);
    method_returnInt(args, res);
}

void PikaStdLib_SysObj_iterMethod(PikaObj *self, Args *args){
    Arg* arg = args_getArg(args, "arg");
    Arg* res = PikaStdLib_SysObj_iter(self, arg);
    method_returnArg(args, res);
}

void PikaStdLib_SysObj_lenMethod(PikaObj *self, Args *args){
    Arg* arg = args_getArg(args, "arg");
    int res = PikaStdLib_SysObj_len(self, arg);
    method_returnInt(args, res);
}

void PikaStdLib_SysObj_listMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdLib_SysObj_list(self);
    method_returnArg(args, res);
}

void PikaStdLib_SysObj_openMethod(PikaObj *self, Args *args){
    char* path = args_getStr(args, "path");
    char* mode = args_getStr(args, "mode");
    PikaObj* res = PikaStdLib_SysObj_open(self, path, mode);
    method_returnObj(args, res);
}

void PikaStdLib_SysObj_ordMethod(PikaObj *self, Args *args){
    char* val = args_getStr(args, "val");
    int res = PikaStdLib_SysObj_ord(self, val);
    method_returnInt(args, res);
}

void PikaStdLib_SysObj_printMethod(PikaObj *self, Args *args){
    PikaTuple* val = args_getPtr(args, "val");
    PikaStdLib_SysObj_print(self, val);
}

void PikaStdLib_SysObj_printNoEndMethod(PikaObj *self, Args *args){
    Arg* val = args_getArg(args, "val");
    PikaStdLib_SysObj_printNoEnd(self, val);
}

void PikaStdLib_SysObj_rangeMethod(PikaObj *self, Args *args){
    int a1 = args_getInt(args, "a1");
    int a2 = args_getInt(args, "a2");
    Arg* res = PikaStdLib_SysObj_range(self, a1, a2);
    method_returnArg(args, res);
}

void PikaStdLib_SysObj_removeMethod(PikaObj *self, Args *args){
    char* argPath = args_getStr(args, "argPath");
    PikaStdLib_SysObj_remove(self, argPath);
}

void PikaStdLib_SysObj_strMethod(PikaObj *self, Args *args){
    Arg* arg = args_getArg(args, "arg");
    char* res = PikaStdLib_SysObj_str(self, arg);
    method_returnStr(args, res);
}

void PikaStdLib_SysObj_typeMethod(PikaObj *self, Args *args){
    Arg* arg = args_getArg(args, "arg");
    Arg* res = PikaStdLib_SysObj_type(self, arg);
    method_returnArg(args, res);
}

PikaObj *New_PikaStdLib_SysObj(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__getitem__(obj:any,key:any)->any", PikaStdLib_SysObj___getitem__Method);
    class_defineMethod(self, "__setitem__(obj:any,key:any,val:any)->any", PikaStdLib_SysObj___setitem__Method);
    class_defineMethod(self, "__slice__(obj:any,start:any,end:any,step:int)->any", PikaStdLib_SysObj___slice__Method);
    class_defineMethod(self, "bytes(val:any)->bytes", PikaStdLib_SysObj_bytesMethod);
    class_defineMethod(self, "cformat(fmt:str,*var)->str", PikaStdLib_SysObj_cformatMethod);
    class_defineMethod(self, "chr(val:int)->str", PikaStdLib_SysObj_chrMethod);
    class_defineMethod(self, "dict()->any", PikaStdLib_SysObj_dictMethod);
    class_defineMethod(self, "float(arg:any)->float", PikaStdLib_SysObj_floatMethod);
    class_defineMethod(self, "hex(val:int)->str", PikaStdLib_SysObj_hexMethod);
    class_defineMethod(self, "id(obj:any)->int", PikaStdLib_SysObj_idMethod);
    class_defineMethod(self, "int(arg:any)->int", PikaStdLib_SysObj_intMethod);
    class_defineMethod(self, "iter(arg:any)->any", PikaStdLib_SysObj_iterMethod);
    class_defineMethod(self, "len(arg:any)->int", PikaStdLib_SysObj_lenMethod);
    class_defineMethod(self, "list()->any", PikaStdLib_SysObj_listMethod);
    class_defineMethod(self, "open(path:str,mode:str)->object", PikaStdLib_SysObj_openMethod);
    class_defineMethod(self, "ord(val:str)->int", PikaStdLib_SysObj_ordMethod);
    class_defineMethod(self, "print(*val)", PikaStdLib_SysObj_printMethod);
    class_defineMethod(self, "printNoEnd(val:any)", PikaStdLib_SysObj_printNoEndMethod);
    class_defineMethod(self, "range(a1:int,a2:int)->any", PikaStdLib_SysObj_rangeMethod);
    class_defineMethod(self, "remove(argPath:str)", PikaStdLib_SysObj_removeMethod);
    class_defineMethod(self, "str(arg:any)->str", PikaStdLib_SysObj_strMethod);
    class_defineMethod(self, "type(arg:any)->any", PikaStdLib_SysObj_typeMethod);
    return self;
}

Arg *PikaStdLib_SysObj(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdLib_SysObj);
}
