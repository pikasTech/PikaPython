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
#include "PikaObj.h"
#include "dataQueue.h"

void stack_reset(Stack* stack) {
    stack->sp = (uint8_t*)arg_getContent(stack->stack_pyload);
    stack->sp_size = (int32_t*)arg_getContent(stack->stack_size_array);
    stack->top = 0;
}

int32_t stack_init(Stack* stack) {
    stack->stack_pyload = arg_setContent(NULL, NULL, PIKA_STACK_BUFF_SIZE);
    stack->stack_size_array =
        arg_setContent(NULL, NULL, PIKA_STACK_BUFF_SIZE / 4);
    stack_reset(stack);
    stack->stack_totle_size = PIKA_STACK_BUFF_SIZE;
    return 0;
};

void stack_pushSize(Stack* stack, int32_t size) {
    *(stack->sp_size) = size;
    stack->sp_size++;
}

int32_t stack_popSize(Stack* stack) {
    stack->sp_size--;
    return *(stack->sp_size);
}

int32_t stack_deinit(Stack* stack) {
    arg_deinit(stack->stack_pyload);
    arg_deinit(stack->stack_size_array);
    return 0;
}

void stack_pushPyload(Stack* stack, Arg* content, size_t size) {
    size_t stack_size_after_push =
        size + (stack->sp - arg_getContent(stack->stack_pyload));
    if (stack_size_after_push > stack->stack_totle_size) {
        __platform_printf(
            "OverflowError: pika VM stack overflow, please use bigger "
            "PIKA_STACK_BUFF_SIZE\r\n");
        __platform_printf("Info: stack size request: %d\r\n",
                          stack_size_after_push);
        __platform_printf("Info: stack size now: %d\r\n",
                          stack->stack_totle_size);
        while (1) {
        }
    }
    if (content->serialized) {
        __platform_memcpy(stack->sp, content, size);
    } else {
        __platform_memcpy(stack->sp, content, sizeof(Arg));
        __platform_memcpy(stack->sp + sizeof(Arg), content->buffer,
                          size - sizeof(Arg));
        /* transfer to serialized form */
        ((Arg*)stack->sp)->serialized = PIKA_TRUE;
    }
    stack->sp += size;
}

uint8_t* stack_popPyload(Stack* stack, size_t size) {
    stack->sp -= size;
    return stack->sp;
}

static int32_t _stack_pushArg(Stack* stack, Arg* arg, PIKA_BOOL is_alloc) {
    stack->top++;
    size_t size = arg_getTotleSize(arg);

//! if you unsure about the __impl_pikaMalloc, uncomment this to force alignment
#if PIKA_ARG_ALIGN_ENABLE
    /* force alignment to avoid unaligned access */
    size = (size + 4 - 1) & ~(4 - 1);
#endif
    /* add ref_cnt to keep object in stack */
    if (argType_isObject(arg_getType(arg))) {
        obj_refcntInc((PikaObj*)arg_getPtr(arg));
    }
    stack_pushSize(stack, size);
    stack_pushPyload(stack, arg, size);
    if (is_alloc) {
        arg_deinit(arg);
    } else {
        arg_deinitHeap(arg);
    }
    return 0;
}

int32_t stack_pushArg(Stack* stack, Arg* arg) {
    if (arg->serialized) {
        return _stack_pushArg(stack, arg, PIKA_TRUE);
    }
    return _stack_pushArg(stack, arg, PIKA_FALSE);
}

int32_t stack_pushStr(Stack* stack, char* str) {
    Arg* newArg = arg_newStr(str);
    return stack_pushArg(stack, newArg);
}

Arg* _stack_popArg(Stack* stack, Arg* arg_dict, PIKA_BOOL is_alloc) {
    if (stack->top == 0) {
        return NULL;
    }
    stack->top--;
    int32_t size = stack_popSize(stack);
    Arg* arg = NULL;
    if (is_alloc) {
        arg = arg_copy((Arg*)stack_popPyload(stack, size));
    } else {
        arg = arg_copy_noalloc((Arg*)stack_popPyload(stack, size), arg_dict);
    }
    ArgType type = arg_getType(arg);
    /* decrase ref_cnt */
    if (argType_isObject(type)) {
        obj_refcntDec((PikaObj*)arg_getPtr(arg));
    }
    return arg;
}

Arg* stack_popArg_alloc(Stack* stack) {
    return _stack_popArg(stack, NULL, PIKA_TRUE);
}

Arg* stack_popArg(Stack* stack, Arg* arg_dict) {
    return _stack_popArg(stack, arg_dict, PIKA_FALSE);
}

char* stack_popStr(Stack* stack, char* outBuff) {
    Arg* arg = stack_popArg_alloc(stack);
    strcpy(outBuff, arg_getStr(arg));
    arg_deinit(arg);
    return outBuff;
}

int32_t stack_getTop(Stack* stack) {
    return stack->top;
}
