/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __LED__H
#define __LED__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_LED(Args *args);

void LED_off(PikaObj *self);
void LED_on(PikaObj *self);

#endif
