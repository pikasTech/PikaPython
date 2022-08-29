/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __re_Pattern__H
#define __re_Pattern__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_re_Pattern(Args *args);

void re_Pattern___del__(PikaObj *self);
void re_Pattern___init__(PikaObj *self);
PikaObj* re_Pattern_findall(PikaObj *self, char* subject, PikaTuple* flags);
PikaObj* re_Pattern_fullmatch(PikaObj *self, char* subject, PikaTuple* flags);
PikaObj* re_Pattern_match(PikaObj *self, char* subject, PikaTuple* flags);
PikaObj* re_Pattern_search(PikaObj *self, char* subject, PikaTuple* flags);
char* re_Pattern_sub(PikaObj *self, char* repl, char* subjet, PikaTuple* flags);

#endif
