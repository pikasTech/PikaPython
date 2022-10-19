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

void queue_init(Queue* queue) {
    args_setInt(queue, "__t", 0);
    args_setInt(queue, "__b", 0);
}

Queue* New_queue(void) {
    Args* args = New_args(NULL);
    queue_init(args);
    return (Queue*)args;
}

int32_t queue_deinit(Queue* queue) {
    Args* args = queue;
    args_deinit(args);
    return 0;
}

int32_t queue_pushArg(Queue* queue, Arg* arg) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "__t");
    /* add top */
    args_setInt(args, "__t", top + 1);
    char buff[11];
    arg = arg_setName(arg, fast_itoa(buff, top));
    return args_setArg(args, arg);
}

Arg* __queue_popArg_noRmoveArg(Queue* queue) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "__t");
    uint64_t bottom = args_getInt(args, "__b");
    if (top - bottom < 1) {
        return NULL;
    }
    /* add bottom */
    args_setInt(args, "__b", bottom + 1);
    char buff[11];
    Arg* res = args_getArg(args, fast_itoa(buff, bottom));
    /* not deinit arg to keep str buff */
    return res;
}

int32_t queue_pushInt(Queue* queue, int val) {
    return queue_pushArg(queue, arg_newInt(val));
}

int64_t queue_popInt(Queue* queue) {
    return arg_getInt(__queue_popArg_noRmoveArg(queue));
}

int32_t queue_pushFloat(Queue* queue, pika_float val) {
    return queue_pushArg(queue, arg_newFloat(val));
}

pika_float queue_popFloat(Queue* queue) {
    return arg_getFloat(__queue_popArg_noRmoveArg(queue));
}

int32_t queue_pushStr(Queue* queue, char* str) {
    return queue_pushArg(queue, arg_newStr(str));
}

char* queue_popStr(Queue* queue) {
    return arg_getStr(__queue_popArg_noRmoveArg(queue));
}
