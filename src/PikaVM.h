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
#include "dataQueue.h"
#include "dataQueueObj.h"

enum Instruct {
#define __INS_ENUM
#include "__instruction_table.cfg"
    __INSTRCUTION_CNT,
};

typedef struct InstructUnit_t {
    uint8_t deepth;
    uint8_t isNewLine_instruct;
    uint16_t const_pool_index;
} InstructUnit;

typedef struct ConstPool_t {
    Arg* arg_buff;
    uint16_t content_offset_now;
    uint16_t size;
} ConstPool;

typedef struct InstructArray_t {
    Arg* arg_buff;
    uint16_t content_offset_now;
    uint16_t size;
} InstructArray;

typedef struct ByteCodeFrame_t {
    ConstPool const_pool;
    InstructArray instruct_array;
} ByteCodeFrame;

typedef struct VMState_t {
    VMParameters* locals;
    VMParameters* globals;
    Queue* q0;
    Queue* q1;
    int32_t jmp;
    int32_t pc;
    ByteCodeFrame* bytecode_frame;
} VMState;

typedef enum VMConfig_t {
    VMconfig_enableByteCode,
    VMconfig_desableByteCode,
} VMConfig;

VMParameters* pikaVM_run(PikaObj* self, char* pyLine);
VMParameters* pikaVM_runAsm(PikaObj* self, char* pikaAsm);
VMParameters* pikaVM_runByteCodeFrame(PikaObj* self,
                                      ByteCodeFrame* byteCode_frame);

#define instructUnit_getBlockDeepth(self) (((self)->deepth) & 0x0F)
#define instructUnit_getInvokeDeepth(self) (((self)->deepth) >> 4)
// #define instructUnit_getDataSize(self) (strGetSize((char*)(self)->data))
// #define instructUnit_getData(self) (char*)((self)->data)
#define instructUnit_getInstruct(self) \
    ((enum Instruct)((self)->isNewLine_instruct & 0x7F))
#define instructUnit_getConstPoolIndex(self) ((self)->const_pool_index)
#define instructUnit_getIsNewLine(self) ((self)->isNewLine_instruct >> 7)

#define instructUnit_setBlockDeepth(self, val) \
    do {                                       \
        ((self)->deepth) |= (0x0F & (val));    \
    } while (0)

#define instructUnit_setConstPoolIndex(self, val) \
    do {                                          \
        ((self)->const_pool_index = (val));       \
    } while (0)

#define instructUnit_setInvokeDeepth(self, val)    \
    do {                                           \
        ((self)->deepth) |= ((0x0F & (val)) << 4); \
    } while (0)

/*
 #define instructUnit_setData(self, val)                            \
     do {                                                           \
         __platform_memcpy((self)->data, val, strGetSize(val) + 1); \
     } while (0)
*/

#define instructUnit_setInstruct(self, val)             \
    do {                                                \
        ((self)->isNewLine_instruct) |= (0x7F & (val)); \
    } while (0)

#define instructUnit_setIsNewLine(self, val)                   \
    do {                                                       \
        ((self)->isNewLine_instruct) |= ((0x01 & (val)) << 7); \
    } while (0)

InstructUnit* New_instructUnit(uint8_t data_size);
void instructUnit_deinit(InstructUnit* self);

/*
 #define __get_alined_size(size) (((((size)-1) / 4) + 1) * 4)
 #define instructUnit_getTotleSize_withDataSize(data_size) \
  (__get_alined_size(instructUnit_getSize() + data_size + 1))
 #define instructUnit_getTotleSize(self) \
  (instructUnit_getTotleSize_withDataSize(instructUnit_getDataSize(self)))
*/

enum Instruct pikaVM_getInstructFromAsm(char* line);

void constPool_init(ConstPool* self);
void constPool_deinit(ConstPool* self);
void constPool_append(ConstPool* self, char* content);
char* constPool_getNow(ConstPool* self);
char* constPool_getNext(ConstPool* self);
char* constPool_getByIndex(ConstPool* self, uint16_t index);
uint16_t constPool_getLastOffset(ConstPool* self);
void constPool_print(ConstPool* self);

#define constPool_getByOffset(ConstPoll_p_self, uint16_t_offset) \
    ((char*)(arg_getContent((ConstPoll_p_self)->arg_buff) +      \
             (uintptr_t)(uint16_t_offset)))

void byteCodeFrame_init(ByteCodeFrame* bf);
void byteCodeFrame_deinit(ByteCodeFrame* bf);
size_t byteCodeFrame_getSize(ByteCodeFrame* bf);

void instructArray_init(InstructArray* ins_array);
void instructArray_deinit(InstructArray* ins_array);
void instructArray_append(InstructArray* ins_array, InstructUnit* ins_unit);
void instructUnit_init(InstructUnit* ins_unit);
void instructUnit_print(InstructUnit* self);
void instructArray_print(InstructArray* self);
void byteCodeFrame_print(ByteCodeFrame* self);
InstructUnit* instructArray_getByOffset(InstructArray* self, int32_t offset);

#define instructArray_getByOffset(InstructArray_p_self, int32_t_offset) \
    ((InstructUnit*)(arg_getContent((InstructArray_p_self)->arg_buff) + \
                     (uintptr_t)(int32_t_offset)));

#define instructUnit_getSize(InstructUnit_p_self) ((size_t)sizeof(InstructUnit))
#define instructArray_getSize(InsturctArry_p_self) \
    ((size_t)(InsturctArry_p_self)->size)

#endif
