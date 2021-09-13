/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdDevice_Time__H
#define __PikaStdDevice_Time__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdDevice_Time(Args *args);

void PikaStdDevice_Time_sleep_ms(PikaObj *self, int ms);
void PikaStdDevice_Time_sleep_s(PikaObj *self, int s);

#endif
