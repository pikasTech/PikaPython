/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdData_dict_keys__H
#define __PikaStdData_dict_keys__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdData_dict_keys(Args *args);

Arg* PikaStdData_dict_keys___iter__(PikaObj *self);
Arg* PikaStdData_dict_keys___next__(PikaObj *self);

#endif
