/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdDeivce_Time__H
#define __PikaStdDeivce_Time__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdDeivce_Time(Args *args);

void PikaStdDeivce_Time_sleep_ms(PikaObj *self, int ms);
void PikaStdDeivce_Time_sleep_s(PikaObj *self, int s);

#endif
