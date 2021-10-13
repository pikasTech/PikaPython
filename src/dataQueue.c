/*
  Author: lyon
  Tencent QQ: 645275593
*/

#include "dataQueue.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataArgs.h"

Queue* New_queue() {
    Args* args = New_args(NULL);
    args_setInt(args, "top", 0);
    args_setInt(args, "bottom", 0);
    Queue* queue = args;
    return queue;
}

Queue* queue_deinit(Queue* queue) {
    Args* args = queue;
    args_deinit(args);
}

int32_t queue_pushInt(Queue* queue, int val) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "top");
    char topStr[32] = {0};
    sprintf(topStr, "%ld", top);
    /* add top */
    args_setInt(args, "top", top + 1);
    return args_setInt(args, topStr, val);
}

int64_t queue_popInt(Queue* queue) {
    Args* args = queue;
    uint64_t bottom = args_getInt(args, "bottom");
    char bottomStr[32] = {0};
    sprintf(bottomStr, "%ld", bottom);
    /* add bottom */
    args_setInt(args, "bottom", bottom + 1);
    int64_t res = args_getInt(args, bottomStr);
    args_removeArg(args, bottomStr);
    return res;
}

int32_t queue_pushFloat(Queue* queue, float val) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "top");
    char topStr[32] = {0};
    sprintf(topStr, "%ld", top);
    /* add top */
    args_setInt(args, "top", top + 1);
    return args_setFloat(args, topStr, val);
}

float queue_popFloat(Queue* queue) {
    Args* args = queue;
    uint64_t bottom = args_getInt(args, "bottom");
    char bottomStr[32] = {0};
    sprintf(bottomStr, "%ld", bottom);
    /* add bottom */
    args_setInt(args, "bottom", bottom + 1);
    float res = args_getFloat(args, bottomStr);
    args_removeArg(args, bottomStr);
    return res;
}

int32_t queue_pushStr(Queue* queue, char* str) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "top");
    char topStr[32] = {0};
    sprintf(topStr, "%ld", top);
    /* add top */
    args_setInt(args, "top", top + 1);
    return args_setStr(args, topStr, str);
}

char* queue_popStr(Queue* queue) {
    Args* args = queue;
    uint64_t bottom = args_getInt(args, "bottom");
    char bottomStr[32] = {0};
    sprintf(bottomStr, "%ld", bottom);
    /* add bottom */
    args_setInt(args, "bottom", bottom + 1);
    return args_getStr(args, bottomStr);
}