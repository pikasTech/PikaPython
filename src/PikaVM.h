#ifndef __PIKA__VM__H
#define __PIKA__VM__H
#include "PikaObj.h"

Parameters* pikaVM_run(PikaObj* self, char* pyLine);
Parameters* pikaVM_runAsm(PikaObj* self, char* pikaAsm);
Parameters* pikaVM_runAsmWithArgs(PikaObj* self,
                                  Parameters* globals,
                                  Parameters* locals,
                                  char* pikaAsm);

int32_t __clearInvokeQueues(Parameters* globals);
char* useFlashAsBuff(char* pikaAsm, Args* buffs);
int32_t gotoNextLine(char* code);
int32_t gotoLastLine(char* start, char* code);
int getThisBlockDeepth(char* start, char* code, int* offset);
int32_t pikaVM_runAsmLine(PikaObj* self,
                          Parameters* globals,
                          char* pikaAsm,
                          int32_t lineAddr);

#endif