/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __MemoryChecker__H
#define __MemoryChecker__H
#include <stdio.h>
#include <stdlib.h>
#include "MimiObj.h"

MimiObj *New_MemoryChecker(Args *args);

void MemoryChecker_max(MimiObj *self);
void MemoryChecker_now(MimiObj *self);
void MemoryChecker_size(MimiObj *self);

#endif
