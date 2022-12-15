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

uint8_t* stack_popPyload(Stack* stack, int32_t size);

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

Arg* stack_checkArg(Stack* stack, int index) {
    if (stack->top - index <= 0) {
        return NULL;
    }
    int sp_offset = 0;
    int32_t size = 0;
    for (int i = 1; i <= index + 1; i++) {
        size = stack->sp_size[-i];
        if (size == -1) {
            sp_offset -= sizeof(Arg*);
        } else {
            sp_offset -= size;
        }
    }
    if (size == -1) {
        return *(Arg**)(stack->sp + sp_offset);
    }
    return (Arg*)(stack->sp + sp_offset);
}

int32_t stack_deinit(Stack* stack) {
    while (stack->top > 0) {
        int32_t size = stack_popSize(stack);
        uint8_t* pyload = stack_popPyload(stack, size);
        stack->top--;
        if (size == -1) {
            arg_deinit(*(Arg**)pyload);
        }
    }
    arg_deinit(stack->stack_pyload);
    arg_deinit(stack->stack_size_array);
    return 0;
}

void stack_pushPyload(Stack* stack,
                      uint8_t* in,
                      size_t size,
                      PIKA_BOOL is_sample_copy) {
    size_t stack_size_after_push =
        size + (stack->sp - arg_getContent(stack->stack_pyload));
    if (stack_size_after_push > stack->stack_totle_size) {
        __platform_printf(
            "OverflowError: pika VM stack overflow, please use bigger "
            "PIKA_STACK_BUFF_SIZE\r\n");
        __platform_printf("Info: stack size request: %d\r\n",
                          (int)stack_size_after_push);
        __platform_printf("Info: stack size now: %d\r\n",
                          (int)stack->stack_totle_size);
        while (1) {
        }
    }
    Arg* top = (Arg*)stack->sp;
    if (is_sample_copy) {
        __platform_memcpy(top, in, size);
    } else {
        __platform_memcpy(top, in, sizeof(Arg));
        __platform_memcpy(top->content, ((Arg*)in)->_.buffer,
                          size - sizeof(Arg));
        /* transfer to serialized form */
        arg_setSerialized(top, PIKA_TRUE);
    }
    stack->sp += size;
}

uint8_t* stack_popPyload(Stack* stack, int32_t size) {
    if (size == -1) {
        size = sizeof(void*);
    }
    stack->sp -= size;
    return stack->sp;
}

static int32_t _stack_pushArg(Stack* stack, Arg* arg, PIKA_BOOL is_alloc) {
    PIKA_BOOL is_big_arg = PIKA_FALSE;
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

    if (arg_isSerialized(arg)) {
        is_big_arg = PIKA_TRUE;
    }

    if (is_big_arg) {
        /* push a pointer to this arg */
        stack_pushSize(stack, -1);
        stack_pushPyload(stack, (uint8_t*)&arg, sizeof(Arg*), 1);
    } else {
        stack_pushSize(stack, size);
        stack_pushPyload(stack, (uint8_t*)arg, size, arg_isSerialized(arg));
    }

    if (is_big_arg) {
        return 0;
    }
    if (is_alloc) {
        arg_deinit(arg);
        return 0;
    }
    arg_deinitHeap(arg);
    return 0;
}

int32_t stack_pushArg(Stack* stack, Arg* arg) {
    pika_assert(arg != NULL);
    if (arg_isSerialized(arg)) {
        return _stack_pushArg(stack, arg, PIKA_TRUE);
    }
    return _stack_pushArg(stack, arg, PIKA_FALSE);
}

int32_t stack_pushStr(Stack* stack, char* str) {
    Arg* newArg = arg_newStr(str);
    return stack_pushArg(stack, newArg);
}

Arg* _stack_popArg(Stack* stack, Arg* arg_dict, PIKA_BOOL is_alloc) {
    PIKA_BOOL is_big_arg = PIKA_FALSE;
    if (stack->top == 0) {
        return NULL;
    }
    stack->top--;
    int32_t size = stack_popSize(stack);
    if (size == -1) {
        is_big_arg = PIKA_TRUE;
        size = sizeof(Arg*);
    }
    Arg* arg = NULL;
    if (is_big_arg) {
        arg = *(Arg**)stack_popPyload(stack, size);
    } else {
        arg = (Arg*)stack_popPyload(stack, size);
        if (is_alloc) {
            arg = arg_copy(arg);
        } else {
            arg = arg_copy_noalloc(arg, arg_dict);
        }
    }

    ArgType type = arg_getType(arg);
    /* decrase ref_cnt */
    if (argType_isObject(type)) {
        obj_refcntDec((PikaObj*)arg_getPtr(arg));
    }
    pika_assert(arg->flag < ARG_FLAG_MAX);
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
