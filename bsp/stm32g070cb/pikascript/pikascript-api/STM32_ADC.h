/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __STM32_ADC__H
#define __STM32_ADC__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_STM32_ADC(Args *args);

void STM32_ADC_platformEnable(PikaObj *self, char * pin);
float STM32_ADC_platformRead(PikaObj *self, char * pin);

#endif
