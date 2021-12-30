/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdLib_MemChecker__H
#define __PikaStdLib_MemChecker__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdLib_MemChecker(Args *args);

float PikaStdLib_MemChecker_getMax(PikaObj *self);
float PikaStdLib_MemChecker_getNow(PikaObj *self);
void PikaStdLib_MemChecker_max(PikaObj *self);
void PikaStdLib_MemChecker_now(PikaObj *self);
void PikaStdLib_MemChecker_resetMax(PikaObj *self);

#endif
