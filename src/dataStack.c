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

#include "dataStack.h"
#include "dataQueue.h"

void stack_reset(Stack* stack) {
    stack->sp = (uint8_t*)arg_getContent(stack->stack_pyload);
    stack->sp_size = (int16_t*)arg_getContent(stack->stack_size_array);
    stack->top = 0;
}

int32_t stack_init(Stack* stack) {
    stack->stack_pyload =
        arg_setContent(NULL, NULL, PIKA_CONFIG_STACK_BUFF_SIZE);
    stack->stack_size_array =
        arg_setContent(NULL, NULL, PIKA_CONFIG_STACK_BUFF_SIZE / 4);
    stack_reset(stack);
    return 0;
};

void stack_pushSize(Stack* stack, int16_t size) {
    *(stack->sp_size) = size;
    stack->sp_size++;
}

int16_t stack_popSize(Stack* stack) {
    stack->sp_size--;
    return *(stack->sp_size);
}

int32_t stack_deinit(Stack* stack) {
    arg_deinit(stack->stack_pyload);
    arg_deinit(stack->stack_size_array);
    return 0;
}

void stack_pushPyload(Stack* stack, uint8_t* content, size_t size) {
    __platform_memcpy(stack->sp, content, size);
    stack->sp += size;
}

uint8_t* stack_popPyload(Stack* stack, size_t size) {
    stack->sp -= size;
    return stack->sp;
}

int32_t stack_pushArg(Stack* stack, Arg* arg) {
    stack->top++;
    int8_t size = arg_getTotleSize(arg);
    stack_pushSize(stack, size);
    stack_pushPyload(stack, arg, size);
    arg_deinit(arg);
    return 0;
}

int32_t stack_pushStr(Stack* stack, char* str) {
    Arg* newArg = arg_setStr(NULL, "", str);
    return stack_pushArg(stack, newArg);
}

Arg* stack_popArg(Stack* stack) {
    if (stack->top == 0) {
        return NULL;
    }
    stack->top--;
    int16_t size = stack_popSize(stack);
    Arg* res = arg_copy((Arg*)stack_popPyload(stack, size));
    return res;
}

char* stack_popStr(Stack* stack, char* outBuff) {
    Arg* arg = stack_popArg(stack);
    strcpy(outBuff, arg_getStr(arg));
    arg_deinit(arg);
    return outBuff;
}

int8_t stack_getTop(Stack* stack) {
    return stack->top;
}
