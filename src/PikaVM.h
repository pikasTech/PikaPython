#ifndef __PIKA__VM__H
#define __PIKA__VM__H
#include "PikaObj.h"

PikaObj* pikaVM_run(PikaObj* self, char* pyLine);
PikaObj* pikaVM_runAsm(PikaObj* self, char* pikaAsm);
int32_t __clearInvokeQueues(PikaObj* localArgs);
char* useFlashAsBuff(char* pikaAsm, Args* buffs);
int32_t gotoNextLine(char* code);
int32_t gotoLastLine(char* start, char* code);
int getThisBlockDeepth(char* start, char* code, int* offset);
PikaObj* pikaVM_runAsmWithLocalArgs(PikaObj* self, PikaObj* localArgs, char* pikaAsm);
int32_t pikaVM_runAsmLine(PikaObj* self,
                          PikaObj* localArgs,
                          char* pikaAsm,
                          int32_t lineAddr);

#endif