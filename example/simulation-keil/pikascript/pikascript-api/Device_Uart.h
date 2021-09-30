/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __Device_Uart__H
#define __Device_Uart__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_Device_Uart(Args *args);

void Device_Uart_printName(PikaObj *self);
void Device_Uart_send(PikaObj *self, char * data);
void Device_Uart_setName(PikaObj *self, char * name);

#endif
