/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __STM32_Time__H
#define __STM32_Time__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_STM32_Time(Args *args);

void STM32_Time_sleep_ms(PikaObj *self, int ms);
void STM32_Time_sleep_s(PikaObj *self, int s);

#endif
