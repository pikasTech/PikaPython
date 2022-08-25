/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"
#include "PikaDebug.h"
#include "TinyObj.h"
#include "PikaDebug_Debuger.h"
#include "TinyObj.h"
#include "PikaMain.h"
#include "PikaStdLib_SysObj.h"
#include "PikaStdLib.h"
#include "PikaStdData.h"
#include "TinyObj.h"
#include "PikaStdData_ByteArray.h"
#include "TinyObj.h"
#include "PikaStdData_Dict.h"
#include "TinyObj.h"
#include "PikaStdData_FILEIO.h"
#include "TinyObj.h"
#include "PikaStdData_List.h"
#include "PikaStdData_Tuple.h"
#include "PikaStdData_String.h"
#include "TinyObj.h"
#include "PikaStdData_Tuple.h"
#include "TinyObj.h"
#include "PikaStdData_Utils.h"
#include "TinyObj.h"
#include "PikaStdData_dict_keys.h"
#include "TinyObj.h"
#include "PikaStdLib.h"
#include "TinyObj.h"
#include "PikaStdLib_MemChecker.h"
#include "TinyObj.h"
#include "PikaStdLib_RangeObj.h"
#include "TinyObj.h"
#include "PikaStdLib_StringObj.h"
#include "TinyObj.h"
#include "PikaStdLib_SysObj.h"
#include "TinyObj.h"
#include "PikaStdTask.h"
#include "TinyObj.h"
#include "PikaStdTask_Task.h"
#include "PikaStdLib_SysObj.h"
#include "PikaStdData_List.h"
#include "pika_lvgl.h"
#include "TinyObj.h"
#include "pika_lvgl_ALIGN.h"
#include "TinyObj.h"
#include "pika_lvgl_ANIM.h"
#include "TinyObj.h"
#include "pika_lvgl_EVENT.h"
#include "TinyObj.h"
#include "pika_lvgl_OPA.h"
#include "TinyObj.h"
#include "pika_lvgl_PALETTE.h"
#include "TinyObj.h"
#include "pika_lvgl_STATE.h"
#include "TinyObj.h"
#include "pika_lvgl_arc.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_bar.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_btn.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_checkbox.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_dropdown.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_indev_t.h"
#include "TinyObj.h"
#include "pika_lvgl_label.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_lv_color_t.h"
#include "TinyObj.h"
#include "pika_lvgl_lv_event.h"
#include "TinyObj.h"
#include "pika_lvgl_lv_obj.h"
#include "TinyObj.h"
#include "pika_lvgl_lv_timer_t.h"
#include "TinyObj.h"
#include "pika_lvgl_point_t.h"
#include "TinyObj.h"
#include "pika_lvgl_roller.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_slider.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_style_t.h"
#include "TinyObj.h"
#include "pika_lvgl_switch.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_table.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_textarea.h"
#include "pika_lvgl_lv_obj.h"

void PikaDebug_DebugerMethod(PikaObj *self, Args *args){
    Arg* res = PikaDebug_Debuger(self);
    method_returnArg(args, res);
}

PikaObj *New_PikaDebug(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineConstructor(self, "Debuger()->any", PikaDebug_DebugerMethod);
    return self;
}

void PikaDebug_Debuger___init__Method(PikaObj *self, Args *args){
    PikaDebug_Debuger___init__(self);
}

void PikaDebug_Debuger_set_traceMethod(PikaObj *self, Args *args){
    PikaDebug_Debuger_set_trace(self);
}

PikaObj *New_PikaDebug_Debuger(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__init__()", PikaDebug_Debuger___init__Method);
    class_defineMethod(self, "set_trace()", PikaDebug_Debuger_set_traceMethod);
    return self;
}

Arg *PikaDebug_Debuger(PikaObj *self){
    return obj_newObjInPackage(New_PikaDebug_Debuger);
}
PikaObj *New_PikaMain(Args *args){
    PikaObj *self = New_PikaStdLib_SysObj(args);
    obj_newObj(self, "PikaStdLib", "PikaStdLib", New_PikaStdLib);
    return self;
}

Arg *PikaMain(PikaObj *self){
    return obj_newObjInPackage(New_PikaMain);
}
void PikaStdData_ByteArrayMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_ByteArray(self);
    method_returnArg(args, res);
}

void PikaStdData_DictMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Dict(self);
    method_returnArg(args, res);
}

void PikaStdData_FILEIOMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_FILEIO(self);
    method_returnArg(args, res);
}

void PikaStdData_ListMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_List(self);
    method_returnArg(args, res);
}

void PikaStdData_StringMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_String(self);
    method_returnArg(args, res);
}

void PikaStdData_TupleMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Tuple(self);
    method_returnArg(args, res);
}

void PikaStdData_UtilsMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Utils(self);
    method_returnArg(args, res);
}

void PikaStdData_dict_keysMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_dict_keys(self);
    method_returnArg(args, res);
}

PikaObj *New_PikaStdData(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineConstructor(self, "ByteArray()->any", PikaStdData_ByteArrayMethod);
    class_defineConstructor(self, "Dict()->any", PikaStdData_DictMethod);
    class_defineConstructor(self, "FILEIO()->any", PikaStdData_FILEIOMethod);
    class_defineConstructor(self, "List()->any", PikaStdData_ListMethod);
    class_defineConstructor(self, "String()->any", PikaStdData_StringMethod);
    class_defineConstructor(self, "Tuple()->any", PikaStdData_TupleMethod);
    class_defineConstructor(self, "Utils()->any", PikaStdData_UtilsMethod);
    class_defineConstructor(self, "dict_keys()->any", PikaStdData_dict_keysMethod);
    return self;
}

void PikaStdData_ByteArray___getitem__Method(PikaObj *self, Args *args){
    int __key = args_getInt(args, "__key");
    int res = PikaStdData_ByteArray___getitem__(self, __key);
    method_returnInt(args, res);
}

void PikaStdData_ByteArray___init__Method(PikaObj *self, Args *args){
    Arg* bytes = args_getArg(args, "bytes");
    PikaStdData_ByteArray___init__(self, bytes);
}

void PikaStdData_ByteArray___iter__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_ByteArray___iter__(self);
    method_returnArg(args, res);
}

void PikaStdData_ByteArray___next__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_ByteArray___next__(self);
    method_returnArg(args, res);
}

void PikaStdData_ByteArray___setitem__Method(PikaObj *self, Args *args){
    int __key = args_getInt(args, "__key");
    int __val = args_getInt(args, "__val");
    PikaStdData_ByteArray___setitem__(self, __key, __val);
}

void PikaStdData_ByteArray___str__Method(PikaObj *self, Args *args){
    char* res = PikaStdData_ByteArray___str__(self);
    method_returnStr(args, res);
}

void PikaStdData_ByteArray_decodeMethod(PikaObj *self, Args *args){
    char* res = PikaStdData_ByteArray_decode(self);
    method_returnStr(args, res);
}

PikaObj *New_PikaStdData_ByteArray(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__getitem__(__key:int)->int", PikaStdData_ByteArray___getitem__Method);
    class_defineMethod(self, "__init__(bytes:any)", PikaStdData_ByteArray___init__Method);
    class_defineMethod(self, "__iter__()->any", PikaStdData_ByteArray___iter__Method);
    class_defineMethod(self, "__next__()->any", PikaStdData_ByteArray___next__Method);
    class_defineMethod(self, "__setitem__(__key:int,__val:int)", PikaStdData_ByteArray___setitem__Method);
    class_defineMethod(self, "__str__()->str", PikaStdData_ByteArray___str__Method);
    class_defineMethod(self, "decode()->str", PikaStdData_ByteArray_decodeMethod);
    return self;
}

Arg *PikaStdData_ByteArray(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_ByteArray);
}
void PikaStdData_Dict___del__Method(PikaObj *self, Args *args){
    PikaStdData_Dict___del__(self);
}

void PikaStdData_Dict___getitem__Method(PikaObj *self, Args *args){
    Arg* __key = args_getArg(args, "__key");
    Arg* res = PikaStdData_Dict___getitem__(self, __key);
    method_returnArg(args, res);
}

void PikaStdData_Dict___init__Method(PikaObj *self, Args *args){
    PikaStdData_Dict___init__(self);
}

void PikaStdData_Dict___iter__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Dict___iter__(self);
    method_returnArg(args, res);
}

void PikaStdData_Dict___len__Method(PikaObj *self, Args *args){
    int res = PikaStdData_Dict___len__(self);
    method_returnInt(args, res);
}

void PikaStdData_Dict___next__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Dict___next__(self);
    method_returnArg(args, res);
}

void PikaStdData_Dict___setitem__Method(PikaObj *self, Args *args){
    Arg* __key = args_getArg(args, "__key");
    Arg* __val = args_getArg(args, "__val");
    PikaStdData_Dict___setitem__(self, __key, __val);
}

void PikaStdData_Dict___str__Method(PikaObj *self, Args *args){
    char* res = PikaStdData_Dict___str__(self);
    method_returnStr(args, res);
}

void PikaStdData_Dict_getMethod(PikaObj *self, Args *args){
    char* key = args_getStr(args, "key");
    Arg* res = PikaStdData_Dict_get(self, key);
    method_returnArg(args, res);
}

void PikaStdData_Dict_keysMethod(PikaObj *self, Args *args){
    PikaObj* res = PikaStdData_Dict_keys(self);
    method_returnObj(args, res);
}

