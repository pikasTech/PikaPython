#ifndef __PIKA__VM__H
#define __PIKA__VM__H
#include "PikaObj.h"

int32_t pikaVM_run(PikaObj* self, char* pyLine);
int32_t pikaVM_runAsm(PikaObj* self, char* pikaAsm);

#endif