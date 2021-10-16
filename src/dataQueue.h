/*
  Author: lyon
  Tencent QQ: 645275593
*/

#ifndef __DATA_QUEUE__H
#define __DATA_QUEUE__H
#include "dataArgs.h"

typedef Args Queue;
Queue* New_queue();

int32_t queue_deinit(Queue* queue);
int32_t queue_pushInt(Queue* queue, int val);
int32_t queue_pushFloat(Queue* queue, float val);
int32_t queue_pushStr(Queue* queue, char* str);
int32_t queue_pushArg(Queue* queue, Arg* arg);
char *fast_itoa(char *buf, uint32_t val);

int64_t queue_popInt(Queue* queue);
float queue_popFloat(Queue* queue);
char* queue_popStr(Queue* queue);
Arg* queue_popArg(Queue* queue);
#endif