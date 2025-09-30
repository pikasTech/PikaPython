/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "dataStack.h"
#include "PikaObj.h"
#include "dataQueue.h"

void _stack_overflow_handler(Stack* stack, size_t stack_require) {
    pika_platform_printf(
        "OverflowError: pika VM stack overflow, please use bigger "
        "PIKA_STACK_BUFF_SIZE\r\n");
    pika_platform_printf("Info: stack size request: %d\r\n",
                         (int)stack_require);
    pika_platform_printf("Info: stack size now: %d\r\n",
                         (int)stack->stack_totle_size);
    while (1) {
    }
}

uint8_t* stack_popPyload(Stack* stack, int32_t size);

uint8_t* stack_getSpStart(Stack* stack) {
    return (uint8_t*)arg_getContent(stack->stack_pyload);
}

int32_t* stack_getSpSizeStart(Stack* stack) {
    return (int32_t*)arg_getContent(stack->stack_size_array);
}

uint32_t stack_spFree(Stack* stack) {
    return stack->stack_totle_size -
           ((uintptr_t)stack->sp - (uintptr_t)stack_getSpStart(stack));
}

uint32_t stack_spSizeFree(Stack* stack) {
    return stack->stack_totle_size / 4 -
           ((uintptr_t)stack->sp_size - (uintptr_t)stack_getSpSizeStart(stack));
}

void stack_reset(Stack* stack) {
    stack->sp = stack_getSpStart(stack);
    stack->sp_size = stack_getSpSizeStart(stack);
    stack->top = 0;
}

int32_t _stack_init(Stack* stack, size_t stack_size) {
    stack->stack_pyload = arg_setContent(NULL, NULL, stack_size);
    stack->stack_size_array = arg_setContent(NULL, NULL, stack_size / 4);
    stack_reset(stack);
    stack->stack_totle_size = stack_size;
    return 0;
}

int32_t stack_init(Stack* stack) {
    return _stack_init(stack, PIKA_STACK_BUFF_SIZE);
};

void stack_pushSize(Stack* stack, int32_t size) {
    *(stack->sp_size) = size;
    stack->sp_size++;
    if (stack_spSizeFree(stack) < sizeof(int32_t)) {
        _stack_overflow_handler(stack,
                                stack->stack_totle_size + sizeof(int32_t) * 4);
    }
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
                      pika_bool is_sample_copy) {
    size_t stack_require = size + (stack->sp - stack_getSpStart(stack));
    if (stack_require > stack->stack_totle_size) {
        _stack_overflow_handler(stack, stack_require);
    }
    Arg* top = (Arg*)stack->sp;
    if (is_sample_copy) {
        pika_platform_memcpy(top, in, size);
    } else {
        pika_platform_memcpy(top, in, sizeof(Arg));
        pika_platform_memcpy(top->content, ((Arg*)in)->_.buffer,
                             size - sizeof(Arg));
        /* transfer to serialized form */
        arg_setSerialized(top, pika_true);
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

static int32_t _stack_pushArg(Stack* stack, Arg* arg, pika_bool is_alloc) {
    pika_bool is_big_arg = pika_false;
    stack->top++;
    size_t size = arg_getTotleSize(arg);
//! if you unsure about the __impl_pikaMalloc, uncomment this to force alignment
#if PIKA_ARG_ALIGN_ENABLE
    /* force alignment to avoid unaligned access */
    size = (size + 4 - 1) & ~(4 - 1);
#endif
    /* add ref_cnt to keep object in stack */
    arg_refcntInc(arg);

    if (arg_isSerialized(arg)) {
        is_big_arg = pika_true;
    }

    if (is_big_arg) {
        /* push a pointer to this arg */
        stack_pushSize(stack, -1);
        stack_pushPyload(stack, (uint8_t*)&arg, sizeof(Arg*), pika_true);
    } else {
        stack_pushSize(stack, size);
        stack_pushPyload(stack, (uint8_t*)arg, size,
                         (pika_bool)arg_isSerialized(arg));
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
    if (arg_isObject(arg)) {
        pika_assert(obj_checkAlive(arg_getPtr(arg)));
    }
    if (arg_isSerialized(arg)) {
        return _stack_pushArg(stack, arg, pika_true);
    }
    return _stack_pushArg(stack, arg, pika_false);
}

int32_t stack_pushStr(Stack* stack, char* str) {
    Arg* newArg = arg_newStr(str);
    return stack_pushArg(stack, newArg);
}

Arg* _stack_popArg(Stack* stack, Arg* arg_dict, pika_bool is_alloc) {
    pika_bool is_big_arg = pika_false;
    if (stack->top == 0) {
        return NULL;
    }
    stack->top--;
    int32_t size = stack_popSize(stack);
    if (size == -1) {
        is_big_arg = pika_true;
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

    /* decrase ref_cnt */
    arg_refcntDec(arg);
    pika_assert(arg->flag < ARG_FLAG_MAX);
    return arg;
}

Arg* stack_popArg_alloc(Stack* stack) {
    return _stack_popArg(stack, NULL, pika_true);
}

Arg* stack_popArg(Stack* stack, Arg* arg_dict) {
    return _stack_popArg(stack, arg_dict, pika_false);
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
