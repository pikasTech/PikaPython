/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __Device__H
#define __Device__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_Device(Args *args);

Arg * Device_LED(PikaObj *self);
Arg * Device_Uart(PikaObj *self);

#endif
