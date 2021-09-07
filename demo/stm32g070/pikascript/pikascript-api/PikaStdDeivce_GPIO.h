/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdDeivce_GPIO__H
#define __PikaStdDeivce_GPIO__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdDeivce_GPIO(Args *args);

void PikaStdDeivce_GPIO_disable(PikaObj *self);
void PikaStdDeivce_GPIO_enable(PikaObj *self);
char * PikaStdDeivce_GPIO_getMode(PikaObj *self);
char * PikaStdDeivce_GPIO_getPin(PikaObj *self);
void PikaStdDeivce_GPIO_high(PikaObj *self);
void PikaStdDeivce_GPIO_init(PikaObj *self);
void PikaStdDeivce_GPIO_low(PikaObj *self);
void PikaStdDeivce_GPIO_platformDisable(PikaObj *self);
void PikaStdDeivce_GPIO_platformEnable(PikaObj *self);
void PikaStdDeivce_GPIO_platformHigh(PikaObj *self);
void PikaStdDeivce_GPIO_platformLow(PikaObj *self);
void PikaStdDeivce_GPIO_platformSetMode(PikaObj *self, char * mode);
void PikaStdDeivce_GPIO_setMode(PikaObj *self, char * mode);
void PikaStdDeivce_GPIO_setPin(PikaObj *self, char * pinName);

#endif