void PikaStdData_Dict_removeMethod(PikaObj *self, Args *args){
    char* key = args_getStr(args, "key");
    PikaStdData_Dict_remove(self, key);
}

void PikaStdData_Dict_setMethod(PikaObj *self, Args *args){
    char* key = args_getStr(args, "key");
    Arg* arg = args_getArg(args, "arg");
    PikaStdData_Dict_set(self, key, arg);
}

PikaObj *New_PikaStdData_Dict(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__del__()", PikaStdData_Dict___del__Method);
    class_defineMethod(self, "__getitem__(__key:any)->any", PikaStdData_Dict___getitem__Method);
    class_defineMethod(self, "__init__()", PikaStdData_Dict___init__Method);
    class_defineMethod(self, "__iter__()->any", PikaStdData_Dict___iter__Method);
    class_defineMethod(self, "__len__()->int", PikaStdData_Dict___len__Method);
    class_defineMethod(self, "__next__()->any", PikaStdData_Dict___next__Method);
    class_defineMethod(self, "__setitem__(__key:any,__val:any)", PikaStdData_Dict___setitem__Method);
    class_defineMethod(self, "__str__()->str", PikaStdData_Dict___str__Method);
    class_defineMethod(self, "get(key:str)->any", PikaStdData_Dict_getMethod);
    class_defineMethod(self, "keys()->dict_keys", PikaStdData_Dict_keysMethod);
    class_defineMethod(self, "remove(key:str)", PikaStdData_Dict_removeMethod);
    class_defineMethod(self, "set(key:str,arg:any)", PikaStdData_Dict_setMethod);
    return self;
}

Arg *PikaStdData_Dict(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_Dict);
}
void PikaStdData_FILEIO_closeMethod(PikaObj *self, Args *args){
    PikaStdData_FILEIO_close(self);
}

void PikaStdData_FILEIO_initMethod(PikaObj *self, Args *args){
    char* path = args_getStr(args, "path");
    char* mode = args_getStr(args, "mode");
    int res = PikaStdData_FILEIO_init(self, path, mode);
    method_returnInt(args, res);
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
    PikaTuple* fromwhere = args_getTuple(args, "fromwhere");
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
    class_defineMethod(self, "init(path:str,mode:str)->int", PikaStdData_FILEIO_initMethod);
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
void PikaStdData_List___add__Method(PikaObj *self, Args *args){
    PikaObj* others = args_getPtr(args, "others");
    PikaObj* res = PikaStdData_List___add__(self, others);
    method_returnObj(args, res);
}

void PikaStdData_List___init__Method(PikaObj *self, Args *args){
    PikaStdData_List___init__(self);
}

void PikaStdData_List___setitem__Method(PikaObj *self, Args *args){
    Arg* __key = args_getArg(args, "__key");
    Arg* __val = args_getArg(args, "__val");
    PikaStdData_List___setitem__(self, __key, __val);
}

void PikaStdData_List___str__Method(PikaObj *self, Args *args){
    char* res = PikaStdData_List___str__(self);
    method_returnStr(args, res);
}

void PikaStdData_List_appendMethod(PikaObj *self, Args *args){
    Arg* arg = args_getArg(args, "arg");
    PikaStdData_List_append(self, arg);
}

void PikaStdData_List_reverseMethod(PikaObj *self, Args *args){
    PikaStdData_List_reverse(self);
}

void PikaStdData_List_setMethod(PikaObj *self, Args *args){
    int i = args_getInt(args, "i");
    Arg* arg = args_getArg(args, "arg");
    PikaStdData_List_set(self, i, arg);
}

PikaObj *New_PikaStdData_List(Args *args){
    PikaObj *self = New_PikaStdData_Tuple(args);
    class_defineMethod(self, "__add__(others:List)->List", PikaStdData_List___add__Method);
    class_defineMethod(self, "__init__()", PikaStdData_List___init__Method);
    class_defineMethod(self, "__setitem__(__key:any,__val:any)", PikaStdData_List___setitem__Method);
    class_defineMethod(self, "__str__()->str", PikaStdData_List___str__Method);
    class_defineMethod(self, "append(arg:any)", PikaStdData_List_appendMethod);
    class_defineMethod(self, "reverse()", PikaStdData_List_reverseMethod);
    class_defineMethod(self, "set(i:int,arg:any)", PikaStdData_List_setMethod);
    return self;
}

Arg *PikaStdData_List(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_List);
}
void PikaStdData_String___getitem__Method(PikaObj *self, Args *args){
    Arg* __key = args_getArg(args, "__key");
    Arg* res = PikaStdData_String___getitem__(self, __key);
    method_returnArg(args, res);
}

void PikaStdData_String___init__Method(PikaObj *self, Args *args){
    char* s = args_getStr(args, "s");
    PikaStdData_String___init__(self, s);
}

void PikaStdData_String___iter__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_String___iter__(self);
    method_returnArg(args, res);
}

void PikaStdData_String___len__Method(PikaObj *self, Args *args){
    int res = PikaStdData_String___len__(self);
    method_returnInt(args, res);
}

void PikaStdData_String___next__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_String___next__(self);
    method_returnArg(args, res);
}

void PikaStdData_String___setitem__Method(PikaObj *self, Args *args){
    Arg* __key = args_getArg(args, "__key");
    Arg* __val = args_getArg(args, "__val");
    PikaStdData_String___setitem__(self, __key, __val);
}

void PikaStdData_String___str__Method(PikaObj *self, Args *args){
    char* res = PikaStdData_String___str__(self);
    method_returnStr(args, res);
}

void PikaStdData_String_encodeMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdData_String_encode(self);
    method_returnArg(args, res);
}

void PikaStdData_String_endswithMethod(PikaObj *self, Args *args){
    char* suffix = args_getStr(args, "suffix");
    int res = PikaStdData_String_endswith(self, suffix);
    method_returnInt(args, res);
}

void PikaStdData_String_getMethod(PikaObj *self, Args *args){
    char* res = PikaStdData_String_get(self);
    method_returnStr(args, res);
}

void PikaStdData_String_isalnumMethod(PikaObj *self, Args *args){
    int res = PikaStdData_String_isalnum(self);
    method_returnInt(args, res);
}

void PikaStdData_String_isalphaMethod(PikaObj *self, Args *args){
    int res = PikaStdData_String_isalpha(self);
    method_returnInt(args, res);
}

void PikaStdData_String_isdigitMethod(PikaObj *self, Args *args){
    int res = PikaStdData_String_isdigit(self);
    method_returnInt(args, res);
}

void PikaStdData_String_islowerMethod(PikaObj *self, Args *args){
    int res = PikaStdData_String_islower(self);
    method_returnInt(args, res);
}

void PikaStdData_String_isspaceMethod(PikaObj *self, Args *args){
    int res = PikaStdData_String_isspace(self);
    method_returnInt(args, res);
}

void PikaStdData_String_replaceMethod(PikaObj *self, Args *args){
    char* old = args_getStr(args, "old");
    char* new = args_getStr(args, "new");
    char* res = PikaStdData_String_replace(self, old, new);
    method_returnStr(args, res);
}

void PikaStdData_String_setMethod(PikaObj *self, Args *args){
    char* s = args_getStr(args, "s");
    PikaStdData_String_set(self, s);
}

void PikaStdData_String_splitMethod(PikaObj *self, Args *args){
    char* s = args_getStr(args, "s");
    PikaObj* res = PikaStdData_String_split(self, s);
    method_returnObj(args, res);
}

void PikaStdData_String_startswithMethod(PikaObj *self, Args *args){
    char* prefix = args_getStr(args, "prefix");
    int res = PikaStdData_String_startswith(self, prefix);
    method_returnInt(args, res);
}

void PikaStdData_String_stripMethod(PikaObj *self, Args *args){
    char* res = PikaStdData_String_strip(self);
    method_returnStr(args, res);
}

PikaObj *New_PikaStdData_String(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__getitem__(__key:any)->any", PikaStdData_String___getitem__Method);
    class_defineMethod(self, "__init__(s:str)", PikaStdData_String___init__Method);
    class_defineMethod(self, "__iter__()->any", PikaStdData_String___iter__Method);
    class_defineMethod(self, "__len__()->int", PikaStdData_String___len__Method);
    class_defineMethod(self, "__next__()->any", PikaStdData_String___next__Method);
    class_defineMethod(self, "__setitem__(__key:any,__val:any)", PikaStdData_String___setitem__Method);
    class_defineMethod(self, "__str__()->str", PikaStdData_String___str__Method);
    class_defineMethod(self, "encode()->bytes", PikaStdData_String_encodeMethod);
    class_defineMethod(self, "endswith(suffix:str)->int", PikaStdData_String_endswithMethod);
    class_defineMethod(self, "get()->str", PikaStdData_String_getMethod);
    class_defineMethod(self, "isalnum()->int", PikaStdData_String_isalnumMethod);
    class_defineMethod(self, "isalpha()->int", PikaStdData_String_isalphaMethod);
    class_defineMethod(self, "isdigit()->int", PikaStdData_String_isdigitMethod);
    class_defineMethod(self, "islower()->int", PikaStdData_String_islowerMethod);
    class_defineMethod(self, "isspace()->int", PikaStdData_String_isspaceMethod);
    class_defineMethod(self, "replace(old:str,new:str)->str", PikaStdData_String_replaceMethod);
    class_defineMethod(self, "set(s:str)", PikaStdData_String_setMethod);
    class_defineMethod(self, "split(s:str)->List", PikaStdData_String_splitMethod);
    class_defineMethod(self, "startswith(prefix:str)->int", PikaStdData_String_startswithMethod);
    class_defineMethod(self, "strip()->str", PikaStdData_String_stripMethod);
    return self;
}

