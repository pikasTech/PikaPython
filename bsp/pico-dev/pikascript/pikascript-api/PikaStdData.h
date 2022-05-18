/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdData__H
#define __PikaStdData__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdData(Args *args);

Arg* PikaStdData_ByteArray(PikaObj *self);
Arg* PikaStdData_Dict(PikaObj *self);
Arg* PikaStdData_List(PikaObj *self);
Arg* PikaStdData_String(PikaObj *self);

#endif
