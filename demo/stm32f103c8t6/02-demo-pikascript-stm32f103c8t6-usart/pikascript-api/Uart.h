/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __Uart__H
#define __Uart__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_Uart(Args *args);

void Uart_printName(PikaObj *self);
void Uart_send(PikaObj *self, char * data);
void Uart_setName(PikaObj *self, char * name);

#endif
