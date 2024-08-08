/*
 * This file is part of the PikaScript project.
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

typedef struct {
    int8_t n_positional;
    int8_t n_positional_got;
    int8_t n_default;
    int8_t n_arg;
    int8_t i_arg;
    int8_t n_input;
    pika_bool is_vars;
    pika_bool is_keys;
    pika_bool is_default;
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
    uint32_t num;
    PikaVMFrame* vm;
};

typedef enum VM_SIGNAL_CTRL {
    VM_SIGNAL_CTRL_NONE = 0,
    VM_SIGNAL_CTRL_EXIT,
} VM_SIGNAL_CTRL;

typedef union _EventDataType {
    int signal;
    Arg* arg;
} EventDataType;

typedef struct PikaArgEventQueue {
    uint32_t id[PIKA_EVENT_LIST_SIZE];
    EventDataType data[PIKA_EVENT_LIST_SIZE];
    PikaEventListener* listener[PIKA_EVENT_LIST_SIZE];
    Arg* res[PIKA_EVENT_LIST_SIZE];
    int head;
    int tail;
} PikaEventQueue;

#if PIKA_SETJMP_ENABLE
typedef struct JmpBufCQ {
    jmp_buf* buf[PIKA_JMP_BUF_LIST_SIZE];
    int head;
    int tail;
} JmpBufCQ;
#endif

typedef struct VMState VMState;
struct VMState {
    VM_SIGNAL_CTRL signal_ctrl;
    int vm_cnt;
#if PIKA_EVENT_ENABLE
    PikaEventQueue cq;
    PikaEventQueue sq;
    int event_pickup_cnt;
    pika_platform_thread_t* event_thread;
    pika_bool event_thread_exit;
    pika_bool event_thread_exit_done;
#endif
#if PIKA_DEBUG_BREAK_POINT_MAX > 0
    Hash break_module_hash[PIKA_DEBUG_BREAK_POINT_MAX];
    uint32_t break_point_pc[PIKA_DEBUG_BREAK_POINT_MAX];
    int break_point_cnt;
#endif
};

typedef Arg* (*VM_instruct_handler)(PikaObj* self,
                                    PikaVMFrame* vm,
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
VMParameters* _pikaVM_runByteCodeFrame(PikaObj* self,
                                       ByteCodeFrame* byteCode_frame,
                                       pika_bool in_repl);
VMParameters* _pikaVM_runByteCodeFrameGlobals(PikaObj* self,
                                              PikaObj* globals,
                                              ByteCodeFrame* byteCode_frame,
                                              pika_bool in_repl);
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

static inline char* PikaVMFrame_getConstWithInstructUnit(
    PikaVMFrame* vm,
    InstructUnit* ins_unit) {
    return constPool_getByOffset(&(vm->bytecode_frame->const_pool),
                                 instructUnit_getConstPoolIndex(ins_unit));
}

pika_bool PikaVMFrame_checkBreakPoint(PikaVMFrame* vm);

typedef struct {
    PikaObj* globals;
    pika_bool in_repl;
    char* module_name;
} pikaVM_run_ex_cfg;

char* constPool_getNow(ConstPool* self);
char* constPool_getNext(ConstPool* self);
char* constPool_getByIndex(ConstPool* self, uint16_t index);
void constPool_print(ConstPool* self);

void byteCodeFrame_init(ByteCodeFrame* bf);
void byteCodeFrame_deinit(ByteCodeFrame* bf);
void byteCodeFrame_setName(ByteCodeFrame* self, char* name);
size_t byteCodeFrame_getSize(ByteCodeFrame* bf);
InstructUnit* byteCodeFrame_findInstructUnit(ByteCodeFrame* bcframe,
                                             int32_t iPcStart,
                                             enum InstructIndex index,
                                             int32_t* iOffset_p,
                                             pika_bool bIsForward);
InstructUnit* byteCodeFrame_findInsUnitBackward(ByteCodeFrame* bcframe,
                                                int32_t pc_start,
                                                enum InstructIndex index,
                                                int32_t* p_offset);
InstructUnit* byteCodeFrame_findInsForward(ByteCodeFrame* bcframe,
                                           int32_t pc_start,
                                           enum InstructIndex index,
                                           int32_t* p_offset);
Hash byteCodeFrame_getNameHash(ByteCodeFrame* bcframe);
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

static inline int PikaVMFrame_getInstructArraySize(PikaVMFrame* vm) {
    return instructArray_getSize(&(vm->bytecode_frame->instruct_array));
}

static inline InstructUnit* instructArray_getByOffset(InstructArray* self,
                                                      int offset) {
    return (InstructUnit*)((uintptr_t)instructArray_getStart(self) +
                           (uintptr_t)offset);
}

static inline InstructUnit* PikaVMFrame_getInstructUnitWithOffset(
    PikaVMFrame* vm,
    int offset) {
    return instructArray_getByOffset(&(vm->bytecode_frame->instruct_array),
                                     vm->pc + offset);
}

static inline InstructUnit* PikaVMFrame_getInstructNow(PikaVMFrame* vm) {
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
pika_bool pikaVM_registerInstructionSet(VMInstructionSet* ins_set);
VMParameters* pikaVM_runByteCode(PikaObj* self, const uint8_t* bytecode);
VMParameters* pikaVM_runByteCodeInconstant(PikaObj* self, uint8_t* bytecode);
Arg* pikaVM_runByteCodeReturn(PikaObj* self,
                              const uint8_t* bytecode,
                              char* returnName);
Arg* pikaVM_runByteCode_exReturn(PikaObj* self,
                                 VMParameters* locals,
                                 VMParameters* globals,
                                 uint8_t* bytecode,
                                 PikaVMThread* vm_thread,
                                 pika_bool is_const_bytecode,
                                 char* return_name);

int pikaVMThread_init(PikaVMThread* state, uint64_t thread_id);
PikaVMThread* pikaVMThread_require(void);
void pikaVMThread_delete(void);
int pikaVMThread_pushError(PikaVMThread* state, PikaVMError* error);
PikaVMError* pikaVMThread_popError(PikaVMThread* state);
int pikaVMFrame_checkErrorCode(PikaVMFrame* state);
int pikaVMFrame_checkErrorStack(PikaVMFrame* vm);
int pikaVMThread_clearErrorStack(PikaVMThread* state);
int pikaVMThread_clearExceptionStack(PikaVMThread* vmThread);
int pikaVMFrame_checkExceptionStack(PikaVMFrame* vm);
int pikaVMThread_convertExceptionStack(PikaVMThread* vmThread);
PikaVMError* pikaVMThread_getErrorCurrent(PikaVMThread* state);
InstructUnit* instructArray_getNow(InstructArray* self);
InstructUnit* instructArray_getNext(InstructArray* self);
VMParameters* pikaVM_runSingleFile(PikaObj* self, char* filename);
VMParameters* pikaVM_runByteCodeFile(PikaObj* self, char* filename);
Arg* obj_runMethodArg(PikaObj* self, PikaObj* method_args_obj, Arg* method_arg);
PikaObj* pikaVM_runFile(PikaObj* self, char* file_name);
Arg* _vm_slice(PikaVMFrame* vm,
               PikaObj* self,
               Arg* end,
               Arg* obj,
               Arg* start,
               int step);

typedef struct {
    VMParameters* locals;
    VMParameters* globals;
    char* name;
    PikaVMThread* vm_thread;
    pika_bool is_const_bytecode;
} pikaVM_runBytecode_ex_cfg;

VMParameters* pikaVM_runByteCode_ex(PikaObj* self,
                                    uint8_t* bytecode,
                                    pikaVM_runBytecode_ex_cfg* cfg);

void _do_byteCodeFrame_loadByteCode(ByteCodeFrame* self,
                                    uint8_t* bytes,
                                    char* name,
                                    pika_bool is_const);
Arg* _vm_get(PikaVMFrame* vm, PikaObj* self, Arg* key, Arg* obj);
VM_SIGNAL_CTRL VMSignal_getCtrl(void);
void pika_vm_exit(void);
void pika_vm_exit_await(void);
void pika_vmSignal_setCtrlClear(void);
PIKA_RES __eventListener_popEvent(PikaEventListener** lisener_p,
                                  uintptr_t* id,
                                  Arg** eventData,
                                  int* signal,
                                  int* head);
PIKA_RES __eventListener_pushEvent(PikaEventListener* lisener,
                                   uintptr_t eventId,
                                   Arg* eventData);
PIKA_RES __eventListener_pushSignal(PikaEventListener* lisener,
                                    uintptr_t eventId,
                                    int signal);
int _VMEvent_getVMCnt(void);
void __VMEvent_pickupEvent(char* info);
void _pikaVM_yield(void);
int _VM_lock_init(void);
int _VM_is_first_lock(void);
PIKA_RES pika_debug_set_break(char* module_name, int pc_break);
void pika_debug_set_trace(PikaObj* self);
PIKA_RES pika_debug_reset_break(char* module_name, int pc_break);
pika_bool pika_debug_check_break_hash(Hash module_hash, int pc_break);
pika_bool pika_debug_check_break(char* module_name, int pc_break);

#define _VMEvent_pickupEvent() __VMEvent_pickupEvent(__FILE__)
#define PIKA_INS(__INS_NAME) _##PIKA_VM_INS_##__INS_NAME

typedef struct {
    PikaObj* lreg[PIKA_REGIST_SIZE];
} VMLocals;

VMParameters* pikaVM_run_ex(PikaObj* self,
                            char* py_lines,
                            pikaVM_run_ex_cfg* cfg);

#endif

#ifdef __cplusplus
}
#endif