Arg *PikaStdData_String(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_String);
}
void PikaStdData_Tuple___del__Method(PikaObj *self, Args *args){
    PikaStdData_Tuple___del__(self);
}

void PikaStdData_Tuple___getitem__Method(PikaObj *self, Args *args){
    Arg* __key = args_getArg(args, "__key");
    Arg* res = PikaStdData_Tuple___getitem__(self, __key);
    method_returnArg(args, res);
}

void PikaStdData_Tuple___init__Method(PikaObj *self, Args *args){
    PikaStdData_Tuple___init__(self);
}

void PikaStdData_Tuple___iter__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Tuple___iter__(self);
    method_returnArg(args, res);
}

void PikaStdData_Tuple___len__Method(PikaObj *self, Args *args){
    int res = PikaStdData_Tuple___len__(self);
    method_returnInt(args, res);
}

void PikaStdData_Tuple___next__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdData_Tuple___next__(self);
    method_returnArg(args, res);
}

void PikaStdData_Tuple___str__Method(PikaObj *self, Args *args){
    char* res = PikaStdData_Tuple___str__(self);
    method_returnStr(args, res);
}

void PikaStdData_Tuple_getMethod(PikaObj *self, Args *args){
    int i = args_getInt(args, "i");
    Arg* res = PikaStdData_Tuple_get(self, i);
    method_returnArg(args, res);
}

void PikaStdData_Tuple_lenMethod(PikaObj *self, Args *args){
    int res = PikaStdData_Tuple_len(self);
    method_returnInt(args, res);
}

PikaObj *New_PikaStdData_Tuple(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__del__()", PikaStdData_Tuple___del__Method);
    class_defineMethod(self, "__getitem__(__key:any)->any", PikaStdData_Tuple___getitem__Method);
    class_defineMethod(self, "__init__()", PikaStdData_Tuple___init__Method);
    class_defineMethod(self, "__iter__()->any", PikaStdData_Tuple___iter__Method);
    class_defineMethod(self, "__len__()->int", PikaStdData_Tuple___len__Method);
    class_defineMethod(self, "__next__()->any", PikaStdData_Tuple___next__Method);
    class_defineMethod(self, "__str__()->str", PikaStdData_Tuple___str__Method);
    class_defineMethod(self, "get(i:int)->any", PikaStdData_Tuple_getMethod);
    class_defineMethod(self, "len()->int", PikaStdData_Tuple_lenMethod);
    return self;
}

Arg *PikaStdData_Tuple(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdData_Tuple);
}
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
void PikaStdLib_MemCheckerMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdLib_MemChecker(self);
    method_returnArg(args, res);
}

void PikaStdLib_RangeObjMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdLib_RangeObj(self);
    method_returnArg(args, res);
}

void PikaStdLib_StringObjMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdLib_StringObj(self);
    method_returnArg(args, res);
}

void PikaStdLib_SysObjMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdLib_SysObj(self);
    method_returnArg(args, res);
}

PikaObj *New_PikaStdLib(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineConstructor(self, "MemChecker()->any", PikaStdLib_MemCheckerMethod);
#if 0
    class_defineConstructor(self, "RangeObj()->any", PikaStdLib_RangeObjMethod);
#endif
#if 0
    class_defineConstructor(self, "StringObj()->any", PikaStdLib_StringObjMethod);
#endif
    class_defineConstructor(self, "SysObj()->any", PikaStdLib_SysObjMethod);
    return self;
}

void PikaStdLib_MemChecker_getMaxMethod(PikaObj *self, Args *args){
    double res = PikaStdLib_MemChecker_getMax(self);
    method_returnFloat(args, res);
}

void PikaStdLib_MemChecker_getNowMethod(PikaObj *self, Args *args){
    double res = PikaStdLib_MemChecker_getNow(self);
    method_returnFloat(args, res);
}

void PikaStdLib_MemChecker_maxMethod(PikaObj *self, Args *args){
    PikaStdLib_MemChecker_max(self);
}

void PikaStdLib_MemChecker_nowMethod(PikaObj *self, Args *args){
    PikaStdLib_MemChecker_now(self);
}

void PikaStdLib_MemChecker_resetMaxMethod(PikaObj *self, Args *args){
    PikaStdLib_MemChecker_resetMax(self);
}

PikaObj *New_PikaStdLib_MemChecker(Args *args){
    PikaObj *self = New_TinyObj(args);
#if !PIKA_NANO_ENABLE
    class_defineMethod(self, "getMax()->float", PikaStdLib_MemChecker_getMaxMethod);
#endif
#if !PIKA_NANO_ENABLE
    class_defineMethod(self, "getNow()->float", PikaStdLib_MemChecker_getNowMethod);
#endif
    class_defineMethod(self, "max()", PikaStdLib_MemChecker_maxMethod);
    class_defineMethod(self, "now()", PikaStdLib_MemChecker_nowMethod);
#if !PIKA_NANO_ENABLE
    class_defineMethod(self, "resetMax()", PikaStdLib_MemChecker_resetMaxMethod);
#endif
    return self;
}

Arg *PikaStdLib_MemChecker(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdLib_MemChecker);
}
void PikaStdLib_RangeObj___next__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdLib_RangeObj___next__(self);
    method_returnArg(args, res);
}

PikaObj *New_PikaStdLib_RangeObj(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__next__()->any", PikaStdLib_RangeObj___next__Method);
    return self;
}

Arg *PikaStdLib_RangeObj(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdLib_RangeObj);
}
void PikaStdLib_StringObj___next__Method(PikaObj *self, Args *args){
    Arg* res = PikaStdLib_StringObj___next__(self);
    method_returnArg(args, res);
}

PikaObj *New_PikaStdLib_StringObj(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__next__()->any", PikaStdLib_StringObj___next__Method);
    return self;
}

Arg *PikaStdLib_StringObj(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdLib_StringObj);
}
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
    PikaTuple* var = args_getTuple(args, "var");
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

void PikaStdLib_SysObj_dirMethod(PikaObj *self, Args *args){
    PikaObj* obj = args_getPtr(args, "obj");
    PikaObj* res = PikaStdLib_SysObj_dir(self, obj);
    method_returnObj(args, res);
}

void PikaStdLib_SysObj_execMethod(PikaObj *self, Args *args){
    char* code = args_getStr(args, "code");
    PikaStdLib_SysObj_exec(self, code);
}

void PikaStdLib_SysObj_floatMethod(PikaObj *self, Args *args){
    Arg* arg = args_getArg(args, "arg");
    double res = PikaStdLib_SysObj_float(self, arg);
    method_returnFloat(args, res);
}

void PikaStdLib_SysObj_getattrMethod(PikaObj *self, Args *args){
    PikaObj* obj = args_getPtr(args, "obj");
    char* name = args_getStr(args, "name");
    Arg* res = PikaStdLib_SysObj_getattr(self, obj, name);
    method_returnArg(args, res);
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
    PikaTuple* val = args_getTuple(args, "val");
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

void PikaStdLib_SysObj_setattrMethod(PikaObj *self, Args *args){
    PikaObj* obj = args_getPtr(args, "obj");
    char* name = args_getStr(args, "name");
    Arg* val = args_getArg(args, "val");
    PikaStdLib_SysObj_setattr(self, obj, name, val);
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
#if PIKA_BUILTIN_STRUCT_ENABLE
    class_defineMethod(self, "__slice__(obj:any,start:any,end:any,step:int)->any", PikaStdLib_SysObj___slice__Method);
#endif
#if !PIKA_NANO_ENABLE
    class_defineMethod(self, "bytes(val:any)->bytes", PikaStdLib_SysObj_bytesMethod);
#endif
#if PIKA_SYNTAX_FORMAT_ENABLE
    class_defineMethod(self, "cformat(fmt:str,*var)->str", PikaStdLib_SysObj_cformatMethod);
#endif
#if !PIKA_NANO_ENABLE
    class_defineMethod(self, "chr(val:int)->str", PikaStdLib_SysObj_chrMethod);
#endif
#if PIKA_BUILTIN_STRUCT_ENABLE
    class_defineMethod(self, "dict()->any", PikaStdLib_SysObj_dictMethod);
#endif
#if !PIKA_NANO_ENABLE
    class_defineMethod(self, "dir(obj:object)->list", PikaStdLib_SysObj_dirMethod);
#endif
#if PIKA_EXEC_ENABLE
    class_defineMethod(self, "exec(code:str)", PikaStdLib_SysObj_execMethod);
#endif
    class_defineMethod(self, "float(arg:any)->float", PikaStdLib_SysObj_floatMethod);
#if !PIKA_NANO_ENABLE
    class_defineMethod(self, "getattr(obj:object,name:str)->any", PikaStdLib_SysObj_getattrMethod);
#endif
#if !PIKA_NANO_ENABLE
    class_defineMethod(self, "hex(val:int)->str", PikaStdLib_SysObj_hexMethod);
#endif
#if !PIKA_NANO_ENABLE
    class_defineMethod(self, "id(obj:any)->int", PikaStdLib_SysObj_idMethod);
#endif
    class_defineMethod(self, "int(arg:any)->int", PikaStdLib_SysObj_intMethod);
    class_defineMethod(self, "iter(arg:any)->any", PikaStdLib_SysObj_iterMethod);
    class_defineMethod(self, "len(arg:any)->int", PikaStdLib_SysObj_lenMethod);
#if PIKA_BUILTIN_STRUCT_ENABLE
    class_defineMethod(self, "list()->any", PikaStdLib_SysObj_listMethod);
#endif
#if PIKA_FILEIO_ENABLE
    class_defineMethod(self, "open(path:str,mode:str)->object", PikaStdLib_SysObj_openMethod);
#endif
#if !PIKA_NANO_ENABLE
    class_defineMethod(self, "ord(val:str)->int", PikaStdLib_SysObj_ordMethod);
#endif
    class_defineMethod(self, "print(*val)", PikaStdLib_SysObj_printMethod);
#if !PIKA_NANO_ENABLE
    class_defineMethod(self, "printNoEnd(val:any)", PikaStdLib_SysObj_printNoEndMethod);
#endif
    class_defineMethod(self, "range(a1:int,a2:int)->any", PikaStdLib_SysObj_rangeMethod);
#if !PIKA_NANO_ENABLE
    class_defineMethod(self, "setattr(obj:object,name:str,val:any)", PikaStdLib_SysObj_setattrMethod);
#endif
    class_defineMethod(self, "str(arg:any)->str", PikaStdLib_SysObj_strMethod);
    class_defineMethod(self, "type(arg:any)->any", PikaStdLib_SysObj_typeMethod);
    return self;
}

Arg *PikaStdLib_SysObj(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdLib_SysObj);
}
void PikaStdTask_TaskMethod(PikaObj *self, Args *args){
    Arg* res = PikaStdTask_Task(self);
    method_returnArg(args, res);
}

