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

#ifndef __PIKA_VM__H
#define __PIKA_VM__H
#include "PikaObj.h"
#include "dataQueue.h"
#include "dataQueueObj.h"
#include "dataStack.h"

enum Instruct {
#define __INS_ENUM
#include "__instruction_table.cfg"
    __INSTRCUTION_CNT,
};

typedef enum {
    VM_JMP_EXIT = -999,
    VM_JMP_CONTINUE = -997,
    VM_JMP_BREAK = -998,
    VM_JMP_RAISE = -996,
} VM_JMP;

typedef enum { VM_PC_EXIT = -99999 } VM_PC;

typedef enum {
    TRY_STATE_NONE = 0,
    TRY_STATE_INNER,
} TRY_STATE;

typedef enum {
    TRY_RESULT_NONE = 0,
    TRY_RESULT_RAISE,
} TRY_RESULT;

typedef struct RunState RunState;
struct RunState {
    TRY_STATE try_state;
    TRY_RESULT try_result;
};

typedef struct VMState VMState;
struct VMState {
    VMParameters* locals;
    VMParameters* globals;
    Stack stack;
    int32_t jmp;
    int32_t pc;
    ByteCodeFrame* bytecode_frame;
    uint8_t loop_deepth;
    uint8_t error_code;
    uint8_t line_error_code;
    uint8_t try_error_code;
    uint32_t ins_cnt;
    PIKA_BOOL in_super;
    uint8_t super_invoke_deepth;
    PikaObj* lreg[PIKA_REGIST_SIZE];
    PIKA_BOOL ireg[PIKA_REGIST_SIZE];
    RunState* run_state;
};

typedef struct OperatorInfo OperatorInfo;
struct OperatorInfo {
    char* opt;
    ArgType t1;
    ArgType t2;
    Arg* a1;
    Arg* a2;
    pika_float f1;
    pika_float f2;
    int64_t i1;
    int64_t i2;
    Arg* res;
    int num;
    VMState* vm;
};

typedef enum VM_SIGNAL_CTRL {
    VM_SIGNAL_CTRL_NONE = 0,
    VM_SIGNAL_CTRL_EXIT,
} VM_SIGNAL_CTRL;

typedef struct VMSignal VMSignal;
struct VMSignal {
    VM_SIGNAL_CTRL signal_ctrl;
    int vm_cnt;
};

VMParameters* pikaVM_run(PikaObj* self, char* pyLine);
VMParameters* pikaVM_runAsm(PikaObj* self, char* pikaAsm);
VMParameters* pikaVM_runByteCodeFrame(PikaObj* self,
                                      ByteCodeFrame* byteCode_frame);

#define instructUnit_getBlockDeepth(self) (((self)->deepth) & 0x0F)
#define instructUnit_getInvokeDeepth(self) (((self)->deepth) >> 4)
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

enum Instruct pikaVM_getInstructFromAsm(char* line);

void constPool_init(ConstPool* self);
void constPool_deinit(ConstPool* self);
void constPool_append(ConstPool* self, char* content);

#define constPool_getStart(self) ((self)->content_start)
#define constPool_getLastOffset(self) ((self)->size)
#define constPool_getByOffset(self, offset) \
    (char*)((uintptr_t)constPool_getStart((self)) + (uintptr_t)(offset))

#define VMState_getConstWithInstructUnit(__vm, __ins_unit)        \
    (constPool_getByOffset(&((__vm)->bytecode_frame->const_pool), \
                           instructUnit_getConstPoolIndex(__ins_unit)))

char* constPool_getNow(ConstPool* self);
char* constPool_getNext(ConstPool* self);
char* constPool_getByIndex(ConstPool* self, uint16_t index);
void constPool_print(ConstPool* self);

void byteCodeFrame_init(ByteCodeFrame* bf);
void byteCodeFrame_deinit(ByteCodeFrame* bf);
size_t byteCodeFrame_getSize(ByteCodeFrame* bf);

void instructArray_init(InstructArray* ins_array);
void instructArray_deinit(InstructArray* ins_array);
void instructArray_append(InstructArray* ins_array, InstructUnit* ins_unit);
void instructUnit_init(InstructUnit* ins_unit);
void instructUnit_print(InstructUnit* self);
void instructArray_print(InstructArray* self);

#define instructArray_getStart(InsturctArry_p_self) \
    ((InsturctArry_p_self)->content_start)

#define instructArray_getSize(InsturctArry_p_self) \
    ((size_t)(InsturctArry_p_self)->size)

#define VMState_getInstructArraySize(vm) \
    (instructArray_getSize(&((vm)->bytecode_frame->instruct_array)))

#define instructArray_getByOffset(__self, __offset)                \
    ((InstructUnit*)((uintptr_t)instructArray_getStart((__self)) + \
                     (uintptr_t)(__offset)))

#define VMState_getInstructUnitWithOffset(vm, offset)                   \
    (instructArray_getByOffset(&((vm)->bytecode_frame->instruct_array), \
                               (vm)->pc + (offset)))

#define VMState_getInstructNow(vm)                                      \
    (instructArray_getByOffset(&((vm)->bytecode_frame->instruct_array), \
                               (vm)->pc))

void byteCodeFrame_print(ByteCodeFrame* self);

#define instructUnit_getSize(InstructUnit_p_self) ((size_t)sizeof(InstructUnit))

uint16_t constPool_getOffsetByData(ConstPool* self, char* data);
void instructArray_printWithConst(InstructArray* self, ConstPool* const_pool);
void constPool_update(ConstPool* self);
void instructArray_update(InstructArray* self);
void constPool_printAsArray(ConstPool* self);
void instructArray_printAsArray(InstructArray* self);
void byteCodeFrame_loadByteCode(ByteCodeFrame* self, uint8_t* bytes);
void byteCodeFrame_printAsArray(ByteCodeFrame* self);
void byteCodeFrame_init(ByteCodeFrame* self);
VMParameters* pikaVM_runByteCode(PikaObj* self, uint8_t* bytecode);
InstructUnit* instructArray_getNow(InstructArray* self);
InstructUnit* instructArray_getNext(InstructArray* self);
VMParameters* pikaVM_runSingleFile(PikaObj* self, char* filename);
Arg* obj_runMethodArg(PikaObj* self, PikaObj* method_args_obj, Arg* method_arg);
PikaObj* pikaVM_runFile(PikaObj* self, char* file_name);
Arg* __vm_slice(PikaObj* self, Arg* end, Arg* obj, Arg* start, int step);
Arg* __vm_get(PikaObj* self, Arg* key, Arg* obj);
void __vm_List_append(PikaObj* self, Arg* arg);
void __vm_List___init__(PikaObj* self);
void __vm_Dict_set(PikaObj* self, Arg* arg, char* key);
void __vm_Dict___init__(PikaObj* self);
VM_SIGNAL_CTRL VMSignal_getCtrl(void);
void pks_vm_exit(void);
void pks_vmSignal_setCtrlElear(void);

#endif
