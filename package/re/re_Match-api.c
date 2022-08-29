/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "re_Match.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void re_Match___del__Method(PikaObj *self, Args *args){
    re_Match___del__(self);
}

void re_Match___init__Method(PikaObj *self, Args *args){
    re_Match___init__(self);
}

void re_Match_groupMethod(PikaObj *self, Args *args){
    int n = args_getInt(args, "n");
    char* res = re_Match_group(self, n);
    method_returnStr(args, res);
}

void re_Match_groupsMethod(PikaObj *self, Args *args){
    PikaObj* res = re_Match_groups(self);
    method_returnObj(args, res);
}

void re_Match_spanMethod(PikaObj *self, Args *args){
    int group_n = args_getInt(args, "group_n");
    PikaObj* res = re_Match_span(self, group_n);
    method_returnObj(args, res);
}

PikaObj *New_re_Match(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "__del__()", re_Match___del__Method);
    class_defineMethod(self, "__init__()", re_Match___init__Method);
    class_defineMethod(self, "group(n:int)->str", re_Match_groupMethod);
    class_defineMethod(self, "groups()->list", re_Match_groupsMethod);
    class_defineMethod(self, "span(group_n:int)->list", re_Match_spanMethod);
    return self;
}

Arg *re_Match(PikaObj *self){
    return obj_newObjInPackage(New_re_Match);
}
