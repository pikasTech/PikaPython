/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __MemoryChecker__H
#define __MemoryChecker__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_MemoryChecker(Args *args);

void MemoryChecker_max(PikaObj *self);
void MemoryChecker_now(PikaObj *self);

#endif