PikaObj *New_PikaStdTask(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineConstructor(self, "Task()->any", PikaStdTask_TaskMethod);
    return self;
}

void PikaStdTask_Task___init__Method(PikaObj *self, Args *args){
    PikaStdTask_Task___init__(self);
}

void PikaStdTask_Task_call_alwaysMethod(PikaObj *self, Args *args){
    Arg* fun_todo = args_getArg(args, "fun_todo");
    PikaStdTask_Task_call_always(self, fun_todo);
}

void PikaStdTask_Task_call_period_msMethod(PikaObj *self, Args *args){
    Arg* fun_todo = args_getArg(args, "fun_todo");
    int period_ms = args_getInt(args, "period_ms");
    PikaStdTask_Task_call_period_ms(self, fun_todo, period_ms);
}

void PikaStdTask_Task_call_whenMethod(PikaObj *self, Args *args){
    Arg* fun_todo = args_getArg(args, "fun_todo");
    Arg* fun_when = args_getArg(args, "fun_when");
    PikaStdTask_Task_call_when(self, fun_todo, fun_when);
}

void PikaStdTask_Task_platformGetTickMethod(PikaObj *self, Args *args){
    PikaStdTask_Task_platformGetTick(self);
}

void PikaStdTask_Task_run_foreverMethod(PikaObj *self, Args *args){
    PikaStdTask_Task_run_forever(self);
}

void PikaStdTask_Task_run_onceMethod(PikaObj *self, Args *args){
    PikaStdTask_Task_run_once(self);
}

void PikaStdTask_Task_run_until_msMethod(PikaObj *self, Args *args){
    int until_ms = args_getInt(args, "until_ms");
    PikaStdTask_Task_run_until_ms(self, until_ms);
}

PikaObj *New_PikaStdTask_Task(Args *args){
    PikaObj *self = New_PikaStdLib_SysObj(args);
    obj_newObj(self, "calls", "PikaStdData_List", New_PikaStdData_List);
    class_defineMethod(self, "__init__()", PikaStdTask_Task___init__Method);
    class_defineMethod(self, "call_always(fun_todo:any)", PikaStdTask_Task_call_alwaysMethod);
    class_defineMethod(self, "call_period_ms(fun_todo:any,period_ms:int)", PikaStdTask_Task_call_period_msMethod);
    class_defineMethod(self, "call_when(fun_todo:any,fun_when:any)", PikaStdTask_Task_call_whenMethod);
    class_defineMethod(self, "platformGetTick()", PikaStdTask_Task_platformGetTickMethod);
    class_defineMethod(self, "run_forever()", PikaStdTask_Task_run_foreverMethod);
    class_defineMethod(self, "run_once()", PikaStdTask_Task_run_onceMethod);
    class_defineMethod(self, "run_until_ms(until_ms:int)", PikaStdTask_Task_run_until_msMethod);
    return self;
}

Arg *PikaStdTask_Task(PikaObj *self){
    return obj_newObjInPackage(New_PikaStdTask_Task);
}
void pika_lvgl_ALIGNMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_ALIGN(self);
    method_returnArg(args, res);
}

void pika_lvgl_ANIMMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_ANIM(self);
    method_returnArg(args, res);
}

void pika_lvgl_EVENTMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_EVENT(self);
    method_returnArg(args, res);
}

void pika_lvgl_OPAMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_OPA(self);
    method_returnArg(args, res);
}

void pika_lvgl_PALETTEMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_PALETTE(self);
    method_returnArg(args, res);
}

void pika_lvgl_STATEMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_STATE(self);
    method_returnArg(args, res);
}

void pika_lvgl___init__Method(PikaObj *self, Args *args){
    pika_lvgl___init__(self);
}

void pika_lvgl_arcMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_arc(self);
    method_returnArg(args, res);
}

void pika_lvgl_barMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_bar(self);
    method_returnArg(args, res);
}

void pika_lvgl_btnMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_btn(self);
    method_returnArg(args, res);
}

void pika_lvgl_checkboxMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_checkbox(self);
    method_returnArg(args, res);
}

void pika_lvgl_dropdownMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_dropdown(self);
    method_returnArg(args, res);
}

void pika_lvgl_indev_get_actMethod(PikaObj *self, Args *args){
    PikaObj* res = pika_lvgl_indev_get_act(self);
    method_returnObj(args, res);
}

void pika_lvgl_indev_tMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_indev_t(self);
    method_returnArg(args, res);
}

void pika_lvgl_labelMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_label(self);
    method_returnArg(args, res);
}

void pika_lvgl_lv_color_tMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_lv_color_t(self);
    method_returnArg(args, res);
}

void pika_lvgl_lv_eventMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_lv_event(self);
    method_returnArg(args, res);
}

void pika_lvgl_lv_objMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_lv_obj(self);
    method_returnArg(args, res);
}

void pika_lvgl_lv_timer_tMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_lv_timer_t(self);
    method_returnArg(args, res);
}

void pika_lvgl_objMethod(PikaObj *self, Args *args){
    PikaObj* parent = args_getPtr(args, "parent");
    PikaObj* res = pika_lvgl_obj(self, parent);
    method_returnObj(args, res);
}

void pika_lvgl_palette_lightenMethod(PikaObj *self, Args *args){
    int p = args_getInt(args, "p");
    int lvl = args_getInt(args, "lvl");
    PikaObj* res = pika_lvgl_palette_lighten(self, p, lvl);
    method_returnObj(args, res);
}

void pika_lvgl_palette_mainMethod(PikaObj *self, Args *args){
    int p = args_getInt(args, "p");
    PikaObj* res = pika_lvgl_palette_main(self, p);
    method_returnObj(args, res);
}

void pika_lvgl_point_tMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_point_t(self);
    method_returnArg(args, res);
}

void pika_lvgl_rollerMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_roller(self);
    method_returnArg(args, res);
}

void pika_lvgl_scr_actMethod(PikaObj *self, Args *args){
    PikaObj* res = pika_lvgl_scr_act(self);
    method_returnObj(args, res);
}

void pika_lvgl_sliderMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_slider(self);
    method_returnArg(args, res);
}

void pika_lvgl_style_tMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_style_t(self);
    method_returnArg(args, res);
}

void pika_lvgl_switchMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_switch(self);
    method_returnArg(args, res);
}

void pika_lvgl_tableMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_table(self);
    method_returnArg(args, res);
}

void pika_lvgl_textareaMethod(PikaObj *self, Args *args){
    Arg* res = pika_lvgl_textarea(self);
    method_returnArg(args, res);
}

void pika_lvgl_timer_create_basicMethod(PikaObj *self, Args *args){
    PikaObj* res = pika_lvgl_timer_create_basic(self);
    method_returnObj(args, res);
}

