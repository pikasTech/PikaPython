#ifndef _pikaBlock__H
#define _pikaBlock__H
#include "PikaObj.h"

void block_deinit(PikaObj *self);
PikaObj *block_init();
char *block_popLine(PikaObj *self);
void block_pushLine(PikaObj *self, char *line);
#endif