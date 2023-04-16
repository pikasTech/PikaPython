/*
 * This file is part of the PikaScript project.
 * http://github.com/pikastech/pikapython
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
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __PIKA_VM__H
#define __PIKA_VM__H
#include "PikaObj.h"
#include "dataQueue.h"
#include "dataQueueObj.h"
#include "dataStack.h"
#if PIKA_SETJMP_ENABLE
#include <setjmp.h>
#endif

enum InstructIndex {
#define __INS_ENUM
#include "__instruction_table.h"
    __INSTRUCTION_CNT,
    __INSTRUCTION_INDEX_MAX = 0xFFFF,
    __INSTRUCTION_UNKNOWN = 0xFFFF,
};

typedef enum {
    VM_JMP_EXIT = -999,
    VM_JMP_CONTINUE = -997,
    VM_JMP_BREAK = -998,
    VM_JMP_RAISE = -996,
} VM_JMP;

#define VM_PC_EXIT (-99999)

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
    int8_t error_code;
    uint8_t line_error_code;
    uint8_t try_error_code;
    uint32_t ins_cnt;
    PIKA_BOOL in_super;
    uint8_t super_invoke_deepth;
    RunState* run_state;
    PIKA_BOOL ireg[PIKA_REGIST_SIZE];
};

typedef struct {
    int8_t n_positional;
    int8_t n_positional_got;
    int8_t n_default;
    int8_t n_arg;
    int8_t i_arg;
    int8_t n_input;
    PIKA_BOOL is_vars;
    PIKA_BOOL is_keys;
    PIKA_BOOL is_default;
    ArgType method_type;
    PikaTuple* tuple;
    PikaDict* kw;
    PikaDict* kw_keys;
    char* var_tuple_name;
    char* kw_dict_name;
    char* type_list;
} FunctionArgsInfo;

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

typedef struct EventCQ {
    uint32_t id[PIKA_EVENT_LIST_SIZE];
    Arg* data[PIKA_EVENT_LIST_SIZE];
    PikaEventListener* lisener[PIKA_EVENT_LIST_SIZE];
    Arg* res[PIKA_EVENT_LIST_SIZE];
    int head;
    int tail;
} EventCQ;

#if PIKA_SETJMP_ENABLE
typedef struct JmpBufCQ {
    jmp_buf* buf[PIKA_JMP_BUF_LIST_SIZE];
    int head;
    int tail;
} JmpBufCQ;
#endif

typedef struct VMSignal VMSignal;
struct VMSignal {
    VM_SIGNAL_CTRL signal_ctrl;
    int vm_cnt;
#if PIKA_EVENT_ENABLE
    EventCQ cq;
    int event_pickup_cnt;
    int event_thread_inited;
#endif
};

typedef Arg* (*VM_instruct_handler)(PikaObj* self,
                                    VMState* vm,
                                    char* data,
                                    Arg* arg_ret_reg);

typedef struct VMInstruction VMInstruction;
struct VMInstruction {
    VM_instruct_handler handler;
    const char* op_str;
    uint16_t op_idx;
    uint16_t op_str_len : 4;
    uint16_t : 12;
};

typedef struct VMInstructionSet VMInstructionSet;
struct VMInstructionSet {
    const VMInstruction* instructions;
    uint16_t count;
    uint16_t signature;
    uint16_t op_idx_start;
    uint16_t op_idx_end;
};

VMParameters* pikaVM_run(PikaObj* self, char* pyLine);
VMParameters* pikaVM_runAsm(PikaObj* self, char* pikaAsm);
VMParameters* pikaVM_runByteCodeFrame(PikaObj* self,
                                      ByteCodeFrame* byteCode_frame);

static inline int instructUnit_getBlockDeepth(InstructUnit* self) {
    return self->deepth & 0x0F;
}

static inline int instructUnit_getInvokeDeepth(InstructUnit* self) {
    return self->deepth >> 4;
}

static inline enum InstructIndex instructUnit_getInstructIndex(
    InstructUnit* self) {
    return (enum InstructIndex)(self->isNewLine_instruct & 0x7F);
}

static inline int instructUnit_getConstPoolIndex(InstructUnit* self) {
    return self->const_pool_index;
}

static inline int instructUnit_getIsNewLine(InstructUnit* self) {
    return self->isNewLine_instruct >> 7;
}

static inline void instructUnit_setBlockDeepth(InstructUnit* self, int val) {
    self->deepth |= (0x0F & val);
}

static inline void instructUnit_setConstPoolIndex(InstructUnit* self, int val) {
    self->const_pool_index = val;
}

static inline void instructUnit_setInvokeDeepth(InstructUnit* self, int val) {
    self->deepth |= ((0x0F & val) << 4);
}

static inline void instructUnit_setInstruct(InstructUnit* self, int val) {
    self->isNewLine_instruct |= (0x7F & val);
}

static inline void instructUnit_setIsNewLine(InstructUnit* self, int val) {
    self->isNewLine_instruct |= ((0x01 & val) << 7);
}

InstructUnit* New_instructUnit(uint8_t data_size);
void instructUnit_deinit(InstructUnit* self);

enum InstructIndex pikaVM_getInstructFromAsm(char* line);

void constPool_init(ConstPool* self);
void constPool_deinit(ConstPool* self);
void constPool_append(ConstPool* self, char* content);

static inline void* constPool_getStart(ConstPool* self) {
    return self->content_start;
}

static inline int constPool_getLastOffset(ConstPool* self) {
    return self->size;
}

static inline char* constPool_getByOffset(ConstPool* self, int offset) {
    return (char*)((uintptr_t)constPool_getStart(self) + (uintptr_t)offset);
}

static inline char* VMState_getConstWithInstructUnit(VMState* vm,
                                                     InstructUnit* ins_unit) {
    return constPool_getByOffset(&(vm->bytecode_frame->const_pool),
                                 instructUnit_getConstPoolIndex(ins_unit));
}

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

static inline InstructUnit* instructArray_getStart(InstructArray* self) {
    return (InstructUnit*)self->content_start;
}

static inline size_t instructArray_getSize(InstructArray* self) {
    return (size_t)self->size;
}

static inline int VMState_getInstructArraySize(VMState* vm) {
    return instructArray_getSize(&(vm->bytecode_frame->instruct_array));
}

static inline InstructUnit* instructArray_getByOffset(InstructArray* self,
                                                      int offset) {
    return (InstructUnit*)((uintptr_t)instructArray_getStart(self) +
                           (uintptr_t)offset);
}

static inline InstructUnit* VMState_getInstructUnitWithOffset(VMState* vm,
                                                              int offset) {
    return instructArray_getByOffset(&(vm->bytecode_frame->instruct_array),
                                     vm->pc + offset);
}

static inline InstructUnit* VMState_getInstructNow(VMState* vm) {
    return instructArray_getByOffset(&(vm->bytecode_frame->instruct_array),
                                     vm->pc);
}

void byteCodeFrame_print(ByteCodeFrame* self);

static inline size_t instructUnit_getSize(void) {
    return (size_t)sizeof(InstructUnit);
}

uint16_t constPool_getOffsetByData(ConstPool* self, char* data);
void instructArray_printWithConst(InstructArray* self, ConstPool* const_pool);
void constPool_update(ConstPool* self);
void instructArray_update(InstructArray* self);
void constPool_printAsArray(ConstPool* self);
void instructArray_printAsArray(InstructArray* self);
void byteCodeFrame_loadByteCode(ByteCodeFrame* self, uint8_t* bytes);
void byteCodeFrame_printAsArray(ByteCodeFrame* self);
void byteCodeFrame_init(ByteCodeFrame* self);
PIKA_BOOL pikaVM_registerInstructionSet(VMInstructionSet* ins_set);
VMParameters* pikaVM_runByteCode(PikaObj* self, const uint8_t* bytecode);
VMParameters* pikaVM_runByteCodeInconstant(PikaObj* self, uint8_t* bytecode);
Arg* pikaVM_runByteCodeReturn(PikaObj* self,
                              const uint8_t* bytecode,
                              char* returnName);
Arg* _do_pikaVM_runByteCodeReturn(PikaObj* self,
                                  VMParameters* locals,
                                  VMParameters* globals,
                                  uint8_t* bytecode,
                                  RunState* run_state,
                                  PIKA_BOOL is_const_bytecode,
                                  char* return_name);
InstructUnit* instructArray_getNow(InstructArray* self);
InstructUnit* instructArray_getNext(InstructArray* self);
VMParameters* pikaVM_runSingleFile(PikaObj* self, char* filename);
VMParameters* pikaVM_runByteCodeFile(PikaObj* self, char* filename);
Arg* obj_runMethodArg(PikaObj* self, PikaObj* method_args_obj, Arg* method_arg);
PikaObj* pikaVM_runFile(PikaObj* self, char* file_name);
Arg* _vm_slice(VMState* vm,
               PikaObj* self,
               Arg* end,
               Arg* obj,
               Arg* start,
               int step);
VMParameters* _do_pikaVM_runByteCode(PikaObj* self,
                                     VMParameters* locals,
                                     VMParameters* globals,
                                     uint8_t* bytecode,
                                     RunState* run_state,
                                     PIKA_BOOL is_const_bytecode);
void _do_byteCodeFrame_loadByteCode(ByteCodeFrame* self,
                                    uint8_t* bytes,
                                    PIKA_BOOL is_const);
Arg* _vm_get(VMState* vm, PikaObj* self, Arg* key, Arg* obj);
void __vm_List_append(PikaObj* self, Arg* arg);
void __vm_List___init__(PikaObj* self);
void __vm_Dict_set(PikaObj* self, Arg* arg, char* key);
void __vm_Dict___init__(PikaObj* self);
VM_SIGNAL_CTRL VMSignal_getCtrl(void);
void pks_vm_exit(void);
void pks_vmSignal_setCtrlElear(void);
PIKA_RES __eventListener_popEvent(PikaEventListener** lisener_p,
                                  uint32_t* id,
                                  Arg** signal,
                                  int* head);
PIKA_RES __eventListener_pushEvent(PikaEventListener* lisener,
                                   uint32_t eventId,
                                   Arg* eventData);
int _VMEvent_getVMCnt(void);
void __VMEvent_pickupEvent(char* info);
void _pikaVM_yield(void);
int _VM_lock_init(void);
int _VM_is_first_lock(void);

#define _VMEvent_pickupEvent() __VMEvent_pickupEvent(__FILE__)

typedef struct {
    PikaObj* lreg[PIKA_REGIST_SIZE];
} VMLocals;

#endif

#ifdef __cplusplus
}
#endif
