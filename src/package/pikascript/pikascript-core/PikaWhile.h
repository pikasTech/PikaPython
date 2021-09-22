/*
  Author: lyon
  Tencent QQ: 645275593
*/

#ifndef _pikaWhile__H
#define _pikaWhile__H
#include "PikaObj.h"

void while_setAssert(PikaObj* self, char* line);
void while_pushLine(PikaObj* self, char* line);
void while_run(PikaObj* self);

#endif
