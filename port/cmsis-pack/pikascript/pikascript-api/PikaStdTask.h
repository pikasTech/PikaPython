/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdTask__H
#define __PikaStdTask__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdTask(Args *args);

Arg* PikaStdTask_Task(PikaObj *self);

#endif
