/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdData_Dict__H
#define __PikaStdData_Dict__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdData_Dict(Args *args);

void PikaStdData_Dict___del__(PikaObj *self);
Arg* PikaStdData_Dict___getitem__(PikaObj *self, Arg* __key);
void PikaStdData_Dict___init__(PikaObj *self);
Arg* PikaStdData_Dict___iter__(PikaObj *self);
int PikaStdData_Dict___len__(PikaObj *self);
Arg* PikaStdData_Dict___next__(PikaObj *self);
void PikaStdData_Dict___setitem__(PikaObj *self, Arg* __key, Arg* __val);
char* PikaStdData_Dict___str__(PikaObj *self);
Arg* PikaStdData_Dict_get(PikaObj *self, char* key);
PikaObj* PikaStdData_Dict_keys(PikaObj *self);
void PikaStdData_Dict_remove(PikaObj *self, char* key);
void PikaStdData_Dict_set(PikaObj *self, char* key, Arg* arg);

#endif
