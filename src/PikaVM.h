#ifndef __PIKA__VM__H
#define __PIKA__VM__H
#include "PikaObj.h"

int32_t pikaVM_run(PikaObj* self, char* pikaAsm, int32_t lineAddr);

#endif