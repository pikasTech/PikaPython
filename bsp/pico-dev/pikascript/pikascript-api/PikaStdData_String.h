/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdData_String__H
#define __PikaStdData_String__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdData_String(Args *args);

Arg* PikaStdData_String___get__(PikaObj *self, Arg* __key);
void PikaStdData_String___init__(PikaObj *self, char* s);
Arg* PikaStdData_String___iter__(PikaObj *self);
Arg* PikaStdData_String___next__(PikaObj *self);
void PikaStdData_String___set__(PikaObj *self, Arg* __key, Arg* __val);
char* PikaStdData_String_get(PikaObj *self);
void PikaStdData_String_set(PikaObj *self, char* s);

#endif
