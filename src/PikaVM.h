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

#ifndef __PIKA__VM__H
#define __PIKA__VM__H
#include "PikaObj.h"

enum Instruct {
#define __INS_ENUM
#include "__instruction_table.cfg"
    __INSTRCUTION_CNT,
};

typedef struct ByteCodeUnit_t {
    uint8_t block_deepth;
    uint8_t invoke_deepth;
    uint8_t is_new_line;
    enum Instruct instruct;
    uint8_t data_size;
    uint8_t data[];
} ByteCodeUnit;

VMParameters* pikaVM_run(PikaObj* self, char* pyLine);
VMParameters* pikaVM_runAsm(PikaObj* self, char* pikaAsm);

#define byteCodeUnit_getBlockDeepth(self) ((self)->block_deepth)
#define byteCodeUnit_getInvokeDeepth(self) ((self)->invoke_deepth)
#define byteCodeUnit_getDataSize(self) ((self)->data_size)
#define byteCodeUnit_getData(self) ((self)->data)
#define byteCodeUnit_getInstruct(self) ((self)->instruct)
#define byteCodeUnit_getIsNewLine(self) ((self)->is_new_line)


#define byteCodeUnit_setBlockDeepth(self, val)          \
    do {                                            \
        ((self)->block_deepth) = val;                                       \
    }while (0)

#define byteCodeUnit_setInvokeDeepth(self, val)         \
    do {                                            \
        ((self)->invoke_deepth) = val;                                       \
    }while (0)

#define byteCodeUnit_setDataSize(self, val)         \
    do {                                            \
        ((self)->data_size) = val;                                       \
    }while (0)

#define byteCodeUnit_setData(self, val)         \
    do {                                            \
        ((self)->data) = val;                                       \
    }while (0)

#define byteCodeUnit_setInstruct(self, val)         \
    do {                                            \
        ((self)->instruct) = val;                                       \
    }while (0)

#define byteCodeUnit_setIsNewLine(self, val)            \
    do {                                            \
        ((self)->is_new_line) = val;                                       \
    }while (0)

#endif
