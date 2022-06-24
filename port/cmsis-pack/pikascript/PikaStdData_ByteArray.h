/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdData_ByteArray__H
#define __PikaStdData_ByteArray__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdData_ByteArray(Args *args);

void PikaStdData_ByteArray_fromString(PikaObj *self, char* s);

#endif
