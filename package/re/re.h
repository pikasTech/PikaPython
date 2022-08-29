/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __re__H
#define __re__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_re(Args *args);

Arg* re_Match(PikaObj *self);
Arg* re_Pattern(PikaObj *self);
PikaObj* re_compile(PikaObj *self, char* pattern);
PikaObj* re_findall(PikaObj *self, char* pattern, char* subject, PikaTuple* flags);
PikaObj* re_fullmatch(PikaObj *self, char* pattern, char* subject, PikaTuple* flags);
PikaObj* re_match(PikaObj *self, char* pattern, char* subject, PikaTuple* flags);
PikaObj* re_search(PikaObj *self, char* pattern, char* subject, PikaTuple* flags);
char* re_sub(PikaObj *self, char* pattern, char* repl, char* subjet, PikaTuple* flags);

#endif
