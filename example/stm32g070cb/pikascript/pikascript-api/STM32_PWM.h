/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __STM32_PWM__H
#define __STM32_PWM__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_STM32_PWM(Args *args);

void STM32_PWM_platformEnable(PikaObj *self, float duty, int freq, char * pin);
void STM32_PWM_platformSetDuty(PikaObj *self, float duty, char * pin);
void STM32_PWM_platformSetFrequency(PikaObj *self, int freq, char * pin);

#endif
