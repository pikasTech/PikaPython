/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __Box__H
#define __Box__H
#include <stdio.h>
#include <stdlib.h>
#include "MimiObj.h"

MimiObj *New_Box(Args *args);

void Box_init(MimiObj *self);
void Box_moveLeft(MimiObj *self, int x);
void Box_moveRight(MimiObj *self, int x);
void Box_moveTo(MimiObj *self, int x, int y);
void Box_off(MimiObj *self);
void Box_on(MimiObj *self);
void Box_setColor(MimiObj *self, char * color);
void Box_setSize(MimiObj *self, int sizeX, int sizeY);

#endif
