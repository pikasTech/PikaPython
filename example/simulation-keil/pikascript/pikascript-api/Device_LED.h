/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __Device_LED__H
#define __Device_LED__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_Device_LED(Args *args);

void Device_LED_off(PikaObj *self);
void Device_LED_on(PikaObj *self);

#endif
