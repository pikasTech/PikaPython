/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdData_ByteArray__H
#define __PikaStdData_ByteArray__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdData_ByteArray(Args *args);

int PikaStdData_ByteArray___getitem__(PikaObj *self, int __key);
void PikaStdData_ByteArray___init__(PikaObj *self, Arg* bytes);
Arg* PikaStdData_ByteArray___iter__(PikaObj *self);
Arg* PikaStdData_ByteArray___next__(PikaObj *self);
void PikaStdData_ByteArray___setitem__(PikaObj *self, int __key, int __val);
char* PikaStdData_ByteArray___str__(PikaObj *self);
char* PikaStdData_ByteArray_decode(PikaObj *self);

#endif
