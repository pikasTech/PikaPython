/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdLib_RangeObj__H
#define __PikaStdLib_RangeObj__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdLib_RangeObj(Args *args);

Arg * PikaStdLib_RangeObj___iter__(PikaObj *self);
Arg * PikaStdLib_RangeObj___next__(PikaObj *self);

#endif
