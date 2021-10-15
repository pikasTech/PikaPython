#ifndef __PIKA__VM__H
#define __PIKA__VM__H
#include "PikaObj.h"

Args* pikaVM_run(PikaObj* self, char* pyLine);
Args* pikaVM_runAsm(PikaObj* self, char* pikaAsm);

#endif