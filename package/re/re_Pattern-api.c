/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "re_Pattern.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void re_Pattern___del__Method(PikaObj *self, Args *args){
    re_Pattern___del__(self);
}

void re_Pattern___init__Method(PikaObj *self, Args *args){
    re_Pattern___init__(self);
}

void re_Pattern_findallMethod(PikaObj *self, Args *args){
    char* subject = args_getStr(args, "subject");
    PikaTuple* flags = args_getTuple(args, "flags");
    PikaObj* res = re_Pattern_findall(self, subject, flags);
    method_returnObj(args, res);
}

void re_Pattern_fullmatchMethod(PikaObj *self, Args *args){
    char* subject = args_getStr(args, "subject");
    PikaTuple* flags = args_getTuple(args, "flags");
    PikaObj* res = re_Pattern_fullmatch(self, subject, flags);
    method_returnObj(args, res);
}

void re_Pattern_matchMethod(PikaObj *self, Args *args){
    char* subject = args_getStr(args, "subject");
    PikaTuple* flags = args_getTuple(args, "flags");
    PikaObj* res = re_Pattern_match(self, subject, flags);
    method_returnObj(args, res);
}

void re_Pattern_searchMethod(PikaObj *self, Args *args){
    char* subject = args_getStr(args, "subject");
    PikaTuple* flags = args_getTuple(args, "flags");
    PikaObj* res = re_Pattern_search(self, subject, flags);
    method_returnObj(args, res);
}

void re_Pattern_subMethod(PikaObj *self, Args *args){
    char* repl = args_getStr(args, "repl");
    char* subjet = args_getStr(args, "subjet");
    PikaTuple* flags = args_getTuple(args, "flags");
    char* res = re_Pattern_sub(self, repl, subjet, flags);
    method_returnStr(args, res);
}

PikaObj *New_re_Pattern(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__del__()", re_Pattern___del__Method);
    class_defineMethod(self, "__init__()", re_Pattern___init__Method);
    class_defineMethod(self, "findall(subject:str,*flags)->list", re_Pattern_findallMethod);
    class_defineMethod(self, "fullmatch(subject:str,*flags)->Match", re_Pattern_fullmatchMethod);
    class_defineMethod(self, "match(subject:str,*flags)->Match", re_Pattern_matchMethod);
    class_defineMethod(self, "search(subject:str,*flags)->Match", re_Pattern_searchMethod);
    class_defineMethod(self, "sub(repl:str,subjet:str,*flags)->str", re_Pattern_subMethod);
    return self;
}

Arg *re_Pattern(PikaObj *self){
    return obj_newObjInPackage(New_re_Pattern);
}
