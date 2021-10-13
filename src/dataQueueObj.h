#ifndef __DATA_QUEUE_OBJ__H
#define __DATA_QUEUE_OBJ__H
#include "PikaObj.h"
typedef PikaObj QueueObj;

int32_t queueObj_init(QueueObj* self);

int32_t queueObj_pushInt(QueueObj* self, int val);

int64_t queueObj_popInt(QueueObj* self);

int32_t queueObj_pushFloat(QueueObj* self, float val);

float queueObj_popFloat(QueueObj* self);

int32_t queueObj_pushStr(QueueObj* self, char* str);

char* queueObj_popStr(QueueObj* self);

#endif