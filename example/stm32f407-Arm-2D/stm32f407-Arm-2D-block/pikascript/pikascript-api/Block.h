/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __Block__H
#define __Block__H
#include <stdio.h>
#include <stdlib.h>
#include "MimiObj.h"

MimiObj *New_Block(Args *args);

void Block_init(MimiObj *self);
void Block_moveLeft(MimiObj *self, int x);
void Block_moveRight(MimiObj *self, int x);
void Block_moveTo(MimiObj *self, int x, int y);
void Block_off(MimiObj *self);
void Block_on(MimiObj *self);
void Block_setColor(MimiObj *self, char * color);
void Block_setSize(MimiObj *self, int sizeX, int sizeY);

#endif