PikaObj *New_pika_lvgl(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineConstructor(self, "ALIGN()->any", pika_lvgl_ALIGNMethod);
    class_defineConstructor(self, "ANIM()->any", pika_lvgl_ANIMMethod);
    class_defineConstructor(self, "EVENT()->any", pika_lvgl_EVENTMethod);
    class_defineConstructor(self, "OPA()->any", pika_lvgl_OPAMethod);
    class_defineConstructor(self, "PALETTE()->any", pika_lvgl_PALETTEMethod);
    class_defineConstructor(self, "STATE()->any", pika_lvgl_STATEMethod);
    class_defineMethod(self, "__init__()", pika_lvgl___init__Method);
    class_defineConstructor(self, "arc()->any", pika_lvgl_arcMethod);
    class_defineConstructor(self, "bar()->any", pika_lvgl_barMethod);
    class_defineConstructor(self, "btn()->any", pika_lvgl_btnMethod);
    class_defineConstructor(self, "checkbox()->any", pika_lvgl_checkboxMethod);
    class_defineConstructor(self, "dropdown()->any", pika_lvgl_dropdownMethod);
    class_defineMethod(self, "indev_get_act()->indev_t", pika_lvgl_indev_get_actMethod);
    class_defineConstructor(self, "indev_t()->any", pika_lvgl_indev_tMethod);
    class_defineConstructor(self, "label()->any", pika_lvgl_labelMethod);
    class_defineConstructor(self, "lv_color_t()->any", pika_lvgl_lv_color_tMethod);
    class_defineConstructor(self, "lv_event()->any", pika_lvgl_lv_eventMethod);
    class_defineConstructor(self, "lv_obj()->any", pika_lvgl_lv_objMethod);
    class_defineConstructor(self, "lv_timer_t()->any", pika_lvgl_lv_timer_tMethod);
    class_defineMethod(self, "obj(parent:lv_obj)->lv_obj", pika_lvgl_objMethod);
    class_defineMethod(self, "palette_lighten(p:int,lvl:int)->lv_color_t", pika_lvgl_palette_lightenMethod);
    class_defineMethod(self, "palette_main(p:int)->lv_color_t", pika_lvgl_palette_mainMethod);
    class_defineConstructor(self, "point_t()->any", pika_lvgl_point_tMethod);
    class_defineConstructor(self, "roller()->any", pika_lvgl_rollerMethod);
    class_defineMethod(self, "scr_act()->lv_obj", pika_lvgl_scr_actMethod);
    class_defineConstructor(self, "slider()->any", pika_lvgl_sliderMethod);
    class_defineConstructor(self, "style_t()->any", pika_lvgl_style_tMethod);
    class_defineConstructor(self, "switch()->any", pika_lvgl_switchMethod);
    class_defineConstructor(self, "table()->any", pika_lvgl_tableMethod);
    class_defineConstructor(self, "textarea()->any", pika_lvgl_textareaMethod);
    class_defineMethod(self, "timer_create_basic()->lv_timer_t", pika_lvgl_timer_create_basicMethod);
    return self;
}

void pika_lvgl_ALIGN___init__Method(PikaObj *self, Args *args){
    pika_lvgl_ALIGN___init__(self);
}

PikaObj *New_pika_lvgl_ALIGN(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__init__()", pika_lvgl_ALIGN___init__Method);
    return self;
}

Arg *pika_lvgl_ALIGN(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_ALIGN);
}
void pika_lvgl_ANIM___init__Method(PikaObj *self, Args *args){
    pika_lvgl_ANIM___init__(self);
}

PikaObj *New_pika_lvgl_ANIM(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__init__()", pika_lvgl_ANIM___init__Method);
    return self;
}

Arg *pika_lvgl_ANIM(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_ANIM);
}
void pika_lvgl_EVENT___init__Method(PikaObj *self, Args *args){
    pika_lvgl_EVENT___init__(self);
}

PikaObj *New_pika_lvgl_EVENT(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__init__()", pika_lvgl_EVENT___init__Method);
    return self;
}

Arg *pika_lvgl_EVENT(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_EVENT);
}
void pika_lvgl_OPA___init__Method(PikaObj *self, Args *args){
    pika_lvgl_OPA___init__(self);
}

PikaObj *New_pika_lvgl_OPA(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__init__()", pika_lvgl_OPA___init__Method);
    return self;
}

Arg *pika_lvgl_OPA(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_OPA);
}
void pika_lvgl_PALETTE___init__Method(PikaObj *self, Args *args){
    pika_lvgl_PALETTE___init__(self);
}

PikaObj *New_pika_lvgl_PALETTE(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__init__()", pika_lvgl_PALETTE___init__Method);
    return self;
}

Arg *pika_lvgl_PALETTE(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_PALETTE);
}
void pika_lvgl_STATE___init__Method(PikaObj *self, Args *args){
    pika_lvgl_STATE___init__(self);
}

PikaObj *New_pika_lvgl_STATE(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__init__()", pika_lvgl_STATE___init__Method);
    return self;
}

Arg *pika_lvgl_STATE(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_STATE);
}
void pika_lvgl_arc___init__Method(PikaObj *self, Args *args){
    PikaObj* parent = args_getPtr(args, "parent");
    pika_lvgl_arc___init__(self, parent);
}

void pika_lvgl_arc_get_angle_endMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_arc_get_angle_end(self);
    method_returnInt(args, res);
}

void pika_lvgl_arc_get_angle_startMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_arc_get_angle_start(self);
    method_returnInt(args, res);
}

void pika_lvgl_arc_get_bg_angle_endMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_arc_get_bg_angle_end(self);
    method_returnInt(args, res);
}

void pika_lvgl_arc_get_bg_angle_startMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_arc_get_bg_angle_start(self);
    method_returnInt(args, res);
}

void pika_lvgl_arc_get_max_valueMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_arc_get_max_value(self);
    method_returnInt(args, res);
}

void pika_lvgl_arc_get_min_valueMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_arc_get_min_value(self);
    method_returnInt(args, res);
}

void pika_lvgl_arc_get_modeMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_arc_get_mode(self);
    method_returnInt(args, res);
}

void pika_lvgl_arc_get_valueMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_arc_get_value(self);
    method_returnInt(args, res);
}

void pika_lvgl_arc_set_anglesMethod(PikaObj *self, Args *args){
    int start = args_getInt(args, "start");
    int end = args_getInt(args, "end");
    pika_lvgl_arc_set_angles(self, start, end);
}

void pika_lvgl_arc_set_bg_anglesMethod(PikaObj *self, Args *args){
    int start = args_getInt(args, "start");
    int end = args_getInt(args, "end");
    pika_lvgl_arc_set_bg_angles(self, start, end);
}

void pika_lvgl_arc_set_bg_end_angleMethod(PikaObj *self, Args *args){
    int angle = args_getInt(args, "angle");
    pika_lvgl_arc_set_bg_end_angle(self, angle);
}

void pika_lvgl_arc_set_bg_start_angleMethod(PikaObj *self, Args *args){
    int start = args_getInt(args, "start");
    pika_lvgl_arc_set_bg_start_angle(self, start);
}

void pika_lvgl_arc_set_change_rateMethod(PikaObj *self, Args *args){
    int rate = args_getInt(args, "rate");
    pika_lvgl_arc_set_change_rate(self, rate);
}

void pika_lvgl_arc_set_end_angleMethod(PikaObj *self, Args *args){
    int angle = args_getInt(args, "angle");
    pika_lvgl_arc_set_end_angle(self, angle);
}

void pika_lvgl_arc_set_modeMethod(PikaObj *self, Args *args){
    int mode = args_getInt(args, "mode");
    pika_lvgl_arc_set_mode(self, mode);
}

void pika_lvgl_arc_set_rangeMethod(PikaObj *self, Args *args){
    int min = args_getInt(args, "min");
    int max = args_getInt(args, "max");
    pika_lvgl_arc_set_range(self, min, max);
}

void pika_lvgl_arc_set_rotationMethod(PikaObj *self, Args *args){
    int rotation = args_getInt(args, "rotation");
    pika_lvgl_arc_set_rotation(self, rotation);
}

void pika_lvgl_arc_set_start_angleMethod(PikaObj *self, Args *args){
    int start = args_getInt(args, "start");
    pika_lvgl_arc_set_start_angle(self, start);
}

void pika_lvgl_arc_set_valueMethod(PikaObj *self, Args *args){
    int value = args_getInt(args, "value");
    pika_lvgl_arc_set_value(self, value);
}

PikaObj *New_pika_lvgl_arc(Args *args){
    PikaObj *self = New_pika_lvgl_lv_obj(args);
    class_defineMethod(self, "__init__(parent:lv_obj)", pika_lvgl_arc___init__Method);
    class_defineMethod(self, "get_angle_end()->int", pika_lvgl_arc_get_angle_endMethod);
    class_defineMethod(self, "get_angle_start()->int", pika_lvgl_arc_get_angle_startMethod);
    class_defineMethod(self, "get_bg_angle_end()->int", pika_lvgl_arc_get_bg_angle_endMethod);
    class_defineMethod(self, "get_bg_angle_start()->int", pika_lvgl_arc_get_bg_angle_startMethod);
    class_defineMethod(self, "get_max_value()->int", pika_lvgl_arc_get_max_valueMethod);
    class_defineMethod(self, "get_min_value()->int", pika_lvgl_arc_get_min_valueMethod);
    class_defineMethod(self, "get_mode()->int", pika_lvgl_arc_get_modeMethod);
    class_defineMethod(self, "get_value()->int", pika_lvgl_arc_get_valueMethod);
    class_defineMethod(self, "set_angles(start:int,end:int)", pika_lvgl_arc_set_anglesMethod);
    class_defineMethod(self, "set_bg_angles(start:int,end:int)", pika_lvgl_arc_set_bg_anglesMethod);
    class_defineMethod(self, "set_bg_end_angle(angle:int)", pika_lvgl_arc_set_bg_end_angleMethod);
    class_defineMethod(self, "set_bg_start_angle(start:int)", pika_lvgl_arc_set_bg_start_angleMethod);
    class_defineMethod(self, "set_change_rate(rate:int)", pika_lvgl_arc_set_change_rateMethod);
    class_defineMethod(self, "set_end_angle(angle:int)", pika_lvgl_arc_set_end_angleMethod);
    class_defineMethod(self, "set_mode(mode:int)", pika_lvgl_arc_set_modeMethod);
    class_defineMethod(self, "set_range(min:int,max:int)", pika_lvgl_arc_set_rangeMethod);
    class_defineMethod(self, "set_rotation(rotation:int)", pika_lvgl_arc_set_rotationMethod);
    class_defineMethod(self, "set_start_angle(start:int)", pika_lvgl_arc_set_start_angleMethod);
    class_defineMethod(self, "set_value(value:int)", pika_lvgl_arc_set_valueMethod);
    return self;
}

