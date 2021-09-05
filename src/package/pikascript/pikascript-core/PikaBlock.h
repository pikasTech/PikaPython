#ifndef _pikaBlock__H
#define _pikaBlock__H
#include "PikaObj.h"

void block_deinit(PikaObj *self);
PikaObj *block_init(Args *args);
char *block_popLine(PikaObj *self);
void block_pushLine(PikaObj *self, char *line);
char *block_getBody(PikaObj *self);
char *block_getAssert(PikaObj *self);
void block_setAssert(PikaObj *self, char *assert);
uint8_t block_checkAssert(PikaObj *self);
uint16_t block_getLineSize(PikaObj *self);
void block_setMode(PikaObj *self, char *mode);
char *block_getMode(PikaObj *self);
#endif