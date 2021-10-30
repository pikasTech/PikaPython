#ifndef __PIKA__VM__H
#define __PIKA__VM__H
#include "PikaObj.h"

PikaObj* pikaVM_run(PikaObj* self, char* pyLine);
PikaObj* pikaVM_runAsm(PikaObj* self, char* pikaAsm);
int32_t __clearInvokeQueues(PikaObj* globalArgs);
char* useFlashAsBuff(char* pikaAsm, Args* buffs);
int32_t gotoNextLine(char* code);
int32_t gotoLastLine(char* start, char* code);
int getThisBlockDeepth(char* start, char* code, int* offset);
PikaObj* pikaVM_runAsmWithArgs(PikaObj* self, PikaObj* globalArgs, char* pikaAsm);
int32_t pikaVM_runAsmLine(PikaObj* self,
                          PikaObj* globalArgs,
                          char* pikaAsm,
                          int32_t lineAddr);

#endif