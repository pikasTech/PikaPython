/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdLib_SysObj__H
#define __PikaStdLib_SysObj__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdLib_SysObj(Args *args);

Arg * PikaStdLib_SysObj___get__(PikaObj *self, Arg * key, Arg * obj);
void PikaStdLib_SysObj___set__(PikaObj *self, Arg * key, Arg * obj, Arg * val);
float PikaStdLib_SysObj_float(PikaObj *self, Arg * arg);
int PikaStdLib_SysObj_int(PikaObj *self, Arg * arg);
Arg * PikaStdLib_SysObj_iter(PikaObj *self, Arg * arg);
Arg * PikaStdLib_SysObj_next(PikaObj *self, Arg * arg);
Arg * PikaStdLib_SysObj_range(PikaObj *self, int a1, int a2, int a3);
void PikaStdLib_SysObj_remove(PikaObj *self, char * argPath);
char * PikaStdLib_SysObj_str(PikaObj *self, Arg * arg);
void PikaStdLib_SysObj_type(PikaObj *self, Arg * arg);

#endif
