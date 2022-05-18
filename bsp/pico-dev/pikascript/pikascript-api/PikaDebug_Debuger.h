/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaDebug_Debuger__H
#define __PikaDebug_Debuger__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaDebug_Debuger(Args *args);

void PikaDebug_Debuger___init__(PikaObj *self);
void PikaDebug_Debuger_set_trace(PikaObj *self);

#endif
