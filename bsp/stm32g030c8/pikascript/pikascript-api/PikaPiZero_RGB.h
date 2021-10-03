/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaPiZero_RGB__H
#define __PikaPiZero_RGB__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaPiZero_RGB(Args *args);

void PikaPiZero_RGB_blue(PikaObj *self);
void PikaPiZero_RGB_enable(PikaObj *self);
void PikaPiZero_RGB_flow(PikaObj *self);
void PikaPiZero_RGB_green(PikaObj *self);
void PikaPiZero_RGB_init(PikaObj *self);
void PikaPiZero_RGB_red(PikaObj *self);
void PikaPiZero_RGB_white(PikaObj *self);

#endif
