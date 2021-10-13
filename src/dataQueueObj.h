#ifndef __DATA_QUEUE_OBJ__H
#define __DATA_QUEUE_OBJ__H
#include "PikaObj.h"
typedef PikaObj QueueObj;

int32_t queueObj_init(QueueObj* self);

int32_t queueObj_pushInt(QueueObj* self, int val);
int32_t queueObj_pushFloat(QueueObj* self, float val);
int32_t queueObj_pushStr(QueueObj* self, char* str);
int32_t queueObj_pushObj(QueueObj* self, char* className);

int64_t queueObj_popInt(QueueObj* self);
float queueObj_popFloat(QueueObj* self);
char* queueObj_popStr(QueueObj* self);
PikaObj* queueObj_popObj(QueueObj* self);

PikaObj* queueObj_getCurrentObj(QueueObj* self);

#endif