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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataArgs.h"

static uint16_t const str100p[100] = {
    0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730, 0x3830,
    0x3930, 0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731,
    0x3831, 0x3931, 0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632,
    0x3732, 0x3832, 0x3932, 0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533,
    0x3633, 0x3733, 0x3833, 0x3933, 0x3034, 0x3134, 0x3234, 0x3334, 0x3434,
    0x3534, 0x3634, 0x3734, 0x3834, 0x3934, 0x3035, 0x3135, 0x3235, 0x3335,
    0x3435, 0x3535, 0x3635, 0x3735, 0x3835, 0x3935, 0x3036, 0x3136, 0x3236,
    0x3336, 0x3436, 0x3536, 0x3636, 0x3736, 0x3836, 0x3936, 0x3037, 0x3137,
    0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737, 0x3837, 0x3937, 0x3038,
    0x3138, 0x3238, 0x3338, 0x3438, 0x3538, 0x3638, 0x3738, 0x3838, 0x3938,
    0x3039, 0x3139, 0x3239, 0x3339, 0x3439, 0x3539, 0x3639, 0x3739, 0x3839,
    0x3939,
};

char* fast_itoa(char* buf, uint32_t val) {
    char* p = &buf[10];

    *p = '\0';

    while (val >= 100) {
        uint32_t const old = val;

        p -= 2;
        val /= 100;
        memcpy(p, &str100p[old - (val * 100)], sizeof(uint16_t));
    }

    p -= 2;
    memcpy(p, &str100p[val], sizeof(uint16_t));

    return &p[val < 10];
}

Queue* New_queue(void) {
    Args* args = New_args(NULL);
    args_setInt(args, "top", 0);
    args_setInt(args, "bottom", 0);
    Queue* queue = args;
    return queue;
}

int32_t queue_deinit(Queue* queue) {
    Args* args = queue;
    args_deinit(args);
    return 0;
}

int32_t queue_pushInt(Queue* queue, int val) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "top");
    char buff[11];
    char* topStr = fast_itoa(buff, top);
    /* add top */
    args_setInt(args, "top", top + 1);
    return args_setInt(args, topStr, val);
}

int64_t queue_popInt(Queue* queue) {
    Args* args = queue;
    uint64_t bottom = args_getInt(args, "bottom");
    char buff[11];
    char* bottomStr = fast_itoa(buff, bottom);
    /* add bottom */
    args_setInt(args, "bottom", bottom + 1);
    int64_t res = args_getInt(args, bottomStr);
    args_removeArg(args, args_getArg(args, bottomStr));
    return res;
}

int32_t queue_pushFloat(Queue* queue, float val) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "top");
    char buff[11];
    char* topStr = fast_itoa(buff, top);
    /* add top */
    args_setInt(args, "top", top + 1);
    return args_setFloat(args, topStr, val);
}

float queue_popFloat(Queue* queue) {
    Args* args = queue;
    uint64_t bottom = args_getInt(args, "bottom");
    char buff[11];
    char* bottomStr = fast_itoa(buff, bottom);
    /* add bottom */
    args_setInt(args, "bottom", bottom + 1);
    float res = args_getFloat(args, bottomStr);
    args_removeArg(args, args_getArg(args, bottomStr));
    return res;
}

int32_t queue_pushStr(Queue* queue, char* str) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "top");
    char buff[11];
    /* add top */
    char* topStr = fast_itoa(buff, top);
    args_setInt(args, "top", top + 1);
    return args_setStr(args, topStr, str);
}

char* queue_popStr(Queue* queue) {
    Args* args = queue;
    uint64_t bottom = args_getInt(args, "bottom");
    char buff[11];
    /* add bottom */
    args_setInt(args, "bottom", bottom + 1);
    return args_getStr(args, fast_itoa(buff, bottom));
}

int32_t queue_pushArg(Queue* queue, Arg* arg) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "top");
    /* add top */
    args_setInt(args, "top", top + 1);
    char buff[11];
    arg = arg_setName(arg, fast_itoa(buff, top));
    return args_setArg(args, arg);
}

Arg* queue_popArg(Queue* queue) {
    Args* args = queue;
    uint64_t bottom = args_getInt(args, "bottom");
    /* add bottom */
    args_setInt(args, "bottom", bottom + 1);
    char buff[11];
    Arg* res = args_getArg(args, fast_itoa(buff, bottom));
    return res;
}
