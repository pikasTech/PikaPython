/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "re.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void re_MatchMethod(PikaObj *self, Args *args)
{
    Arg *res = re_Match(self);
    method_returnArg(args, res);
}

void re_PatternMethod(PikaObj *self, Args *args)
{
    Arg *res = re_Pattern(self);
    method_returnArg(args, res);
}

void re_compileMethod(PikaObj *self, Args *args)
{
    char *pattern = args_getStr(args, "pattern");
    PikaObj *res = re_compile(self, pattern);
    method_returnObj(args, res);
}

void re_findallMethod(PikaObj *self, Args *args)
{
    char *pattern = args_getStr(args, "pattern");
    char *subject = args_getStr(args, "subject");
    PikaTuple *flags = args_getTuple(args, "flags");
    PikaObj *res = re_findall(self, pattern, subject, flags);
    method_returnObj(args, res);
}

void re_fullmatchMethod(PikaObj *self, Args *args)
{
    char *pattern = args_getStr(args, "pattern");
    char *subject = args_getStr(args, "subject");
    PikaTuple *flags = args_getTuple(args, "flags");
    PikaObj *res = re_fullmatch(self, pattern, subject, flags);
    method_returnObj(args, res);
}

void re_matchMethod(PikaObj *self, Args *args)
{
    char *pattern = args_getStr(args, "pattern");
    char *subject = args_getStr(args, "subject");
    PikaTuple *flags = args_getTuple(args, "flags");
    PikaObj *res = re_match(self, pattern, subject, flags);
    method_returnObj(args, res);
}

void re_searchMethod(PikaObj *self, Args *args)
{
    char *pattern = args_getStr(args, "pattern");
    char *subject = args_getStr(args, "subject");
    PikaTuple *flags = args_getTuple(args, "flags");
    PikaObj *res = re_search(self, pattern, subject, flags);
    method_returnObj(args, res);
}

void re_subMethod(PikaObj *self, Args *args)
{
    char *pattern = args_getStr(args, "pattern");
    char *repl = args_getStr(args, "repl");
    char *subjet = args_getStr(args, "subjet");
    PikaTuple *flags = args_getTuple(args, "flags");
    char *res = re_sub(self, pattern, repl, subjet, flags);
    method_returnStr(args, res);
}

PikaObj *New_re(Args *args)
{
    PikaObj *self = New_TinyObj(args);
    void pre_init_re(PikaObj * self);
    pre_init_re(self);
    class_defineConstructor(self, "Match()->any", re_MatchMethod);
    class_defineConstructor(self, "Pattern()->any", re_PatternMethod);
    class_defineMethod(self, "compile(pattern:str)->Pattern", re_compileMethod);
    class_defineMethod(self, "findall(pattern:str,subject:str,*flags)->list", re_findallMethod);
    class_defineMethod(self, "fullmatch(pattern:str,subject:str,*flags)->Match", re_fullmatchMethod);
    class_defineMethod(self, "match(pattern:str,subject:str,*flags)->Match", re_matchMethod);
    class_defineMethod(self, "search(pattern:str,subject:str,*flags)->Match", re_searchMethod);
    class_defineMethod(self, "sub(pattern:str,repl:str,subjet:str,*flags)->str", re_subMethod);
    return self;
}