Arg *pika_lvgl_arc(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_arc);
}
void pika_lvgl_bar___init__Method(PikaObj *self, Args *args){
    PikaObj* parent = args_getPtr(args, "parent");
    pika_lvgl_bar___init__(self, parent);
}

void pika_lvgl_bar_get_max_valueMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_bar_get_max_value(self);
    method_returnInt(args, res);
}

void pika_lvgl_bar_get_min_valueMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_bar_get_min_value(self);
    method_returnInt(args, res);
}

void pika_lvgl_bar_get_modeMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_bar_get_mode(self);
    method_returnInt(args, res);
}

void pika_lvgl_bar_get_start_valueMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_bar_get_start_value(self);
    method_returnInt(args, res);
}

void pika_lvgl_bar_get_valueMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_bar_get_value(self);
    method_returnInt(args, res);
}

void pika_lvgl_bar_set_modeMethod(PikaObj *self, Args *args){
    int mode = args_getInt(args, "mode");
    pika_lvgl_bar_set_mode(self, mode);
}

void pika_lvgl_bar_set_rangeMethod(PikaObj *self, Args *args){
    int min = args_getInt(args, "min");
    int max = args_getInt(args, "max");
    pika_lvgl_bar_set_range(self, min, max);
}

void pika_lvgl_bar_set_start_valueMethod(PikaObj *self, Args *args){
    int start_value = args_getInt(args, "start_value");
    int anim = args_getInt(args, "anim");
    pika_lvgl_bar_set_start_value(self, start_value, anim);
}

void pika_lvgl_bar_set_valueMethod(PikaObj *self, Args *args){
    int value = args_getInt(args, "value");
    int anim = args_getInt(args, "anim");
    pika_lvgl_bar_set_value(self, value, anim);
}

PikaObj *New_pika_lvgl_bar(Args *args){
    PikaObj *self = New_pika_lvgl_lv_obj(args);
    class_defineMethod(self, "__init__(parent:lv_obj)", pika_lvgl_bar___init__Method);
    class_defineMethod(self, "get_max_value()->int", pika_lvgl_bar_get_max_valueMethod);
    class_defineMethod(self, "get_min_value()->int", pika_lvgl_bar_get_min_valueMethod);
    class_defineMethod(self, "get_mode()->int", pika_lvgl_bar_get_modeMethod);
    class_defineMethod(self, "get_start_value()->int", pika_lvgl_bar_get_start_valueMethod);
    class_defineMethod(self, "get_value()->int", pika_lvgl_bar_get_valueMethod);
    class_defineMethod(self, "set_mode(mode:int)", pika_lvgl_bar_set_modeMethod);
    class_defineMethod(self, "set_range(min:int,max:int)", pika_lvgl_bar_set_rangeMethod);
    class_defineMethod(self, "set_start_value(start_value:int,anim:int)", pika_lvgl_bar_set_start_valueMethod);
    class_defineMethod(self, "set_value(value:int,anim:int)", pika_lvgl_bar_set_valueMethod);
    return self;
}

Arg *pika_lvgl_bar(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_bar);
}
void pika_lvgl_btn___init__Method(PikaObj *self, Args *args){
    PikaObj* parent = args_getPtr(args, "parent");
    pika_lvgl_btn___init__(self, parent);
}

PikaObj *New_pika_lvgl_btn(Args *args){
    PikaObj *self = New_pika_lvgl_lv_obj(args);
    class_defineMethod(self, "__init__(parent:lv_obj)", pika_lvgl_btn___init__Method);
    return self;
}

Arg *pika_lvgl_btn(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_btn);
}
void pika_lvgl_checkbox___init__Method(PikaObj *self, Args *args){
    PikaObj* parent = args_getPtr(args, "parent");
    pika_lvgl_checkbox___init__(self, parent);
}

void pika_lvgl_checkbox_get_textMethod(PikaObj *self, Args *args){
    char* res = pika_lvgl_checkbox_get_text(self);
    method_returnStr(args, res);
}

void pika_lvgl_checkbox_set_textMethod(PikaObj *self, Args *args){
    char* txt = args_getStr(args, "txt");
    pika_lvgl_checkbox_set_text(self, txt);
}

void pika_lvgl_checkbox_set_text_staticMethod(PikaObj *self, Args *args){
    char* txt = args_getStr(args, "txt");
    pika_lvgl_checkbox_set_text_static(self, txt);
}

PikaObj *New_pika_lvgl_checkbox(Args *args){
    PikaObj *self = New_pika_lvgl_lv_obj(args);
    class_defineMethod(self, "__init__(parent:lv_obj)", pika_lvgl_checkbox___init__Method);
    class_defineMethod(self, "get_text()->str", pika_lvgl_checkbox_get_textMethod);
    class_defineMethod(self, "set_text(txt:str)", pika_lvgl_checkbox_set_textMethod);
    class_defineMethod(self, "set_text_static(txt:str)", pika_lvgl_checkbox_set_text_staticMethod);
    return self;
}

Arg *pika_lvgl_checkbox(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_checkbox);
}
void pika_lvgl_dropdown___init__Method(PikaObj *self, Args *args){
    PikaObj* parent = args_getPtr(args, "parent");
    pika_lvgl_dropdown___init__(self, parent);
}

void pika_lvgl_dropdown_add_optionMethod(PikaObj *self, Args *args){
    char* option = args_getStr(args, "option");
    int pos = args_getInt(args, "pos");
    pika_lvgl_dropdown_add_option(self, option, pos);
}

void pika_lvgl_dropdown_clear_optionsMethod(PikaObj *self, Args *args){
    pika_lvgl_dropdown_clear_options(self);
}

void pika_lvgl_dropdown_closeMethod(PikaObj *self, Args *args){
    pika_lvgl_dropdown_close(self);
}

void pika_lvgl_dropdown_get_dirMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_dropdown_get_dir(self);
    method_returnInt(args, res);
}

void pika_lvgl_dropdown_get_option_cntMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_dropdown_get_option_cnt(self);
    method_returnInt(args, res);
}

void pika_lvgl_dropdown_get_option_indexMethod(PikaObj *self, Args *args){
    char* option = args_getStr(args, "option");
    int res = pika_lvgl_dropdown_get_option_index(self, option);
    method_returnInt(args, res);
}

void pika_lvgl_dropdown_get_optionsMethod(PikaObj *self, Args *args){
    char* res = pika_lvgl_dropdown_get_options(self);
    method_returnStr(args, res);
}

void pika_lvgl_dropdown_get_selectedMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_dropdown_get_selected(self);
    method_returnInt(args, res);
}

void pika_lvgl_dropdown_get_selected_highlightMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_dropdown_get_selected_highlight(self);
    method_returnInt(args, res);
}

void pika_lvgl_dropdown_get_selected_strMethod(PikaObj *self, Args *args){
    char* res = pika_lvgl_dropdown_get_selected_str(self);
    method_returnStr(args, res);
}

void pika_lvgl_dropdown_get_symbolMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_dropdown_get_symbol(self);
    method_returnInt(args, res);
}

void pika_lvgl_dropdown_get_textMethod(PikaObj *self, Args *args){
    char* res = pika_lvgl_dropdown_get_text(self);
    method_returnStr(args, res);
}

void pika_lvgl_dropdown_is_openMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_dropdown_is_open(self);
    method_returnInt(args, res);
}

void pika_lvgl_dropdown_openMethod(PikaObj *self, Args *args){
    pika_lvgl_dropdown_open(self);
}

void pika_lvgl_dropdown_set_dirMethod(PikaObj *self, Args *args){
    int dir = args_getInt(args, "dir");
    pika_lvgl_dropdown_set_dir(self, dir);
}

void pika_lvgl_dropdown_set_optionsMethod(PikaObj *self, Args *args){
    char* options = args_getStr(args, "options");
    pika_lvgl_dropdown_set_options(self, options);
}

void pika_lvgl_dropdown_set_selectedMethod(PikaObj *self, Args *args){
    int sel_opt = args_getInt(args, "sel_opt");
    pika_lvgl_dropdown_set_selected(self, sel_opt);
}

void pika_lvgl_dropdown_set_selected_hightlightMethod(PikaObj *self, Args *args){
    int en = args_getInt(args, "en");
    pika_lvgl_dropdown_set_selected_hightlight(self, en);
}

void pika_lvgl_dropdown_set_symbolMethod(PikaObj *self, Args *args){
    char* symbol = args_getStr(args, "symbol");
    pika_lvgl_dropdown_set_symbol(self, symbol);
}

void pika_lvgl_dropdown_set_textMethod(PikaObj *self, Args *args){
    char* txt = args_getStr(args, "txt");
    pika_lvgl_dropdown_set_text(self, txt);
}

