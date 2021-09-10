/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __STM32_UART__H
#define __STM32_UART__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_STM32_UART(Args *args);

void STM32_UART_platformEnable(PikaObj *self, int baudRate, int id);
char * STM32_UART_platformRead(PikaObj *self, int id, int length);
void STM32_UART_platformWrite(PikaObj *self, char * data, int id);

#endif
