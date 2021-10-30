#ifndef __PIKA__VM__H
#define __PIKA__VM__H
#include "PikaObj.h"

Args* pikaVM_run(PikaObj* self, char* pyLine);
Args* pikaVM_runAsm(PikaObj* self, char* pikaAsm);
int32_t __clearInvokeQueues(Args* localArgs);
char* useFlashAsBuff(char* pikaAsm, Args* buffs);
int32_t gotoNextLine(char* code);
int32_t gotoLastLine(char* start, char* code);
int getThisBlockDeepth(char* start, char* code, int* offset);
Args* pikaVM_runAsmWithLocalArgs(PikaObj* self, Args* localArgs, char* pikaAsm);
int32_t pikaVM_runAsmLine(PikaObj* self,
                          Args* localArgs,
                          char* pikaAsm,
                          int32_t lineAddr);

#endif