PikaObj *New_pika_lvgl_dropdown(Args *args){
    PikaObj *self = New_pika_lvgl_lv_obj(args);
    class_defineMethod(self, "__init__(parent:lv_obj)", pika_lvgl_dropdown___init__Method);
    class_defineMethod(self, "add_option(option:str,pos:int)", pika_lvgl_dropdown_add_optionMethod);
    class_defineMethod(self, "clear_options()", pika_lvgl_dropdown_clear_optionsMethod);
    class_defineMethod(self, "close()", pika_lvgl_dropdown_closeMethod);
    class_defineMethod(self, "get_dir()->int", pika_lvgl_dropdown_get_dirMethod);
    class_defineMethod(self, "get_option_cnt()->int", pika_lvgl_dropdown_get_option_cntMethod);
    class_defineMethod(self, "get_option_index(option:str)->int", pika_lvgl_dropdown_get_option_indexMethod);
    class_defineMethod(self, "get_options()->str", pika_lvgl_dropdown_get_optionsMethod);
    class_defineMethod(self, "get_selected()->int", pika_lvgl_dropdown_get_selectedMethod);
    class_defineMethod(self, "get_selected_highlight()->int", pika_lvgl_dropdown_get_selected_highlightMethod);
    class_defineMethod(self, "get_selected_str()->str", pika_lvgl_dropdown_get_selected_strMethod);
    class_defineMethod(self, "get_symbol()->int", pika_lvgl_dropdown_get_symbolMethod);
    class_defineMethod(self, "get_text()->str", pika_lvgl_dropdown_get_textMethod);
    class_defineMethod(self, "is_open()->int", pika_lvgl_dropdown_is_openMethod);
    class_defineMethod(self, "open()", pika_lvgl_dropdown_openMethod);
    class_defineMethod(self, "set_dir(dir:int)", pika_lvgl_dropdown_set_dirMethod);
    class_defineMethod(self, "set_options(options:str)", pika_lvgl_dropdown_set_optionsMethod);
    class_defineMethod(self, "set_selected(sel_opt:int)", pika_lvgl_dropdown_set_selectedMethod);
    class_defineMethod(self, "set_selected_hightlight(en:int)", pika_lvgl_dropdown_set_selected_hightlightMethod);
    class_defineMethod(self, "set_symbol(symbol:str)", pika_lvgl_dropdown_set_symbolMethod);
    class_defineMethod(self, "set_text(txt:str)", pika_lvgl_dropdown_set_textMethod);
    return self;
}

Arg *pika_lvgl_dropdown(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_dropdown);
}
void pika_lvgl_indev_t_get_vectMethod(PikaObj *self, Args *args){
    PikaObj* point = args_getPtr(args, "point");
    pika_lvgl_indev_t_get_vect(self, point);
}

PikaObj *New_pika_lvgl_indev_t(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "get_vect(point:point_t)", pika_lvgl_indev_t_get_vectMethod);
    return self;
}

Arg *pika_lvgl_indev_t(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_indev_t);
}
void pika_lvgl_label___init__Method(PikaObj *self, Args *args){
    PikaObj* parent = args_getPtr(args, "parent");
    pika_lvgl_label___init__(self, parent);
}

void pika_lvgl_label_set_long_modeMethod(PikaObj *self, Args *args){
    int mode = args_getInt(args, "mode");
    pika_lvgl_label_set_long_mode(self, mode);
}

void pika_lvgl_label_set_recolorMethod(PikaObj *self, Args *args){
    int en = args_getInt(args, "en");
    pika_lvgl_label_set_recolor(self, en);
}

void pika_lvgl_label_set_style_text_alignMethod(PikaObj *self, Args *args){
    int value = args_getInt(args, "value");
    int selector = args_getInt(args, "selector");
    pika_lvgl_label_set_style_text_align(self, value, selector);
}

void pika_lvgl_label_set_textMethod(PikaObj *self, Args *args){
    char* txt = args_getStr(args, "txt");
    pika_lvgl_label_set_text(self, txt);
}

PikaObj *New_pika_lvgl_label(Args *args){
    PikaObj *self = New_pika_lvgl_lv_obj(args);
    class_defineMethod(self, "__init__(parent:lv_obj)", pika_lvgl_label___init__Method);
    class_defineMethod(self, "set_long_mode(mode:int)", pika_lvgl_label_set_long_modeMethod);
    class_defineMethod(self, "set_recolor(en:int)", pika_lvgl_label_set_recolorMethod);
    class_defineMethod(self, "set_style_text_align(value:int,selector:int)", pika_lvgl_label_set_style_text_alignMethod);
    class_defineMethod(self, "set_text(txt:str)", pika_lvgl_label_set_textMethod);
    return self;
}

Arg *pika_lvgl_label(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_label);
}
PikaObj *New_pika_lvgl_lv_color_t(Args *args){
    PikaObj *self = New_TinyObj(args);
    return self;
}

Arg *pika_lvgl_lv_color_t(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_lv_color_t);
}
void pika_lvgl_lv_event_get_codeMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_lv_event_get_code(self);
    method_returnInt(args, res);
}

void pika_lvgl_lv_event_get_targetMethod(PikaObj *self, Args *args){
    PikaObj* res = pika_lvgl_lv_event_get_target(self);
    method_returnObj(args, res);
}

PikaObj *New_pika_lvgl_lv_event(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "get_code()->int", pika_lvgl_lv_event_get_codeMethod);
    class_defineMethod(self, "get_target()->lv_obj", pika_lvgl_lv_event_get_targetMethod);
    return self;
}

Arg *pika_lvgl_lv_event(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_lv_event);
}
void pika_lvgl_lv_obj_add_event_cbMethod(PikaObj *self, Args *args){
    Arg* event_cb = args_getArg(args, "event_cb");
    int filter = args_getInt(args, "filter");
    void* user_data = args_getPtr(args, "user_data");
    pika_lvgl_lv_obj_add_event_cb(self, event_cb, filter, user_data);
}

void pika_lvgl_lv_obj_add_stateMethod(PikaObj *self, Args *args){
    int state = args_getInt(args, "state");
    pika_lvgl_lv_obj_add_state(self, state);
}

void pika_lvgl_lv_obj_add_styleMethod(PikaObj *self, Args *args){
    PikaObj* style = args_getPtr(args, "style");
    int selector = args_getInt(args, "selector");
    pika_lvgl_lv_obj_add_style(self, style, selector);
}

void pika_lvgl_lv_obj_alignMethod(PikaObj *self, Args *args){
    int align = args_getInt(args, "align");
    int x_ofs = args_getInt(args, "x_ofs");
    int y_ofs = args_getInt(args, "y_ofs");
    pika_lvgl_lv_obj_align(self, align, x_ofs, y_ofs);
}

void pika_lvgl_lv_obj_centerMethod(PikaObj *self, Args *args){
    pika_lvgl_lv_obj_center(self);
}

void pika_lvgl_lv_obj_get_xMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_lv_obj_get_x(self);
    method_returnInt(args, res);
}

void pika_lvgl_lv_obj_get_yMethod(PikaObj *self, Args *args){
    int res = pika_lvgl_lv_obj_get_y(self);
    method_returnInt(args, res);
}

void pika_lvgl_lv_obj_set_hightMethod(PikaObj *self, Args *args){
    int h = args_getInt(args, "h");
    pika_lvgl_lv_obj_set_hight(self, h);
}

void pika_lvgl_lv_obj_set_posMethod(PikaObj *self, Args *args){
    int x = args_getInt(args, "x");
    int y = args_getInt(args, "y");
    pika_lvgl_lv_obj_set_pos(self, x, y);
}

void pika_lvgl_lv_obj_set_sizeMethod(PikaObj *self, Args *args){
    int size_x = args_getInt(args, "size_x");
    int size_y = args_getInt(args, "size_y");
    pika_lvgl_lv_obj_set_size(self, size_x, size_y);
}

void pika_lvgl_lv_obj_set_widthMethod(PikaObj *self, Args *args){
    int w = args_getInt(args, "w");
    pika_lvgl_lv_obj_set_width(self, w);
}

void pika_lvgl_lv_obj_update_layoutMethod(PikaObj *self, Args *args){
    pika_lvgl_lv_obj_update_layout(self);
}

PikaObj *New_pika_lvgl_lv_obj(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "add_event_cb(event_cb:any,filter:int,user_data:pointer)", pika_lvgl_lv_obj_add_event_cbMethod);
    class_defineMethod(self, "add_state(state:int)", pika_lvgl_lv_obj_add_stateMethod);
    class_defineMethod(self, "add_style(style:style_t,selector:int)", pika_lvgl_lv_obj_add_styleMethod);
    class_defineMethod(self, "align(align:int,x_ofs:int,y_ofs:int)", pika_lvgl_lv_obj_alignMethod);
    class_defineMethod(self, "center()", pika_lvgl_lv_obj_centerMethod);
    class_defineMethod(self, "get_x()->int", pika_lvgl_lv_obj_get_xMethod);
    class_defineMethod(self, "get_y()->int", pika_lvgl_lv_obj_get_yMethod);
    class_defineMethod(self, "set_hight(h:int)", pika_lvgl_lv_obj_set_hightMethod);
    class_defineMethod(self, "set_pos(x:int,y:int)", pika_lvgl_lv_obj_set_posMethod);
    class_defineMethod(self, "set_size(size_x:int,size_y:int)", pika_lvgl_lv_obj_set_sizeMethod);
    class_defineMethod(self, "set_width(w:int)", pika_lvgl_lv_obj_set_widthMethod);
    class_defineMethod(self, "update_layout()", pika_lvgl_lv_obj_update_layoutMethod);
    return self;
}

