/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdLib_StringObj__H
#define __PikaStdLib_StringObj__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdLib_StringObj(Args *args);

Arg * PikaStdLib_StringObj___next__(PikaObj *self);

#endif
