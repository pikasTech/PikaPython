/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdData_Tuple__H
#define __PikaStdData_Tuple__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdData_Tuple(Args *args);

void PikaStdData_Tuple___del__(PikaObj *self);
Arg* PikaStdData_Tuple___getitem__(PikaObj *self, Arg* __key);
void PikaStdData_Tuple___init__(PikaObj *self);
Arg* PikaStdData_Tuple___iter__(PikaObj *self);
int PikaStdData_Tuple___len__(PikaObj *self);
Arg* PikaStdData_Tuple___next__(PikaObj *self);
char* PikaStdData_Tuple___str__(PikaObj *self);
Arg* PikaStdData_Tuple_get(PikaObj *self, int i);
int PikaStdData_Tuple_len(PikaObj *self);

#endif
