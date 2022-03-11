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
    uint8_t deepth;
    uint8_t isNewLine_instruct;
    uint16_t const_pool_index;
} ByteCodeUnit;

VMParameters* pikaVM_run(PikaObj* self, char* pyLine);
VMParameters* pikaVM_runAsm(PikaObj* self, char* pikaAsm);

#define byteCodeUnit_getBlockDeepth(self) (((self)->deepth) & 0x0F)
#define byteCodeUnit_getInvokeDeepth(self) (((self)->deepth) >> 4)
// #define byteCodeUnit_getDataSize(self) (strGetSize((char*)(self)->data))
// #define byteCodeUnit_getData(self) (char*)((self)->data)
#define byteCodeUnit_getInstruct(self) ((self)->isNewLine_instruct & 0x7F)
#define byteCodeUnit_getConstPoolIndex(self) ((self)->const_pool_index)
#define byteCodeUnit_getIsNewLine(self) ((self)->isNewLine_instruct >> 7)

#define byteCodeUnit_setBlockDeepth(self, val) \
    do {                                       \
        ((self)->deepth) |= (0x0F & val);      \
    } while (0)

#define byteCodeUnit_setConstPoolIndex(self, val) \
    do {                                          \
        ((self)->const_pool_index = val);         \
    } while (0)

#define byteCodeUnit_setInvokeDeepth(self, val)  \
    do {                                         \
        ((self)->deepth) |= ((0x0F & val) << 4); \
    } while (0)

/*
 #define byteCodeUnit_setData(self, val)                            \
     do {                                                           \
         __platform_memcpy((self)->data, val, strGetSize(val) + 1); \
     } while (0)
*/

#define byteCodeUnit_setInstruct(self, val)           \
    do {                                              \
        ((self)->isNewLine_instruct) |= (0x7F & val); \
    } while (0)

#define byteCodeUnit_setIsNewLine(self, val)                 \
    do {                                                     \
        ((self)->isNewLine_instruct) |= ((0x01 & val) << 7); \
    } while (0)

ByteCodeUnit* New_byteCodeUnit(uint8_t data_size);
void byteCodeUnit_deinit(ByteCodeUnit* self);

/*
 #define __get_alined_size(size) (((((size)-1) / 4) + 1) * 4)
 #define byteCodeUnit_getTotleSize_withDataSize(data_size) \
  (__get_alined_size(sizeof(ByteCodeUnit) + data_size + 1))
 #define byteCodeUnit_getTotleSize(self) \
  (byteCodeUnit_getTotleSize_withDataSize(byteCodeUnit_getDataSize(self)))
*/

enum Instruct pikaVM_getInstructFromAsm(char* line);

#endif
