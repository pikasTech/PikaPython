/*
 * This file is part of the PikaScript project.
 * http://github.com/pikastech/pikascript
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon 李昂 liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "dataQueue.h"
#include "PikaPlatform.h"
#include "dataArgs.h"

Queue* New_queue(void) {
    Args* args = New_args(NULL);
    args_setInt(args, "offset", 0);
    Queue* queue = args;
    return queue;
}

int32_t queue_deinit(Queue* queue) {
    Args* args = queue;
    args_deinit(args);
    return 0;
}

int32_t queue_pushArg(Queue* queue, Arg* arg) {
    Args* args = queue;
    int offset = args_getInt(args, "offset");
    /* add top */
    args_setInt(args, "offset", offset + 1);
    char buff[11];
    return args_pushArg(args, arg);
}

Arg* queue_popArg(Queue* queue) {
    Args* args = queue;
    int offset = args_getInt(args, "offset");
    if (offset - 1 < 0) {
        return NULL;
    }
    /* add bottom */
    args_setInt(args, "offset", offset - 1);
    char buff[11];
    Arg* res = args_getArg_index(args, offset - 1);
    return res;
}

int32_t queue_pushInt(Queue* queue, int val) {
    return queue_pushArg(queue, arg_setInt(NULL, "", val));
}

int64_t queue_popInt(Queue* queue) {
    return arg_getInt(queue_popArg(queue));
}

int32_t queue_pushFloat(Queue* queue, float val) {
    return queue_pushArg(queue, arg_setFloat(NULL, "", val));
}

float queue_popFloat(Queue* queue) {
    return arg_getFloat(queue_popArg(queue));
}

int32_t queue_pushStr(Queue* queue, char* str) {
    return queue_pushArg(queue, arg_setStr(NULL, "", str));
}

char* queue_popStr(Queue* queue) {
    return arg_getStr(queue_popArg(queue));
}