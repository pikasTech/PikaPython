/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdData_List__H
#define __PikaStdData_List__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdData_List(Args *args);

PikaObj* PikaStdData_List___add__(PikaObj *self, PikaObj* others);
void PikaStdData_List___init__(PikaObj *self);
void PikaStdData_List___setitem__(PikaObj *self, Arg* __key, Arg* __val);
char* PikaStdData_List___str__(PikaObj *self);
void PikaStdData_List_append(PikaObj *self, Arg* arg);
void PikaStdData_List_reverse(PikaObj *self);
void PikaStdData_List_set(PikaObj *self, int i, Arg* arg);

#endif
