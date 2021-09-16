/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __STM32_GPIO__H
#define __STM32_GPIO__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_STM32_GPIO(Args *args);

void STM32_GPIO_platformDisable(PikaObj *self);
void STM32_GPIO_platformEnable(PikaObj *self);
void STM32_GPIO_platformHigh(PikaObj *self);
void STM32_GPIO_platformLow(PikaObj *self);
void STM32_GPIO_platformSetMode(PikaObj *self, char * mode);

#endif
