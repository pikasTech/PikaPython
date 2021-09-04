#ifndef _pikaIf__H
#define _pikaIf__H
#include "PikaObj.h"

void if_pushLine(PikaObj *self, char *line);
void if_setAssert(PikaObj *self, char *line);
void if_run(PikaObj *self);
#endif

