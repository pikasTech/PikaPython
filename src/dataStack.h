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

#ifndef __DATA_STACK__H
#define __DATA_STACK__H
#include "dataArgs.h"

typedef struct Stack_t {
    Arg* stack_pyload;
    Arg* stack_size_array;
    uint8_t* sp;
    int32_t* sp_size;
    int32_t top;
    size_t stack_totle_size;
} Stack;

int32_t stack_deinit(Stack* stack);

int32_t stack_pushStr(Stack* stack, char* str);
char* stack_popStr(Stack* stack, char* outBuff);
Arg* stack_checkArg(Stack* stack, int index);
int32_t stack_pushArg(Stack* stack, Arg* arg);
Arg* stack_popArg_alloc(Stack* stack);
Arg* stack_popArg(Stack* stack, Arg* arg_dict);
int32_t stack_getTop(Stack* stack);
int32_t stack_init(Stack* stack);
int32_t stack_popSize(Stack* stack);
void stack_pushSize(Stack* stack, int32_t size);
void stack_reset(Stack* stack);
#endif
