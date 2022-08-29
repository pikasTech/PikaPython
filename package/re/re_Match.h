/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __re_Match__H
#define __re_Match__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_re_Match(Args *args);

void re_Match___del__(PikaObj *self);
void re_Match___init__(PikaObj *self);
char* re_Match_group(PikaObj *self, int n);
PikaObj* re_Match_groups(PikaObj *self);
PikaObj* re_Match_span(PikaObj *self, int group_n);

#endif
