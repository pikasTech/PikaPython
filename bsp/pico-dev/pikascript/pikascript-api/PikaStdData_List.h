/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdData_List__H
#define __PikaStdData_List__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdData_List(Args *args);

Arg* PikaStdData_List___get__(PikaObj *self, Arg* __key);
void PikaStdData_List___init__(PikaObj *self);
Arg* PikaStdData_List___iter__(PikaObj *self);
Arg* PikaStdData_List___next__(PikaObj *self);
void PikaStdData_List___set__(PikaObj *self, Arg* __key, Arg* __val);
void PikaStdData_List_append(PikaObj *self, Arg* arg);
Arg* PikaStdData_List_get(PikaObj *self, int i);
int PikaStdData_List_len(PikaObj *self);
void PikaStdData_List_set(PikaObj *self, Arg* arg, int i);

#endif
