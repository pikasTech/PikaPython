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

Stack* New_Stack(void) {
    Args* args = New_args(NULL);
    args_setInt(args, "_", 0);
    Stack* stack = args;
    return stack;
}

int32_t stack_deinit(Stack* stack) {
    Args* args = stack;
    args_deinit(args);
    return 0;
}

int32_t stack_pushArg(Stack* stack, Arg* arg) {
    Args* args = stack;
    uint64_t top = args_getInt(args, "_");
    /* add top */
    args_setInt(args, "_", top + 1);
    char buff[11];
    arg = arg_setName(arg, fast_itoa(buff, top));
    return args_setArg(args, arg);
}

int32_t stack_pushStr(Stack* stack, char* str) {
    Arg* newArg = arg_setStr(NULL, "", str);
    return stack_pushArg(stack, newArg);
}

Arg* stack_popArg(Stack* stack) {
    Args* args = stack;
    int64_t top = args_getInt(args, "_") - 1;
    if (top < 0) {
        return NULL;
    }
    char buff[11];
    Arg* topArg = args_getArg(args, fast_itoa(buff, top));
    Arg* res = arg_copy(topArg);
    /* dec top */
    args_setInt(args, "_", top);
    args_removeArg(args, topArg);
    return res;
}

char* stack_popStr(Stack* stack, char* outBuff) {
    Arg* arg = stack_popArg(stack);
    strcpy(outBuff, arg_getStr(arg));
    arg_deinit(arg);
    return outBuff;
}

int8_t stack_getTop(Stack* stack) {
    return args_getInt(stack, "_");
}
