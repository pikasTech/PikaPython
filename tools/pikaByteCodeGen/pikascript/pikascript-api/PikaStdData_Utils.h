/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdData_Utils__H
#define __PikaStdData_Utils__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdData_Utils(Args *args);

Arg* PikaStdData_Utils_int_to_bytes(PikaObj *self, int val);

#endif
