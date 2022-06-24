/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaDebug__H
#define __PikaDebug__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaDebug(Args *args);

Arg* PikaDebug_Debuger(PikaObj *self);

#endif
