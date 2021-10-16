/*
  Author: lyon
  Tencent QQ: 645275593
*/

#ifndef __DATA_STACK__H
#define __DATA_STACK__H
#include "dataArgs.h"

typedef Args Stack;
Stack* New_Stack();
int32_t stack_deinit(Stack* stack);

int32_t stack_pushStr(Stack* stack, char* str);
char* stack_popStr(Stack* queue, char* buff);
#endif