Arg *pika_lvgl_lv_obj(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_lv_obj);
}
void pika_lvgl_lv_timer_t__delMethod(PikaObj *self, Args *args){
    pika_lvgl_lv_timer_t__del(self);
}

void pika_lvgl_lv_timer_t_set_cbMethod(PikaObj *self, Args *args){
    Arg* cb = args_getArg(args, "cb");
    pika_lvgl_lv_timer_t_set_cb(self, cb);
}

void pika_lvgl_lv_timer_t_set_periodMethod(PikaObj *self, Args *args){
    int period = args_getInt(args, "period");
    pika_lvgl_lv_timer_t_set_period(self, period);
}

PikaObj *New_pika_lvgl_lv_timer_t(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "_del()", pika_lvgl_lv_timer_t__delMethod);
    class_defineMethod(self, "set_cb(cb:any)", pika_lvgl_lv_timer_t_set_cbMethod);
    class_defineMethod(self, "set_period(period:int)", pika_lvgl_lv_timer_t_set_periodMethod);
    return self;
}

Arg *pika_lvgl_lv_timer_t(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_lv_timer_t);
}
void pika_lvgl_point_t___init__Method(PikaObj *self, Args *args){
    pika_lvgl_point_t___init__(self);
}

PikaObj *New_pika_lvgl_point_t(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__init__()", pika_lvgl_point_t___init__Method);
    return self;
}

Arg *pika_lvgl_point_t(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_point_t);
}
void pika_lvgl_roller___init__Method(PikaObj *self, Args *args){
    PikaObj* parent = args_getPtr(args, "parent");
    pika_lvgl_roller___init__(self, parent);
}

void pika_lvgl_roller_set_optionsMethod(PikaObj *self, Args *args){
    char* options = args_getStr(args, "options");
    int mode = args_getInt(args, "mode");
    pika_lvgl_roller_set_options(self, options, mode);
}

void pika_lvgl_roller_set_visible_row_countMethod(PikaObj *self, Args *args){
    int row_cnt = args_getInt(args, "row_cnt");
    pika_lvgl_roller_set_visible_row_count(self, row_cnt);
}

PikaObj *New_pika_lvgl_roller(Args *args){
    PikaObj *self = New_pika_lvgl_lv_obj(args);
    class_defineMethod(self, "__init__(parent:lv_obj)", pika_lvgl_roller___init__Method);
    class_defineMethod(self, "set_options(options:str,mode:int)", pika_lvgl_roller_set_optionsMethod);
    class_defineMethod(self, "set_visible_row_count(row_cnt:int)", pika_lvgl_roller_set_visible_row_countMethod);
    return self;
}

Arg *pika_lvgl_roller(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_roller);
}
void pika_lvgl_slider___init__Method(PikaObj *self, Args *args){
    PikaObj* parent = args_getPtr(args, "parent");
    pika_lvgl_slider___init__(self, parent);
}

PikaObj *New_pika_lvgl_slider(Args *args){
    PikaObj *self = New_pika_lvgl_lv_obj(args);
    class_defineMethod(self, "__init__(parent:lv_obj)", pika_lvgl_slider___init__Method);
    return self;
}

Arg *pika_lvgl_slider(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_slider);
}
void pika_lvgl_style_t___init__Method(PikaObj *self, Args *args){
    pika_lvgl_style_t___init__(self);
}

void pika_lvgl_style_t_initMethod(PikaObj *self, Args *args){
    pika_lvgl_style_t_init(self);
}

void pika_lvgl_style_t_set_bg_colorMethod(PikaObj *self, Args *args){
    PikaObj* color = args_getPtr(args, "color");
    pika_lvgl_style_t_set_bg_color(self, color);
}

void pika_lvgl_style_t_set_bg_opaMethod(PikaObj *self, Args *args){
    int opa = args_getInt(args, "opa");
    pika_lvgl_style_t_set_bg_opa(self, opa);
}

void pika_lvgl_style_t_set_outline_colorMethod(PikaObj *self, Args *args){
    PikaObj* color = args_getPtr(args, "color");
    pika_lvgl_style_t_set_outline_color(self, color);
}

void pika_lvgl_style_t_set_outline_padMethod(PikaObj *self, Args *args){
    int pad = args_getInt(args, "pad");
    pika_lvgl_style_t_set_outline_pad(self, pad);
}

void pika_lvgl_style_t_set_outline_widthMethod(PikaObj *self, Args *args){
    int w = args_getInt(args, "w");
    pika_lvgl_style_t_set_outline_width(self, w);
}

void pika_lvgl_style_t_set_radiusMethod(PikaObj *self, Args *args){
    int radius = args_getInt(args, "radius");
    pika_lvgl_style_t_set_radius(self, radius);
}

void pika_lvgl_style_t_set_shadow_colorMethod(PikaObj *self, Args *args){
    PikaObj* color = args_getPtr(args, "color");
    pika_lvgl_style_t_set_shadow_color(self, color);
}

void pika_lvgl_style_t_set_shadow_spreadMethod(PikaObj *self, Args *args){
    int s = args_getInt(args, "s");
    pika_lvgl_style_t_set_shadow_spread(self, s);
}

void pika_lvgl_style_t_set_shadow_widthMethod(PikaObj *self, Args *args){
    int w = args_getInt(args, "w");
    pika_lvgl_style_t_set_shadow_width(self, w);
}

PikaObj *New_pika_lvgl_style_t(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__init__()", pika_lvgl_style_t___init__Method);
    class_defineMethod(self, "init()", pika_lvgl_style_t_initMethod);
    class_defineMethod(self, "set_bg_color(color:lv_color_t)", pika_lvgl_style_t_set_bg_colorMethod);
    class_defineMethod(self, "set_bg_opa(opa:int)", pika_lvgl_style_t_set_bg_opaMethod);
    class_defineMethod(self, "set_outline_color(color:lv_color_t)", pika_lvgl_style_t_set_outline_colorMethod);
    class_defineMethod(self, "set_outline_pad(pad:int)", pika_lvgl_style_t_set_outline_padMethod);
    class_defineMethod(self, "set_outline_width(w:int)", pika_lvgl_style_t_set_outline_widthMethod);
    class_defineMethod(self, "set_radius(radius:int)", pika_lvgl_style_t_set_radiusMethod);
    class_defineMethod(self, "set_shadow_color(color:lv_color_t)", pika_lvgl_style_t_set_shadow_colorMethod);
    class_defineMethod(self, "set_shadow_spread(s:int)", pika_lvgl_style_t_set_shadow_spreadMethod);
    class_defineMethod(self, "set_shadow_width(w:int)", pika_lvgl_style_t_set_shadow_widthMethod);
    return self;
}

Arg *pika_lvgl_style_t(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_style_t);
}
void pika_lvgl_switch___init__Method(PikaObj *self, Args *args){
    PikaObj* parent = args_getPtr(args, "parent");
    pika_lvgl_switch___init__(self, parent);
}

PikaObj *New_pika_lvgl_switch(Args *args){
    PikaObj *self = New_pika_lvgl_lv_obj(args);
    class_defineMethod(self, "__init__(parent:lv_obj)", pika_lvgl_switch___init__Method);
    return self;
}

Arg *pika_lvgl_switch(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_switch);
}
void pika_lvgl_table___init__Method(PikaObj *self, Args *args){
    PikaObj* parent = args_getPtr(args, "parent");
    pika_lvgl_table___init__(self, parent);
}

void pika_lvgl_table_set_cell_valueMethod(PikaObj *self, Args *args){
    int row = args_getInt(args, "row");
    int col = args_getInt(args, "col");
    char* txt = args_getStr(args, "txt");
    pika_lvgl_table_set_cell_value(self, row, col, txt);
}

PikaObj *New_pika_lvgl_table(Args *args){
    PikaObj *self = New_pika_lvgl_lv_obj(args);
    class_defineMethod(self, "__init__(parent:lv_obj)", pika_lvgl_table___init__Method);
    class_defineMethod(self, "set_cell_value(row:int,col:int,txt:str)", pika_lvgl_table_set_cell_valueMethod);
    return self;
}

Arg *pika_lvgl_table(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_table);
}
void pika_lvgl_textarea___init__Method(PikaObj *self, Args *args){
    PikaObj* parent = args_getPtr(args, "parent");
    pika_lvgl_textarea___init__(self, parent);
}

void pika_lvgl_textarea_set_one_lineMethod(PikaObj *self, Args *args){
    int en = args_getInt(args, "en");
    pika_lvgl_textarea_set_one_line(self, en);
}

PikaObj *New_pika_lvgl_textarea(Args *args){
    PikaObj *self = New_pika_lvgl_lv_obj(args);
    class_defineMethod(self, "__init__(parent:lv_obj)", pika_lvgl_textarea___init__Method);
    class_defineMethod(self, "set_one_line(en:int)", pika_lvgl_textarea_set_one_lineMethod);
    return self;
}

Arg *pika_lvgl_textarea(PikaObj *self){
    return obj_newObjInPackage(New_pika_lvgl_textarea);
}
