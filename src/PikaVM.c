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

#include "PikaVM.h"
#include "BaseObj.h"
#include "PikaCompiler.h"
#include "PikaObj.h"
#include "PikaParser.h"
#include "PikaPlatform.h"
#include "dataStrs.h"
#if PIKA_MATH_ENABLE
#include <math.h>
#endif

volatile VMSignal PikaVMSignal = {.signal_ctrl = VM_SIGNAL_CTRL_NONE,
                                  .vm_cnt = 0,
#if PIKA_EVENT_ENABLE
                                  .cq =
                                      {
                                          .head = 0,
                                          .tail = 0,
                                          .res = {0},
                                      }
#endif
};

int VMSignal_getVMCnt(void) {
    return PikaVMSignal.vm_cnt;
}

#if PIKA_EVENT_ENABLE
static PIKA_BOOL _cq_isEmpty(volatile EventCQ* cq) {
    return cq->head == cq->tail;
}

static PIKA_BOOL _cq_isFull(volatile EventCQ* cq) {
    return (cq->tail + 1) % PIKA_EVENT_LIST_SIZE == cq->head;
}
#endif

void VMSignal_deinit(void) {
#if !PIKA_EVENT_ENABLE
    __platform_printf("PIKA_EVENT_ENABLE is not enable");
    while (1) {
    };
#else
    for (int i = 0; i < PIKA_EVENT_LIST_SIZE; i++) {
        if (NULL != PikaVMSignal.cq.res[i]) {
            arg_deinit(PikaVMSignal.cq.res[i]);
            PikaVMSignal.cq.res[i] = NULL;
        }
    }
#endif
}

PIKA_RES VMSignal_pushEvent(PikaEventListener* lisener,
                            uint32_t eventId,
                            int eventSignal) {
#if !PIKA_EVENT_ENABLE
    __platform_printf("PIKA_EVENT_ENABLE is not enable");
    while (1) {
    };
#else
    /* push to event_cq_buff */
    if (_cq_isFull(&PikaVMSignal.cq)) {
        return PIKA_RES_ERR_SIGNAL_EVENT_FULL;
    }
    if (PikaVMSignal.cq.res[PikaVMSignal.cq.tail] != NULL) {
        arg_deinit(PikaVMSignal.cq.res[PikaVMSignal.cq.tail]);
        PikaVMSignal.cq.res[PikaVMSignal.cq.tail] = NULL;
    }
    PikaVMSignal.cq.id[PikaVMSignal.cq.tail] = eventId;
    PikaVMSignal.cq.signal[PikaVMSignal.cq.tail] = eventSignal;
    PikaVMSignal.cq.lisener[PikaVMSignal.cq.tail] = lisener;
    PikaVMSignal.cq.tail = (PikaVMSignal.cq.tail + 1) % PIKA_EVENT_LIST_SIZE;
    return PIKA_RES_OK;
#endif
}

PIKA_RES VMSignal_popEvent(PikaEventListener** lisener_p,
                           uint32_t* id,
                           int* signal,
                           int* head) {
#if !PIKA_EVENT_ENABLE
    __platform_printf("PIKA_EVENT_ENABLE is not enable");
    while (1) {
    };
#else
    /* pop from event_cq_buff */
    if (_cq_isEmpty(&PikaVMSignal.cq)) {
        return PIKA_RES_ERR_SIGNAL_EVENT_EMPTY;
    }
    *id = PikaVMSignal.cq.id[PikaVMSignal.cq.head];
    *signal = PikaVMSignal.cq.signal[PikaVMSignal.cq.head];
    *lisener_p = PikaVMSignal.cq.lisener[PikaVMSignal.cq.head];
    *head = PikaVMSignal.cq.head;
    PikaVMSignal.cq.head = (PikaVMSignal.cq.head + 1) % PIKA_EVENT_LIST_SIZE;
    return PIKA_RES_OK;
#endif
}

void VMSignale_pickupEvent(void) {
#if !PIKA_EVENT_ENABLE
    __platform_printf("PIKA_EVENT_ENABLE is not enable");
    while (1) {
    };
#else
    PikaObj* event_lisener;
    uint32_t event_id;
    int event_signal;
    int head;
    if (PIKA_RES_OK ==
        VMSignal_popEvent(&event_lisener, &event_id, &event_signal, &head)) {
        Arg* res =
            __eventLisener_runEvent(event_lisener, event_id, event_signal);
        PikaVMSignal.cq.res[head] = res;
    }
#endif
}

VM_SIGNAL_CTRL VMSignal_getCtrl(void) {
    return PikaVMSignal.signal_ctrl;
}

void pks_vm_exit(void) {
    PikaVMSignal.signal_ctrl = VM_SIGNAL_CTRL_EXIT;
}

void pks_vmSignal_setCtrlElear(void) {
    PikaVMSignal.signal_ctrl = VM_SIGNAL_CTRL_NONE;
}

/* head declare start */
static uint8_t VMState_getInputArgNum(VMState* vm);
static VMParameters* __pikaVM_runByteCodeFrameWithState(
    PikaObj* self,
    VMParameters* locals,
    VMParameters* globals,
    ByteCodeFrame* bytecode_frame,
    uint16_t pc,
    RunState* run_state);

/* head declare end */

static void VMState_setErrorCode(VMState* vm, uint8_t error_code) {
    vm->error_code = error_code;
}

static enum Instruct VMstate_getInstructWithOffset(VMState* vm,
                                                   int32_t offset) {
    return instructUnit_getInstruct(
        VMState_getInstructUnitWithOffset(vm, offset));
}

static int VMState_getBlockDeepthNow(VMState* vm) {
    /* support run byteCode */
    InstructUnit* ins_unit = VMState_getInstructNow(vm);
    return instructUnit_getBlockDeepth(ins_unit);
}

#if !PIKA_NANO_ENABLE
static char* VMState_getConstWithOffset(VMState* vm, int32_t offset) {
    return VMState_getConstWithInstructUnit(
        vm, VMState_getInstructUnitWithOffset(vm, offset));
}
#endif

static int VMState_getInvokeDeepthNow(VMState* vm) {
    /* support run byteCode */
    InstructUnit* ins_unit = VMState_getInstructNow(vm);
    return instructUnit_getInvokeDeepth(ins_unit);
}

static int32_t VMState_getAddrOffsetOfJmpBack(VMState* vm) {
    int offset = 0;
    int loop_deepth = -1;

    /* find loop deepth */
    while (1) {
        offset -= instructUnit_getSize();
        InstructUnit* ins_unit_now =
            VMState_getInstructUnitWithOffset(vm, offset);
        uint16_t invoke_deepth = instructUnit_getInvokeDeepth(ins_unit_now);
        enum Instruct ins = instructUnit_getInstruct(ins_unit_now);
        char* data = VMState_getConstWithInstructUnit(vm, ins_unit_now);
        if ((0 == invoke_deepth) && (JEZ == ins) && data[0] == '2') {
            loop_deepth = instructUnit_getBlockDeepth(ins_unit_now);
            break;
        }
    }

    offset = 0;
    while (1) {
        offset += instructUnit_getSize();
        InstructUnit* ins_unit_now =
            VMState_getInstructUnitWithOffset(vm, offset);
        enum Instruct ins = instructUnit_getInstruct(ins_unit_now);
        char* data = VMState_getConstWithInstructUnit(vm, ins_unit_now);
        int block_deepth_now = instructUnit_getBlockDeepth(ins_unit_now);
        if ((block_deepth_now == loop_deepth) && (JMP == ins) &&
            data[0] == '-' && data[1] == '1') {
            return offset;
        }
    }
}

static int32_t VMState_getAddrOffsetFromJmp(VMState* vm) {
    int offset = 0;
    /* run byte Code */
    InstructUnit* this_ins_unit = VMState_getInstructNow(vm);
    int thisBlockDeepth = instructUnit_getBlockDeepth(this_ins_unit);
    int8_t blockNum = 0;
    int pc_max = (int)VMState_getInstructArraySize(vm);
    if (vm->jmp > 0) {
        offset = 0;
        while (1) {
            offset += instructUnit_getSize();
            /* reach the end */
            if (vm->pc + offset >= pc_max) {
                break;
            }
            this_ins_unit = VMState_getInstructUnitWithOffset(vm, offset);
            if (instructUnit_getIsNewLine(this_ins_unit)) {
                uint8_t blockDeepth =
                    instructUnit_getBlockDeepth(this_ins_unit);
                if (blockDeepth <= thisBlockDeepth) {
                    blockNum++;
                }
            }
            if (blockNum >= vm->jmp) {
                break;
            }
        }
    }
    if (vm->jmp < 0) {
        while (1) {
            offset -= instructUnit_getSize();
            this_ins_unit = VMState_getInstructUnitWithOffset(vm, offset);
            if (instructUnit_getIsNewLine(this_ins_unit)) {
                uint8_t blockDeepth =
                    instructUnit_getBlockDeepth(this_ins_unit);
                if (blockDeepth == thisBlockDeepth) {
                    blockNum--;
                }
            }
            if (blockNum <= vm->jmp) {
                break;
            }
        }
    }
    return offset;
}

static int32_t VMState_getAddrOffsetOfBreak(VMState* vm) {
    int32_t offset = VMState_getAddrOffsetOfJmpBack(vm);
    /* byteCode */
    offset += instructUnit_getSize();
    return offset;
}

#if !PIKA_NANO_ENABLE
static int32_t VMState_getAddrOffsetOfRaise(VMState* vm) {
    int offset = 0;
    InstructUnit* ins_unit_now = VMState_getInstructNow(vm);
    while (1) {
        offset += instructUnit_getSize();
        if (vm->pc + offset >= (int)VMState_getInstructArraySize(vm)) {
            return 0;
        }
        ins_unit_now = VMState_getInstructUnitWithOffset(vm, offset);
        enum Instruct ins = instructUnit_getInstruct(ins_unit_now);
        if (NTR == ins) {
            return offset;
        }
        /* if not found except, return */
        if (RET == ins) {
            return 0;
        }
    }
}
#endif

static int32_t VMState_getAddrOffsetOfContinue(VMState* vm) {
    int32_t offset = VMState_getAddrOffsetOfJmpBack(vm);
    /* byteCode */
    return offset;
}

static void VMState_delLReg(VMState* vm, uint8_t index) {
    PikaObj* obj = vm->lreg[index];
    if (NULL != obj) {
        obj_refcntDec(obj);
        vm->lreg[index] = NULL;
        if (0 == obj_refcntNow(obj)) {
            obj_deinit(obj);
        }
    }
}

static void VMState_initReg(VMState* vm) {
    for (uint8_t i = 0; i < PIKA_REGIST_SIZE; i++) {
        vm->lreg[i] = NULL;
        vm->ireg[i] = PIKA_FALSE;
    }
}

static PIKA_BOOL _checkLReg(char* data) {
    if ((data[0] == '$') && (data[1] == 'l') && (data[2] >= '0') &&
        (data[2] <= '9')) {
        return PIKA_TRUE;
    }
    return PIKA_FALSE;
}

static uint8_t _getLRegIndex(char* data) {
    return data[2] - '0';
}

static void VMState_setLReg(VMState* vm, uint8_t index, PikaObj* obj) {
    obj_refcntInc(obj);
    vm->lreg[index] = obj;
}

typedef Arg* (*VM_instruct_handler)(PikaObj* self,
                                    VMState* vm,
                                    char* data,
                                    Arg* arg_ret_reg);

static Arg* VM_instruction_handler_NON(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    return NULL;
}

Arg* __vm_get(VMState* vm, PikaObj* self, Arg* key, Arg* obj) {
    ArgType type = arg_getType(obj);
    Arg* obj_new = NULL;
    int index = 0;
    if (ARG_TYPE_INT == arg_getType(key)) {
        index = arg_getInt(key);
    }
    if (ARG_TYPE_STRING == type) {
#if PIKA_STRING_UTF8_ENABLE
        PIKA_BOOL is_temp = PIKA_FALSE;
        obj_new = arg_newObj(_arg_to_obj(obj, &is_temp));
        type = arg_getType(obj_new);
#else
        char* str_pyload = arg_getStr(obj);
        char char_buff[] = " ";
        if (index < 0) {
            index = strGetSize(str_pyload) + index;
        }
        char_buff[0] = str_pyload[index];
        return arg_newStr(char_buff);
#endif
    }
    if (ARG_TYPE_BYTES == type) {
        uint8_t* bytes_pyload = arg_getBytes(obj);
        uint8_t byte_buff[] = " ";
        if (index < 0) {
            index = arg_getBytesSize(obj) + index;
        }
        byte_buff[0] = bytes_pyload[index];
        return arg_newBytes(byte_buff, 1);
    }
    if (argType_isObject(type)) {
        PikaObj* arg_obj = NULL;
        if (obj_new != NULL) {
            arg_obj = arg_getPtr(obj_new);
        } else {
            arg_obj = arg_getPtr(obj);
        }
        obj_setArg(arg_obj, "__key", key);
        /* clang-format off */
        PIKA_PYTHON(
        __res = __getitem__(__key)
        )
        /* clang-format on */
        const uint8_t bytes[] = {
            0x0c, 0x00, 0x00, 0x00, /* instruct array size */
            0x10, 0x81, 0x01, 0x00, 0x00, 0x02, 0x07, 0x00, 0x00, 0x04, 0x13,
            0x00,
            /* instruct array */
            0x19, 0x00, 0x00, 0x00, /* const pool size */
            0x00, 0x5f, 0x5f, 0x6b, 0x65, 0x79, 0x00, 0x5f, 0x5f, 0x67, 0x65,
            0x74, 0x69, 0x74, 0x65, 0x6d, 0x5f, 0x5f, 0x00, 0x5f, 0x5f, 0x72,
            0x65, 0x73, 0x00,
            /* const pool */
        };
        if (NULL != vm) {
            _do_pikaVM_runByteCode(arg_obj, arg_obj, arg_obj, (uint8_t*)bytes,
                                   vm->run_state, PIKA_TRUE);
        } else {
            pikaVM_runByteCode(arg_obj, (uint8_t*)bytes);
        }
        Arg* __res = args_getArg(arg_obj->list, "__res");
        Arg* res = NULL;
        if (NULL != __res) {
            res = arg_copy(__res);
        }
        if (NULL != obj_new) {
            arg_deinit(obj_new);
        }
        if (NULL == res) {
            if (NULL != vm) {
                VMState_setErrorCode(vm, PIKA_RES_ERR_ARG_NO_FOUND);
            }
            return arg_newNull();
        }
        return res;
    }
    return arg_newNull();
}

Arg* _vm_slice(VMState* vm,
               PikaObj* self,
               Arg* end,
               Arg* obj,
               Arg* start,
               int step) {
#if PIKA_SYNTAX_SLICE_ENABLE
    /* No interger index only support __getitem__ */
    if (!(arg_getType(start) == ARG_TYPE_INT &&
          arg_getType(end) == ARG_TYPE_INT)) {
        return __vm_get(vm, self, start, obj);
    }

    int start_i = arg_getInt(start);
    int end_i = arg_getInt(end);

    /* __slice__ is equal to __getitem__ */
    if (end_i - start_i == 1) {
        return __vm_get(vm, self, start, obj);
    }

    if (ARG_TYPE_STRING == arg_getType(obj)) {
        char* string_slice(Args * outBuffs, char* str, int start, int end);
        Args buffs = {0};
        Arg* sliced_arg = NULL;
        char* sliced_str =
            string_slice(&buffs, arg_getStr(obj), start_i, end_i);
        if (NULL != sliced_str) {
            sliced_arg = arg_newStr(sliced_str);
        } else {
            sliced_arg = arg_newNull();
        }
        strsDeinit(&buffs);
        return sliced_arg;
    }

    if (ARG_TYPE_BYTES == arg_getType(obj)) {
        size_t len = arg_getBytesSize(obj);
        if (start_i < 0) {
            start_i += len;
        }
        /* magit code, to the end */
        if (end_i == -99999) {
            end_i = len;
        }
        if (end_i < 0) {
            end_i += len;
        }
        Arg* sliced_arg = arg_newBytes(NULL, 0);
        for (int i = start_i; i < end_i; i++) {
            Arg* i_arg = arg_newInt(i);
            Arg* item_arg = __vm_get(vm, self, i_arg, obj);
            uint8_t* bytes_origin = arg_getBytes(sliced_arg);
            size_t size_origin = arg_getBytesSize(sliced_arg);
            Arg* sliced_arg_new = arg_newBytes(NULL, size_origin + 1);
            __platform_memcpy(arg_getBytes(sliced_arg_new), bytes_origin,
                              size_origin);
            __platform_memcpy(arg_getBytes(sliced_arg_new) + size_origin,
                              arg_getBytes(item_arg), 1);
            arg_deinit(sliced_arg);
            sliced_arg = sliced_arg_new;
            arg_deinit(item_arg);
            arg_deinit(i_arg);
        }
        return sliced_arg;
    }

    if (argType_isObject(arg_getType(obj))) {
        PikaObj* arg_obj = arg_getPtr(obj);
        PikaObj* New_PikaStdData_List(Args * args);
        PikaObj* New_PikaStdData_Tuple(Args * args);
        if (arg_obj->constructor == New_PikaStdData_List ||
            arg_obj->constructor == New_PikaStdData_Tuple) {
            PikaObj* sliced_obj = newNormalObj((NewFun)arg_obj->constructor);
            __vm_List___init__(sliced_obj);
            for (int i = start_i; i < end_i; i++) {
                Arg* i_arg = arg_newInt(i);
                Arg* item_arg = __vm_get(vm, self, i_arg, obj);
                __vm_List_append(sliced_obj, item_arg);
                arg_deinit(item_arg);
                arg_deinit(i_arg);
            }
            return arg_newObj(sliced_obj);
        }
    }
    return arg_newNull();
#else
    return __vm_get(vm, self, start, obj);
#endif
}

static Arg* VM_instruction_handler_SLC(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
#if PIKA_SYNTAX_SLICE_ENABLE
    int n_input = VMState_getInputArgNum(vm);
    if (n_input < 2) {
        return arg_newNull();
    }
    if (n_input == 2) {
        Arg* key = stack_popArg_alloc(&vm->stack);
        Arg* obj = stack_popArg_alloc(&vm->stack);
        Arg* res = __vm_get(vm, self, key, obj);
        arg_deinit(key);
        arg_deinit(obj);
        return res;
    }
    if (n_input == 3) {
        Arg* end = stack_popArg_alloc(&vm->stack);
        Arg* start = stack_popArg_alloc(&vm->stack);
        Arg* obj = stack_popArg_alloc(&vm->stack);
        Arg* res = _vm_slice(vm, self, end, obj, start, 1);
        arg_deinit(end);
        arg_deinit(obj);
        arg_deinit(start);
        return res;
    }
    return arg_newNull();
#else
    Arg* key = stack_popArg_alloc(&vm->stack);
    Arg* obj = stack_popArg_alloc(&vm->stack);
    Arg* res = __vm_get(vm, self, key, obj);
    arg_deinit(key);
    arg_deinit(obj);
    return res;
#endif
}

static Arg* VM_instruction_handler_TRY(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    pika_assert(NULL != vm->run_state);
    vm->run_state->try_state = TRY_STATE_INNER;
    return NULL;
}

static Arg* VM_instruction_handler_EXP(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    return NULL;
}

static Arg* VM_instruction_handler_NTR(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    vm->run_state->try_state = TRY_STATE_NONE;
    return NULL;
}

static Arg* VM_instruction_handler_NEW(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    Arg* origin_arg = obj_getArg(vm->locals, data);
    Arg* new_arg = arg_copy(origin_arg);
    arg_setType(origin_arg, ARG_TYPE_OBJECT);
    arg_setType(new_arg, ARG_TYPE_OBJECT_NEW);
    return new_arg;
}

static Arg* _proxy_getattribute(PikaObj* host, char* name) {
#if PIKA_NANO_ENABLE
    return NULL;
#endif
    if ('@' != name[0] && obj_getFlag(host, OBJ_FLAG_PROXY_GETATTRIBUTE)) {
        args_setStr(host->list, "@name", name);
        /* clang-format off */
        PIKA_PYTHON(
        @res = __getattribute__(@name)
        )
        /* clang-format on */
        const uint8_t bytes[] = {
            0x0c, 0x00, 0x00, 0x00, /* instruct array size */
            0x10, 0x81, 0x01, 0x00, 0x00, 0x02, 0x07, 0x00, 0x00, 0x04, 0x18,
            0x00,
            /* instruct array */
            0x1d, 0x00, 0x00, 0x00, /* const pool size */
            0x00, 0x40, 0x6e, 0x61, 0x6d, 0x65, 0x00, 0x5f, 0x5f, 0x67, 0x65,
            0x74, 0x61, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74, 0x65, 0x5f,
            0x5f, 0x00, 0x40, 0x72, 0x65, 0x73, 0x00, /* const pool */
        };
        pikaVM_runByteCode(host, (uint8_t*)bytes);
        return args_getArg(host->list, "@res");
    }
    return NULL;
}

static Arg* _proxy_getattr(PikaObj* host, char* name) {
#if PIKA_NANO_ENABLE
    return NULL;
#endif
    if ('@' != name[0] && obj_getFlag(host, OBJ_FLAG_PROXY_GETATTR)) {
        args_setStr(host->list, "@name", name);
        /* clang-format off */
        PIKA_PYTHON(
        @res = __getattr__(@name)
        )
        /* clang-format on */
        const uint8_t bytes[] = {
            0x0c, 0x00, 0x00, 0x00, /* instruct array size */
            0x10, 0x81, 0x01, 0x00, 0x00, 0x02, 0x07, 0x00, 0x00, 0x04, 0x13,
            0x00,
            /* instruct array */
            0x18, 0x00, 0x00, 0x00, /* const pool size */
            0x00, 0x40, 0x6e, 0x61, 0x6d, 0x65, 0x00, 0x5f, 0x5f, 0x67, 0x65,
            0x74, 0x61, 0x74, 0x74, 0x72, 0x5f, 0x5f, 0x00, 0x40, 0x72, 0x65,
            0x73, 0x00, /* const pool */
        };
        pikaVM_runByteCode(host, (uint8_t*)bytes);
        return args_getArg(host->list, "@res");
    }
    return NULL;
}

static Arg* VM_instruction_handler_REF(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    PikaObj* host_object = NULL;
    char* arg_path = data;
    char* arg_name = strPointToLastToken(arg_path, '.');
    PIKA_BOOL is_temp = PIKA_FALSE;

    switch (data[0]) {
        case 'T':
            if (strEqu(arg_path, (char*)"True")) {
                return arg_setInt(arg_ret_reg, "", 1);
            }
        case 'F':
            if (strEqu(arg_path, (char*)"False")) {
                return arg_setInt(arg_ret_reg, "", 0);
            }
        case 'N':
            if (strEqu(arg_path, (char*)"None")) {
                return arg_setNull(arg_ret_reg);
            }
        case 'R':
            if (strEqu(arg_path, (char*)"RuntimeError")) {
                return arg_setInt(arg_ret_reg, "", PIKA_RES_ERR_RUNTIME_ERROR);
            }
    }

    Arg* res = NULL;
    if (arg_path[0] == '.') {
        /* find host from stack */
        Arg* host_obj = stack_popArg_alloc(&(vm->stack));
        if (argType_isObject(arg_getType(host_obj))) {
            host_object = arg_getPtr(host_obj);
            res = arg_copy_noalloc(obj_getArg(host_object, arg_path + 1),
                                   arg_ret_reg);
        }
        arg_deinit(host_obj);
        goto exit;
    }

    /* host_object is self */
    if (NULL == host_object) {
        if (!strIsContain(arg_path, '.')) {
            host_object = vm->locals;
        }
    }

    /* find in local list first */
    if (NULL == host_object) {
        host_object = obj_getHostObjWithIsTemp(vm->locals, arg_path, &is_temp);
    }

    /* find in global list */
    if (NULL == host_object) {
        host_object = obj_getHostObjWithIsTemp(vm->globals, arg_path, &is_temp);
    }

    /* error cannot found host_object */
    if (NULL == host_object) {
        goto exit;
    }

    /* proxy */
    if (NULL == res) {
        res = _proxy_getattribute(host_object, arg_name);
    }

    /* find res in host */
    if (NULL == res) {
        res = args_getArg(host_object->list, arg_name);
    }

    /* find res in host prop */
    if (NULL == res) {
        res = _obj_getProp(host_object, arg_name);
    }

    /* find res in globlas */
    if (NULL == res) {
        res = args_getArg(vm->globals->list, arg_name);
    }

    /* proxy */
    if (NULL == res) {
        res = _proxy_getattr(host_object, arg_name);
    }

exit:
    if (NULL == res) {
        VMState_setErrorCode(vm, PIKA_RES_ERR_ARG_NO_FOUND);
        __platform_printf("NameError: name '%s' is not defined\r\n", arg_path);
    } else {
        res = arg_copy_noalloc(res, arg_ret_reg);
    }
    if (is_temp) {
        obj_deinit(host_object);
    }
    return res;
}

static Arg* VM_instruction_handler_GER(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    PIKA_RES err = (PIKA_RES)vm->try_error_code;
    Arg* err_arg = arg_newInt(err);
    return err_arg;
}

Arg* _get_return_arg(PikaObj* locals) {
    Arg* res = args_getArg(locals->list, (char*)"@rt");
    args_removeArg_notDeinitArg(locals->list, res);
    return res;
}

Arg* _obj_runMethodArgWithState(PikaObj* self,
                                PikaObj* locals,
                                Arg* method_arg,
                                RunState* run_state,
                                Arg* ret_arg_reg) {
    pika_assert(NULL != run_state);
    Arg* return_arg = NULL;
    /* get method Ptr */
    Method method_ptr = methodArg_getPtr(method_arg);
    /* get method type list */
    ArgType method_type = arg_getType(method_arg);
    /* error */
    if (ARG_TYPE_NONE == method_type) {
        return NULL;
    }
    ByteCodeFrame* method_bytecodeFrame =
        methodArg_getBytecodeFrame(method_arg);

    /* redirect to def context */
    if (!argType_isNative(method_type)) {
        self = methodArg_getDefContext(method_arg);
    }

    obj_setErrorCode(self, PIKA_RES_OK);

    /* run method */
    if (method_type == ARG_TYPE_METHOD_NATIVE) {
        /* native method */
        method_ptr(self, locals->list);
        /* get method return */
        return_arg = _get_return_arg(locals);
    } else if (method_type == ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR) {
        /* native method */
        method_ptr(self, locals->list);
        /* get method return */
        return_arg = _get_return_arg(locals);
    } else {
        /* static method and object method */
        /* byteCode */
        uintptr_t insturctArray_start = (uintptr_t)instructArray_getByOffset(
            &(method_bytecodeFrame->instruct_array), 0);
        uint16_t pc = (uintptr_t)method_ptr - insturctArray_start;
        locals = __pikaVM_runByteCodeFrameWithState(
            self, locals, self, method_bytecodeFrame, pc, run_state);

        /* get method return */
        return_arg = _get_return_arg(locals);
    }
    return return_arg;
}

Arg* obj_runMethodArgWithState(PikaObj* self,
                               PikaObj* method_args_obj,
                               Arg* method_arg,
                               RunState* try_state) {
    return _obj_runMethodArgWithState(self, method_args_obj, method_arg,
                                      try_state, NULL);
}

Arg* obj_runMethodArgWithState_noalloc(PikaObj* self,
                                       PikaObj* locals,
                                       Arg* method_arg,
                                       RunState* run_state,
                                       Arg* ret_arg_reg) {
    return _obj_runMethodArgWithState(self, locals, method_arg, run_state,
                                      ret_arg_reg);
}

Arg* obj_runMethodArg(PikaObj* self,
                      PikaObj* method_args_obj,
                      Arg* method_arg) {
    RunState run_state = {.try_state = TRY_STATE_NONE,
                          .try_result = TRY_RESULT_NONE};
    return obj_runMethodArgWithState(self, method_args_obj, method_arg,
                                     &run_state);
}

static char* _kw_to_default_all(FunctionArgsInfo* f,
                                char* arg_name,
                                int* argc,
                                Arg* argv[]) {
#if PIKA_NANO
    return arg_name;
#endif
    while (strIsContain(arg_name, '=')) {
        strPopLastToken(arg_name, '=');
        /* load default arg from kws */
        if (f->kw != NULL) {
            Arg* default_arg = pikaDict_getArg(f->kw, arg_name);
            if (default_arg != NULL) {
                argv[(*argc)++] = arg_copy(default_arg);
            }
        }
        arg_name = strPopLastToken(f->type_list, ',');
    }
    return arg_name;
}

static int _kw_to_pos_one(FunctionArgsInfo* f,
                          char* arg_name,
                          int* argc,
                          Arg* argv[]) {
    if (f->kw == NULL) {
        return 0;
    }
    Arg* pos_arg = pikaDict_getArg(f->kw, arg_name);
    if (pos_arg == NULL) {
        return 0;
    }
    argv[(*argc)++] = arg_copy(pos_arg);
    return 1;
}

static void _kw_to_pos_all(FunctionArgsInfo* f, int* argc, Arg* argv[]) {
    int arg_num_need = f->n_positional - f->n_positional_got;
    if (0 == arg_num_need) {
        return;
    }
    for (int i = 0; i < arg_num_need; i++) {
        char* arg_name = strPopLastToken(f->type_list, ',');
        pika_assert(f->kw != NULL);
        Arg* pos_arg = pikaDict_getArg(f->kw, arg_name);
        pika_assert(pos_arg != NULL);
        argv[(*argc)++] = arg_copy(pos_arg);
    }
}

static void _loadLocalsFromArgv(Args* locals, int argc, Arg* argv[]) {
    for (int i = 0; i < argc; i++) {
        Arg* arg = argv[i];
        args_setArg(locals, arg);
    }
}

static void _type_list_parse(FunctionArgsInfo* f) {
    if (f->type_list[0] == 0) {
        f->n_positional = 0;
        return;
    }
    int8_t res = strCountSign(f->type_list, ',') + 1;
    int8_t x = strCountSign(f->type_list, '*');
    int8_t y = strCountSign(f->type_list, '=');
    /* default */
    if (y > 0) {
        res -= y;
        f->is_default = PIKA_TRUE;
        f->n_default = y;
    }
    /* vars */
    if (x == 1) {
        f->is_vars = PIKA_TRUE;
        f->n_positional = res - 1;
        return;
    }
    /* kw */
    if (x == 2) {
        f->is_keys = 1;
        f->n_positional = res - 1;
        return;
    }
    /* vars and kw */
    if (x == 3) {
        f->is_vars = PIKA_TRUE;
        f->is_keys = PIKA_TRUE;
        f->n_positional = res - 2;
        return;
    }
    f->n_positional = res;
    return;
}

static void _kw_push(FunctionArgsInfo* f, Arg* call_arg, int i) {
    if (NULL == f->kw) {
        f->kw = New_pikaDict();
    }
    if (NULL == f->kw_keys) {
        f->kw_keys = New_pikaDict();
    }
    arg_setIsKeyword(call_arg, PIKA_FALSE);
    pikaDict_setArg(f->kw, call_arg);
    char kw_keys_index_buff[11] = {0};
    char* kw_keys_index = fast_itoa(kw_keys_index_buff, i);
    pikaDict_setArg(f->kw_keys,
                    arg_setInt(NULL, kw_keys_index, arg_getNameHash(call_arg)));
}

static void _load_call_arg(VMState* vm,
                           Arg* call_arg,
                           FunctionArgsInfo* f,
                           int* i,
                           int* argc,
                           Arg* argv[]) {
    /* load the kw arg */
    if (call_arg != NULL && arg_getIsKeyword(call_arg)) {
        _kw_push(f, call_arg, *i);
        return;
    }
    /* load variable arg */
    if (f->i_arg > f->n_positional) {
        if (f->is_vars) {
            pikaList_append(&(f->tuple)->super, call_arg);
            /* the append would copy the arg */
            if (NULL != call_arg) {
                arg_deinit(call_arg);
            }
            return;
        }
    }
    char* arg_name = strPopLastToken(f->type_list, ',');
    /* load default from pos */
    if (f->i_arg > f->n_positional) {
        if (f->is_default) {
            if (arg_name[strlen(arg_name) - 1] == '=') {
                /* found default arg*/
                arg_name[strlen(arg_name) - 1] = '\0';
                arg_setNameHash(call_arg, hash_time33EndWith(arg_name, ':'));
                argv[(*argc)++] = call_arg;
            }
            return;
        }
    }
    /* load default from kw */
    arg_name = _kw_to_default_all(f, arg_name, argc, argv);
    /* load position arg */
    if (_kw_to_pos_one(f, arg_name, argc, argv)) {
        /* load pos from kw */
        (f->n_positional_got)++;
        /* restore the stack */
        (*i)--;
        stack_pushArg(&(vm->stack), call_arg);
        return;
    }
    /*load pos from pos */
    arg_setNameHash(call_arg, hash_time33EndWith(arg_name, ':'));
    argv[(*argc)++] = call_arg;
    (f->n_positional_got)++;
}

static int _get_n_input_with_unpack(VMState* vm) {
#if PIKA_NANO_ENABLE
    return VMState_getInputArgNum(vm);
#else
    int n_input = VMState_getInputArgNum(vm);
    int get_star = 0;
    int unpack_num = 0;
    for (int i = 0; i < n_input; i++) {
        Arg* arg_check = stack_checkArg(&(vm->stack), i);
        if (NULL == arg_check) {
            break;
        }
        if (arg_getIsDoubleStarred(arg_check) || arg_getIsStarred(arg_check)) {
            get_star++;
        }
    }
    if (0 == get_star) {
        return n_input;
    }
    Stack stack_tmp = {0};
    stack_init(&stack_tmp);
    for (int i = 0; i < n_input; i++) {
        /* unpack starred arg */
        Arg* call_arg = stack_popArg_alloc(&(vm->stack));
        if (call_arg == NULL) {
            break;
        }
        if (arg_getIsStarred(call_arg)) {
            pika_assert(argType_isObject(arg_getType(call_arg)));
            PikaObj* obj = arg_getPtr(call_arg);
            /* clang-format off */
            PIKA_PYTHON(
            @l = __len__()
            )
            /* clang-format on */
            const uint8_t bytes[] = {
                0x08, 0x00, 0x00, 0x00, /* instruct array size */
                0x00, 0x82, 0x01, 0x00, 0x00, 0x04, 0x09, 0x00, /* instruct
                                                                   array */
                0x0c, 0x00, 0x00, 0x00, /* const pool size */
                0x00, 0x5f, 0x5f, 0x6c, 0x65, 0x6e, 0x5f, 0x5f, 0x00,
                0x40, 0x6c, 0x00, /* const pool */
            };
            pikaVM_runByteCode(obj, (uint8_t*)bytes);
            int len = obj_getInt(obj, "@l");
            for (int i_star_arg = len - 1; i_star_arg >= 0; i_star_arg--) {
                obj_setInt(obj, "@d", i_star_arg);
                /* clang-format off */
                PIKA_PYTHON(
                @a = __getitem__(@d)
                )
                /* clang-format on */
                const uint8_t bytes[] = {
                    0x0c, 0x00, 0x00, 0x00, /* instruct array size */
                    0x10, 0x81, 0x01, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x04,
                    0x10, 0x00,
                    /* instruct array */
                    0x13, 0x00, 0x00, 0x00, /* const pool size */
                    0x00, 0x40, 0x64, 0x00, 0x5f, 0x5f, 0x67, 0x65, 0x74, 0x69,
                    0x74, 0x65, 0x6d, 0x5f, 0x5f, 0x00, 0x40, 0x61,
                    0x00, /* const pool */
                };
                pikaVM_runByteCode(obj, (uint8_t*)bytes);
                Arg* arg_a = obj_getArg(obj, "@a");
                stack_pushArg(&stack_tmp, arg_copy(arg_a));
                unpack_num++;
            }
            goto __continue;
        }
        if (arg_getIsDoubleStarred(call_arg)) {
            pika_assert(argType_isObject(arg_getType(call_arg)));
            PikaObj* New_PikaStdData_Dict(Args * args);
            PikaObj* obj = arg_getPtr(call_arg);
            pika_assert(obj->constructor == New_PikaStdData_Dict);
            PikaDict* dict = obj_getPtr(obj, "dict");
            int i_item = 0;
            while (PIKA_TRUE) {
                Arg* item_val = args_getArgByidex(&dict->super, i_item);
                if (NULL == item_val) {
                    break;
                }
                /* unpack as keyword arg */
                arg_setIsKeyword(item_val, 1);
                stack_pushArg(&stack_tmp, arg_copy(item_val));
                i_item++;
            }
            goto __continue;
        }
        stack_pushArg(&stack_tmp, arg_copy(call_arg));
    __continue:
        if (NULL != call_arg) {
            arg_deinit(call_arg);
        }
    }
    int n_input_new = stack_getTop(&stack_tmp);
    for (int i = 0; i < n_input_new; i++) {
        Arg* arg = stack_popArg_alloc(&stack_tmp);
        stack_pushArg(&(vm->stack), arg);
    }
    stack_deinit(&stack_tmp);
    return n_input_new;
#endif
}

#define vars_or_keys_or_default (f.is_vars || f.is_keys || f.is_default)
static int VMState_loadArgsFromMethodArg(VMState* vm,
                                         PikaObj* method_host_obj,
                                         Args* locals,
                                         Arg* method_arg,
                                         char* method_name,
                                         int n_used) {
    Arg* argv[PIKA_ARG_NUM_MAX] = {0};
    int argc = 0;
    char _buffs1[PIKA_LINE_BUFF_SIZE] = {0};
    char* buffs1 = (char*)_buffs1;
    char _buffs2[PIKA_LINE_BUFF_SIZE] = {0};
    char* buffs2 = (char*)_buffs2;
    FunctionArgsInfo f = {0};
    char* type_list_buff = NULL;
    /* get method type list */
    f.type_list = methodArg_getTypeList(method_arg, buffs1, sizeof(_buffs1));
    if (NULL == f.type_list) {
        __platform_printf(
            "OverflowError: type list is too long, please use bigger "
            "PIKA_LINE_BUFF_SIZE\r\n");
        while (1)
            ;
    }
    f.method_type = arg_getType(method_arg);

    /* get arg_num_pos */
    _type_list_parse(&f);
    if (f.method_type == ARG_TYPE_METHOD_OBJECT) {
        /* delete the 'self' */
        f.n_positional--;
    }

    f.n_input = _get_n_input_with_unpack(vm);

    /* check arg num */
    if (f.method_type == ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR ||
        f.method_type == ARG_TYPE_METHOD_CONSTRUCTOR ||
        f.is_vars == PIKA_TRUE || n_used != 0) {
        /* skip for constrctor */
        /* skip for variable args */
        /* n_used != 0 means it is a factory method */
    } else {
        /* check position arg num */
        if (!vars_or_keys_or_default) {
            if (f.n_positional != f.n_input) {
                VMState_setErrorCode(vm, PIKA_RES_ERR_INVALID_PARAM);
                __platform_printf(
                    "TypeError: %s() takes %d positional argument but %d were "
                    "given\r\n",
                    method_name, f.n_positional, f.n_input);
                goto exit;
            }
        }
#if !PIKA_NANO_ENABLE
        if (f.is_default) {
            int8_t n_min = f.n_positional;
            int8_t n_max = f.n_positional + f.n_default;
            if (f.n_input < n_min || f.n_input > n_max) {
                VMState_setErrorCode(vm, PIKA_RES_ERR_INVALID_PARAM);
                __platform_printf(
                    "TypeError: %s() takes from %d to %d positional arguments "
                    "but %d were given\r\n",
                    method_name, n_min, n_max, f.n_input);
                goto exit;
            }
        }
#endif
    }

    if (vars_or_keys_or_default) {
        f.n_arg = f.n_input;
    } else {
        f.n_arg = f.n_positional;
    }

    /* create tuple/dict for vars/keys */
    if (vars_or_keys_or_default) {
        if (strGetSize(f.type_list) > sizeof(_buffs2)) {
            __platform_printf(
                "OverFlowError: please use bigger PIKA_LINE_BUFF_SIZE\r\n");
            while (1) {
            }
        }
        type_list_buff = strCopy(buffs2, f.type_list);
        uint8_t n_typelist = strCountSign(type_list_buff, ',') + 1;
        for (int i = 0; i < n_typelist; i++) {
            char* arg_def = strPopLastToken(type_list_buff, ',');
            if (arg_def[0] == '*' && arg_def[1] != '*') {
                /* get variable tuple name */
                /* skip the '*' */
                f.var_tuple_name = arg_def + 1;
                /* create tuple */
                if (NULL == f.tuple) {
                    f.tuple = New_pikaTuple();
                    /* remove the format arg */
                    strPopLastToken(f.type_list, ',');
                }
                continue;
            }
            if (arg_def[0] == '*' && arg_def[1] == '*') {
                /* get kw dict name */
                f.kw_dict_name = arg_def + 2;
                f.kw = New_pikaDict();
                f.kw_keys = New_pikaDict();
                /* remove the format arg */
                strPopLastToken(f.type_list, ',');
                continue;
            }
        }
    }

    /* load args */
    for (int i = 0; i < f.n_arg; i++) {
        f.i_arg = f.n_arg - i;
        Arg* call_arg = stack_popArg_alloc(&(vm->stack));
        _load_call_arg(vm, call_arg, &f, &i, &argc, argv);
    }

/* only default */
#if !PIKA_NANO_ENABLE
    if (strIsContain(f.type_list, '=')) {
        char* arg_name = strPopLastToken(f.type_list, ',');
        _kw_to_default_all(&f, arg_name, &argc, argv);
    }
    /* load kw to pos */
    _kw_to_pos_all(&f, &argc, argv);
#endif

    if (f.tuple != NULL) {
        pikaList_reverse(&(f.tuple)->super);
        /* load variable tuple */
        PikaObj* New_PikaStdData_Tuple(Args * args);
        PikaObj* tuple_obj = newNormalObj(New_PikaStdData_Tuple);
        obj_setPtr(tuple_obj, "list", f.tuple);
        Arg* argi =
            arg_setPtr(NULL, f.var_tuple_name, ARG_TYPE_OBJECT, tuple_obj);
        argv[argc++] = argi;
    }

    if (f.kw != NULL) {
        if (NULL == f.kw_dict_name) {
            f.kw_dict_name = "__kwargs";
        }
        /* load kw dict */
        PikaObj* New_PikaStdData_Dict(Args * args);
        PikaObj* dict_obj = newNormalObj(New_PikaStdData_Dict);
        obj_setPtr(dict_obj, "dict", f.kw);
        obj_setPtr(dict_obj, "_keys", f.kw_keys);
        Arg* argi = arg_setPtr(NULL, f.kw_dict_name, ARG_TYPE_OBJECT, dict_obj);
        argv[argc++] = argi;
    }

    /* load 'self' as the first arg when call object method */
    if (f.method_type == ARG_TYPE_METHOD_OBJECT) {
        Arg* call_arg = arg_setRef(NULL, "self", method_host_obj);
        argv[argc++] = call_arg;
    }
    _loadLocalsFromArgv(locals, argc, argv);
exit:
    return f.n_arg;
}

void __vm_List_append(PikaObj* self, Arg* arg) {
    PikaList* list = obj_getPtr(self, "list");
    pikaList_append(list, arg);
}

void __vm_List___init__(PikaObj* self) {
    PikaList* list = New_pikaList();
    obj_setPtr(self, "list", list);
}

#if PIKA_BUILTIN_STRUCT_ENABLE
PikaObj* New_PikaStdData_List(Args* args);
PikaObj* New_PikaStdData_Tuple(Args* args);
#endif

static Arg* _vm_create_list_or_tuple(PikaObj* self,
                                     VMState* vm,
                                     PIKA_BOOL is_list) {
#if PIKA_BUILTIN_STRUCT_ENABLE
    NewFun constructor = is_list ? New_PikaStdData_List : New_PikaStdData_Tuple;
    uint8_t n_arg = VMState_getInputArgNum(vm);
    PikaObj* list = newNormalObj(constructor);
    __vm_List___init__(list);
    Stack stack = {0};
    stack_init(&stack);
    /* load to local stack to change sort */
    for (int i = 0; i < n_arg; i++) {
        Arg* arg = stack_popArg_alloc(&(vm->stack));
        pika_assert(arg != NULL);
        stack_pushArg(&stack, arg);
    }
    for (int i = 0; i < n_arg; i++) {
        Arg* arg = stack_popArg_alloc(&stack);
        pika_assert(arg != NULL);
        __vm_List_append(list, arg);
        arg_deinit(arg);
    }
    stack_deinit(&stack);
    return arg_newObj(list);
#else
    return VM_instruction_handler_NON(self, vm, "", NULL);
#endif
}

static Arg* VM_instruction_handler_LST(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    return _vm_create_list_or_tuple(self, vm, PIKA_TRUE);
}

void __vm_Dict___init__(PikaObj* self) {
    PikaDict* dict = New_pikaDict();
    PikaDict* keys = New_pikaDict();
    obj_setPtr(self, "dict", dict);
    obj_setPtr(self, "_keys", keys);
}

void __vm_Dict_set(PikaObj* self, Arg* arg, char* key) {
    PikaDict* dict = obj_getPtr(self, "dict");
    PikaDict* keys = obj_getPtr(self, "_keys");
    Arg* arg_key = arg_setStr(NULL, key, key);
    Arg* arg_new = arg_copy(arg);
    arg_setName(arg_new, key);
    pikaDict_setArg(dict, arg_new);
    pikaDict_setArg(keys, arg_key);
}

#if PIKA_BUILTIN_STRUCT_ENABLE
PikaObj* New_PikaStdData_Dict(Args* args);
#endif

static Arg* VM_instruction_handler_DCT(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
#if PIKA_BUILTIN_STRUCT_ENABLE
    uint8_t n_arg = VMState_getInputArgNum(vm);
    PikaObj* dict = newNormalObj(New_PikaStdData_Dict);
    __vm_Dict___init__(dict);
    Stack stack = {0};
    stack_init(&stack);
    /* load to local stack to change sort */
    for (int i = 0; i < n_arg; i++) {
        Arg* arg = stack_popArg_alloc(&(vm->stack));
        stack_pushArg(&stack, arg);
    }
    for (int i = 0; i < n_arg / 2; i++) {
        Arg* key_arg = stack_popArg_alloc(&stack);
        Arg* val_arg = stack_popArg_alloc(&stack);
        __vm_Dict_set(dict, val_arg, arg_getStr(key_arg));
        arg_deinit(key_arg);
        arg_deinit(val_arg);
    }
    stack_deinit(&stack);
    return arg_newObj(dict);
#else
    return VM_instruction_handler_NON(self, vm, data, arg_ret_reg);
#endif
}

static Arg* VM_instruction_handler_RET(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    /* exit jmp signal */
    vm->jmp = VM_JMP_EXIT;
    Arg* return_arg = stack_popArg_alloc(&(vm->stack));
    method_returnArg(vm->locals->list, return_arg);
    return NULL;
}

#if !PIKA_NANO_ENABLE
static char* _find_super_class_name(VMState* vm) {
    /* find super class */
    int offset = 0;
    char* super_class_name = NULL;
    while (1) {
        offset -= instructUnit_getSize();
        if (vm->pc + offset >= (int)VMState_getInstructArraySize(vm)) {
            return 0;
        }
        if ((CLS == VMstate_getInstructWithOffset(vm, offset))) {
            break;
        }
    }

    while (1) {
        offset += instructUnit_getSize();
        if (vm->pc + offset >= (int)VMState_getInstructArraySize(vm)) {
            return 0;
        }
        if ((RUN == instructUnit_getInstruct(
                        VMState_getInstructUnitWithOffset(vm, offset)))) {
            super_class_name = VMState_getConstWithOffset(vm, offset);
            return super_class_name;
        }
    }
}
#endif

#if !PIKA_NANO_ENABLE
static char* _find_self_name(VMState* vm) {
    /* find super class */
    int offset = 0;
    char* self_name = NULL;
    while (1) {
        offset -= instructUnit_getSize();
        if (vm->pc + offset >= (int)VMState_getInstructArraySize(vm)) {
            return 0;
        }
        if ((CLS == VMstate_getInstructWithOffset(vm, offset))) {
            break;
        }
    }

    while (1) {
        offset += instructUnit_getSize();
        if (vm->pc + offset >= (int)VMState_getInstructArraySize(vm)) {
            return 0;
        }
        if ((OUT == instructUnit_getInstruct(
                        VMState_getInstructUnitWithOffset(vm, offset)))) {
            self_name = VMState_getConstWithOffset(vm, offset);
            return self_name;
        }
    }
}
#endif

static Arg* VM_instruction_handler_RUN(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    Arg* return_arg = NULL;
    VMParameters* sub_locals = NULL;
    char* run_path = data;
    PikaObj* method_host = NULL;
    PikaObj* obj_this = NULL;
    Arg* method = NULL;
    Arg* host_arg = NULL;
    PIKA_BOOL is_temp = PIKA_FALSE;
    PIKA_BOOL skip_init = PIKA_FALSE;
    char* sys_out;
    int n_used = 0;
    arg_newReg(arg_reg1, 64);
    RunState sub_run_state = {.try_state = vm->run_state->try_state,
                              .try_result = TRY_RESULT_NONE};
    pika_assert(NULL != vm->run_state);

    /* inhert */
    if (vm->pc - 2 * (int)instructUnit_getSize() >= 0) {
        if (CLS == VMstate_getInstructWithOffset(
                       vm, -2 * (int)instructUnit_getSize())) {
            skip_init = PIKA_TRUE;
        }
    }

    /* tuple or single arg */
    if (run_path[0] == 0) {
        if (VMState_getInputArgNum(vm) < 2) {
            /* return arg directly */
            return_arg = stack_popArg(&(vm->stack), arg_ret_reg);
            goto exit;
        }
        /* create a tuple */
        return_arg = _vm_create_list_or_tuple(self, vm, PIKA_FALSE);
        goto exit;
    }

#if !PIKA_NANO_ENABLE
    /* support for super() */
    if (strEqu(run_path, "super")) {
        run_path = _find_super_class_name(vm);
        vm->in_super = PIKA_TRUE;
        vm->super_invoke_deepth = VMState_getInvokeDeepthNow(vm);
        skip_init = PIKA_TRUE;
    }
#endif

    /* return tiny obj */
    if (strEqu(run_path, "TinyObj")) {
        return_arg = arg_newMetaObj(New_TinyObj);
        goto exit;
    }

    if (strEqu(run_path, "object")) {
        return_arg = arg_newMetaObj(New_TinyObj);
        goto exit;
    }

    /* get method host obj from reg */
    if (NULL == method_host && _checkLReg(run_path)) {
        uint8_t reg_index = _getLRegIndex(run_path);
        method_host = vm->lreg[reg_index];
    }

#if !PIKA_NANO_ENABLE
    /* get method host obj from stack */
    if (NULL == method_host && run_path[0] == '.') {
        /* get method host obj from stack */
        Arg* stack_tmp[PIKA_ARG_NUM_MAX] = {0};
        int n_arg = VMState_getInputArgNum(vm);
        if (n_arg > PIKA_ARG_NUM_MAX) {
            __platform_printf(
                "[ERROR] Too many args in RUN instruction, please use bigger "
                "#define PIKA_ARG_NUM_MAX\n");
            while (1) {
            }
        }
        for (int i = 0; i < n_arg; i++) {
            stack_tmp[i] = stack_popArg_alloc(&(vm->stack));
        }
        host_arg = stack_tmp[n_arg - 1];
        method_host = _arg_to_obj(host_arg, &is_temp);
        if (NULL != method_host) {
            n_used++;
        }
        /* push back other args to stack */
        for (int i = n_arg - 2; i >= 0; i--) {
            stack_pushArg(&(vm->stack), stack_tmp[i]);
        }
    }
#endif

    /* get method host obj from local scope */
    if (NULL == method_host) {
        method_host = obj_getHostObjWithIsTemp(vm->locals, run_path, &is_temp);
    }

    /* get method host obj from local scope */
    if (NULL == method_host) {
        method_host = obj_getHostObjWithIsTemp(vm->globals, run_path, &is_temp);
    }

    /* method host obj is not found */
    if (NULL == method_host) {
        /* error, not found object */
        VMState_setErrorCode(vm, PIKA_RES_ERR_ARG_NO_FOUND);
        __platform_printf("Error: method '%s' no found.\r\n", run_path);
        goto exit;
    }

#if !PIKA_NANO_ENABLE
    if (!skip_init && vm->in_super &&
        VMState_getInvokeDeepthNow(vm) == vm->super_invoke_deepth - 1) {
        vm->in_super = PIKA_FALSE;
        obj_this = obj_getPtr(vm->locals, _find_self_name(vm));
    }
#endif

    /* get object this */
    if (NULL == obj_this) {
        obj_this = method_host;
    }

    /* get method in object */
    if (NULL == method) {
        method = obj_getMethodArg_noalloc(method_host, run_path, &arg_reg1);
    }

    /* get method in locals */
    if (NULL == method) {
        method = obj_getMethodArg_noalloc(vm->locals, run_path, &arg_reg1);
    }

    /* get method in global */
    if (NULL == method) {
        method = obj_getMethodArg_noalloc(vm->globals, run_path, &arg_reg1);
    }

    /* assert method exist */
    if (NULL == method || ARG_TYPE_NONE == arg_getType(method)) {
        /* error, method no found */
        VMState_setErrorCode(vm, PIKA_RES_ERR_ARG_NO_FOUND);
        __platform_printf("NameError: name '%s' is not defined\r\n", run_path);
        goto exit;
    }

    /* assert methodd type */
    if (!argType_isCallable(arg_getType(method))) {
        /* error, method no found */
        VMState_setErrorCode(vm, PIKA_RES_ERR_ARG_NO_FOUND);
        __platform_printf("TypeError: '%s' object is not callable\r\n",
                          run_path);
        goto exit;
    }

    /* create sub local scope */
    sub_locals = New_PikaObj();

    /* load args from vmState to sub_local->list */
    n_used += VMState_loadArgsFromMethodArg(vm, obj_this, sub_locals->list,
                                            method, run_path, n_used);

    /* load args failed */
    if (vm->error_code != 0) {
        goto exit;
    }

    /* run method arg */
    return_arg = obj_runMethodArgWithState_noalloc(obj_this, sub_locals, method,
                                                   &sub_run_state, arg_ret_reg);
    if (skip_init) {
        if (arg_getType(return_arg) == ARG_TYPE_OBJECT_NEW) {
            pika_assert(NULL != return_arg);
            arg_setType(return_arg, ARG_TYPE_OBJECT);
        }
    }

    if (sub_run_state.try_result != TRY_RESULT_NONE) {
        /* try result */
        vm->error_code = sub_run_state.try_result;
    }

    /* __init__() */
    if (NULL != return_arg && ARG_TYPE_OBJECT_NEW == arg_getType(return_arg)) {
        pika_assert(NULL != return_arg);
        arg_setType(return_arg, ARG_TYPE_OBJECT);
        /* init object */
        PikaObj* new_obj = arg_getPtr(return_arg);
        Arg* method_arg =
            obj_getMethodArg_noalloc(new_obj, "__init__", &arg_reg1);
        PikaObj* sub_locals = New_PikaObj();
        Arg* return_arg_init = NULL;
        if (NULL == method_arg) {
            goto init_exit;
        }
        VMState_loadArgsFromMethodArg(vm, new_obj, sub_locals->list, method_arg,
                                      "__init__", n_used);
        /* load args failed */
        if (vm->error_code != 0) {
            goto init_exit;
        }
        return_arg_init = obj_runMethodArgWithState(new_obj, sub_locals,
                                                    method_arg, &sub_run_state);
    init_exit:
        if (NULL != return_arg_init) {
            arg_deinit(return_arg_init);
        }
        obj_deinit(sub_locals);
        if (NULL != method_arg) {
            arg_deinit(method_arg);
        }
    }

    /* transfer sysOut */
    sys_out = obj_getSysOut(obj_this);
    if (NULL != sys_out) {
        args_setSysOut(vm->locals->list, sys_out);
    }

    /* transfer errCode */
    if (0 != obj_getErrorCode(obj_this)) {
        /* method error */
        VMState_setErrorCode(vm, PIKA_RES_ERR_RUNTIME_ERROR);
    }

    goto exit;
exit:
    if (NULL != method) {
        arg_deinit(method);
    }
    if (NULL != sub_locals) {
        obj_deinit(sub_locals);
    }
    if (NULL != host_arg) {
        arg_deinit(host_arg);
    }
    if (NULL != method_host && is_temp) {
        /* class method */
        obj_deinit(method_host);
    }

    return return_arg;
}

static char* __get_transferd_str(Args* buffs, char* str, size_t* iout_p) {
    char* str_rep = strsReplace(buffs, str, "\\n", "\n");
    str_rep = strsReplace(buffs, str_rep, "\\r", "\r");
    str_rep = strsReplace(buffs, str_rep, "\\t", "\t");
    str_rep = strsReplace(buffs, str_rep, "\\\\", "\\");

    char* transfered_str = args_getBuff(buffs, strGetSize(str_rep));
    size_t i_out = 0;
    size_t len = strGetSize(str_rep);
    for (size_t i = 0; i < len; i++) {
        /* eg. replace '\x33' to '3' */
        if ((str_rep[i] == '\\') && (str_rep[i + 1] == 'x')) {
            char hex_str[] = "0x00";
            hex_str[2] = str_rep[i + 2];
            hex_str[3] = str_rep[i + 3];
            char hex = (char)strtoll(hex_str, NULL, 0);
            transfered_str[i_out++] = hex;
            i += 3;
            continue;
        }
        /* normal char */
        transfered_str[i_out++] = str_rep[i];
    }
    *iout_p = i_out;
    return transfered_str;
}

static Arg* VM_instruction_handler_STR(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    if (strIsContain(data, '\\')) {
        Args buffs = {0};
        size_t i_out = 0;
        char* transfered_str = __get_transferd_str(&buffs, data, &i_out);
        Arg* return_arg = arg_ret_reg;
        return_arg = arg_setStr(return_arg, "", transfered_str);
        strsDeinit(&buffs);
        return return_arg;
    }
    return arg_setStr(arg_ret_reg, "", data);
}

static Arg* VM_instruction_handler_BYT(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    if (strIsContain(data, '\\')) {
        Args buffs = {0};
        size_t i_out = 0;
        char* transfered_str = __get_transferd_str(&buffs, data, &i_out);
        Arg* return_arg = New_arg(NULL);
        return_arg =
            arg_setBytes(return_arg, "", (uint8_t*)transfered_str, i_out);
        strsDeinit(&buffs);
        return return_arg;
    }
    return arg_newBytes((uint8_t*)data, strGetSize(data));
}

static PIKA_BOOL _proxy_setattr(PikaObj* self, char* name, Arg* arg) {
#if PIKA_NANO_ENABLE
    return PIKA_FALSE;
#endif
    if (obj_getFlag(self, OBJ_FLAG_PROXY_SETATTR)) {
        obj_setStr(self, "@name", name);
        obj_setArg(self, "@value", arg);
        /* clang-format off */
        PIKA_PYTHON(
        __setattr__(@name, @value)
        )
        /* clang-format on */
        const uint8_t bytes[] = {
            0x0c, 0x00, 0x00, 0x00, /* instruct array size */
            0x10, 0x81, 0x01, 0x00, 0x10, 0x01, 0x07, 0x00, 0x00, 0x02, 0x0e,
            0x00,
            /* instruct array */
            0x1a, 0x00, 0x00, 0x00, /* const pool size */
            0x00, 0x40, 0x6e, 0x61, 0x6d, 0x65, 0x00, 0x40, 0x76, 0x61, 0x6c,
            0x75, 0x65, 0x00, 0x5f, 0x5f, 0x73, 0x65, 0x74, 0x61, 0x74, 0x74,
            0x72, 0x5f, 0x5f, 0x00, /* const pool */
        };
        pikaVM_runByteCode(self, (uint8_t*)bytes);
        return PIKA_TRUE;
    }
    return PIKA_FALSE;
}

static Arg* VM_instruction_handler_OUT(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    char* arg_path = data;
    char* arg_name = strPointToLastToken(arg_path, '.');
    PikaObj* host_obj = NULL;
    PIKA_BOOL is_temp = PIKA_FALSE;
    arg_newReg(outArg_reg, PIKA_ARG_BUFF_SIZE);
    Arg* out_arg = stack_popArg(&vm->stack, &outArg_reg);
    // Arg* outArg = stack_popArg_alloc(&vm->stack);
    if (NULL == out_arg) {
        return NULL;
    }
    ArgType outArg_type = arg_getType(out_arg);
    if (VMState_getInvokeDeepthNow(vm) > 0) {
        /* in block, is a kw arg */
        arg_setIsKeyword(out_arg, PIKA_TRUE);
        arg_setName(out_arg, arg_path);
        Arg* res = arg_copy_noalloc(out_arg, arg_ret_reg);
        if (arg_isSerialized(out_arg)) {
            arg_deinit(out_arg);
        }
        return res;
    }

    if (_checkLReg(arg_path)) {
        uint8_t index = _getLRegIndex(arg_path);
        if (argType_isObject(outArg_type)) {
            PikaObj* obj = arg_getPtr(out_arg);
            VMState_setLReg(vm, index, obj);
            arg_deinit(out_arg);
        }
        return NULL;
    }

    PikaObj* context = vm->locals;
    /* match global_list */
    if (obj_getFlag(vm->locals, OBJ_FLAG_GLOBALS)) {
        char* global_list = args_getStr(vm->locals->list, "@g");
        /* use a arg as buff */
        Arg* global_list_arg = arg_newStr(global_list);
        char* global_list_buff = arg_getStr(global_list_arg);
        /* for each arg arg in global_list */
        for (int i = 0; i < strCountSign(global_list, ',') + 1; i++) {
            char* global_arg = strPopFirstToken(&global_list_buff, ',');
            /* matched global arg, context set to global */
            if (strEqu(global_arg, arg_path)) {
                context = vm->globals;
            }
        }
        arg_deinit(global_list_arg);
    }
    /* use RunAs object */
    if (obj_getFlag(vm->locals, OBJ_FLAG_RUN_AS)) {
        context = args_getPtr(vm->locals->list, "@r");
    }
    /* set free object to nomal object */
    if (ARG_TYPE_OBJECT_NEW == outArg_type) {
        pika_assert(NULL != out_arg);
        arg_setType(out_arg, ARG_TYPE_OBJECT);
    }

    /* ouput arg to context */
    if (arg_path == arg_name) {
        obj_setArg_noCopy(context, arg_path, out_arg);
        return NULL;
    }

    host_obj = obj_getHostObjWithIsTemp(context, arg_path, &is_temp);

    if (NULL == host_obj) {
        host_obj = obj_getHostObjWithIsTemp(vm->globals, arg_path, &is_temp);
    }

    if (host_obj != NULL) {
        if (_proxy_setattr(host_obj, arg_name, out_arg)) {
            return NULL;
        }
        obj_setArg_noCopy(host_obj, arg_name, out_arg);
        return NULL;
    }

    obj_setArg_noCopy(context, arg_path, out_arg);
    return NULL;
}

/* run as */
static Arg* VM_instruction_handler_RAS(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    if (strEqu(data, "$origin")) {
        /* use origin object to run */
        obj_removeArg(vm->locals, "@r");
        obj_clearFlag(vm->locals, OBJ_FLAG_RUN_AS);
        return NULL;
    }
    /* use "data" object to run */
    PikaObj* runAs = obj_getObj(vm->locals, data);
    args_setRef(vm->locals->list, "@r", runAs);
    obj_setFlag(vm->locals, OBJ_FLAG_RUN_AS);
    return NULL;
}

static Arg* VM_instruction_handler_NUM(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    /* fast return */
    if (data[1] == '\0') {
        return arg_setInt(arg_ret_reg, "", data[0] - '0');
    }
    /* hex */
    if (data[1] == 'x' || data[1] == 'X') {
        return arg_setInt(arg_ret_reg, "", strtoll(data, NULL, 0));
    }
    if (data[1] == 'o' || data[1] == 'O') {
        char strtoll_buff[10] = {0};
        strtoll_buff[0] = '0';
        __platform_memcpy(strtoll_buff + 1, data + 2, strGetSize(data) - 2);
        return arg_setInt(arg_ret_reg, "", strtoll(strtoll_buff, NULL, 0));
    }
    if (data[1] == 'b' || data[1] == 'B') {
        char strtoll_buff[10] = {0};
        __platform_memcpy(strtoll_buff, data + 2, strGetSize(data) - 2);
        return arg_setInt(arg_ret_reg, "", strtoll(strtoll_buff, NULL, 2));
    }
    /* float */
    if (strIsContain(data, '.') ||
        (strIsContain(data, 'e') || strIsContain(data, 'E'))) {
        return arg_setFloat(arg_ret_reg, "", strtod(data, NULL));
    }
    /* int */
    return arg_setInt(arg_ret_reg, "", fast_atoi(data));
}

static Arg* VM_instruction_handler_JMP(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    vm->jmp = fast_atoi(data);
    return NULL;
}

static Arg* VM_instruction_handler_SER(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    vm->try_error_code = fast_atoi(data);
    return NULL;
}

static Arg* _VM_JEZ(PikaObj* self,
                    VMState* vm,
                    char* data,
                    Arg* arg_ret_reg,
                    int pika_assert) {
    int thisBlockDeepth = VMState_getBlockDeepthNow(vm);
    int jmp_expect = fast_atoi(data);
    vm->ireg[thisBlockDeepth] = (PIKA_BOOL)!pika_assert;

    if (0 == pika_assert) {
        /* jump */
        vm->jmp = jmp_expect;
    }

    /* restore loop deepth */
    if (2 == jmp_expect && 0 == pika_assert) {
        int block_deepth_now = VMState_getBlockDeepthNow(vm);
        vm->loop_deepth = block_deepth_now;
    }

    return NULL;
}

static Arg* VM_instruction_handler_JEZ(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    Arg* pika_assertArg = stack_popArg(&(vm->stack), arg_ret_reg);
    int pika_assert = 0;
    if (NULL != pika_assertArg) {
        pika_assert = arg_getInt(pika_assertArg);
        arg_deinit(pika_assertArg);
    }
    return _VM_JEZ(self, vm, data, arg_ret_reg, pika_assert);
}

static Arg* VM_instruction_handler_JNZ(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    Arg* pika_assertArg = stack_popArg(&(vm->stack), arg_ret_reg);
    int pika_assert = 0;
    if (NULL != pika_assertArg) {
        pika_assert = arg_getInt(pika_assertArg);
        arg_deinit(pika_assertArg);
    }
    return _VM_JEZ(self, vm, data, arg_ret_reg, !pika_assert);
}

static uint8_t VMState_getInputArgNum(VMState* vm) {
    InstructUnit* ins_unit_now = VMState_getInstructNow(vm);
    uint8_t invoke_deepth_this = instructUnit_getInvokeDeepth(ins_unit_now);
    int32_t pc_this = vm->pc;
    uint8_t num = 0;
    while (1) {
        ins_unit_now--;
        pc_this -= instructUnit_getSize();
        uint8_t invode_deepth = instructUnit_getInvokeDeepth(ins_unit_now);
        if (pc_this < 0) {
            break;
        }
        if (invode_deepth == invoke_deepth_this + 1) {
            if (instructUnit_getInstruct(ins_unit_now) == OUT) {
                continue;
            }
            num++;
        }
        if (instructUnit_getIsNewLine(ins_unit_now)) {
            break;
        }
        if (invode_deepth <= invoke_deepth_this) {
            break;
        }
    }
    return num;
}

void operatorInfo_init(OperatorInfo* info,
                       PikaObj* self,
                       VMState* vm,
                       char* data,
                       Arg* arg_ret_reg) {
    info->opt = data;
    info->res = arg_ret_reg;
    if (info->a1 != NULL) {
        info->t1 = arg_getType(info->a1);
        if (info->t1 == ARG_TYPE_INT) {
            info->i1 = arg_getInt(info->a1);
            info->f1 = (pika_float)info->i1;
        } else if (info->t1 == ARG_TYPE_FLOAT) {
            info->f1 = arg_getFloat(info->a1);
            info->i1 = (int64_t)info->f1;
        }
    }
    info->t2 = arg_getType(info->a2);
    info->vm = vm;
    if (info->t2 == ARG_TYPE_INT) {
        info->i2 = arg_getInt(info->a2);
        info->f2 = (pika_float)info->i2;
    } else if (info->t2 == ARG_TYPE_FLOAT) {
        info->f2 = arg_getFloat(info->a2);
        info->i2 = (int64_t)info->f2;
    }
}

static void _OPT_ADD(OperatorInfo* op) {
#if !PIKA_NANO_ENABLE
    if (argType_isObject(op->t1)) {
        if (!argType_isObject(op->t2)) {
            VMState_setErrorCode(op->vm, PIKA_RES_ERR_OPERATION_FAILED);
            __platform_printf("TypeError: unsupported operand +\n");
            op->res = NULL;
            return;
        }
        PikaObj* obj1 = arg_getPtr(op->a1);
        Arg* method_add = obj_getMethodArg(obj1, "__add__");
        if (NULL == method_add) {
            VMState_setErrorCode(op->vm, PIKA_RES_ERR_OPERATION_FAILED);
            __platform_printf("TypeError: unsupported operand +\n");
            op->res = NULL;
            return;
        }
        arg_deinit(method_add);
        PikaObj* obj2 = arg_getPtr(op->a2);
        obj_setPtr(obj1, "__others", obj2);
        /* clang-format off */
        PIKA_PYTHON(
        __res = __add__(__others)
        )
        /* clang-format on */
        const uint8_t bytes[] = {
            0x0c, 0x00, 0x00, 0x00, /* instruct array size */
            0x10, 0x81, 0x01, 0x00, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x04, 0x12,
            0x00,
            /* instruct array */
            0x18, 0x00, 0x00, 0x00, /* const pool size */
            0x00, 0x5f, 0x5f, 0x6f, 0x74, 0x68, 0x65, 0x72, 0x73, 0x00, 0x5f,
            0x5f, 0x61, 0x64, 0x64, 0x5f, 0x5f, 0x00, 0x5f, 0x5f, 0x72, 0x65,
            0x73, 0x00, /* const pool */
        };
        pikaVM_runByteCode(obj1, (uint8_t*)bytes);
        Arg* __res = arg_copy(obj_getArg(obj1, "__res"));
        op->res = __res;
        obj_removeArg(obj1, "__others");
        obj_removeArg(obj1, "__res");
        return;
    }
#endif

    if ((op->t1 == ARG_TYPE_STRING) && (op->t2 == ARG_TYPE_STRING)) {
        char* num1_s = NULL;
        char* num2_s = NULL;
        Args str_opt_buffs = {0};
        num1_s = arg_getStr(op->a1);
        num2_s = arg_getStr(op->a2);
        char* opt_str_out = strsAppend(&str_opt_buffs, num1_s, num2_s);
        op->res = arg_setStr(op->res, "", opt_str_out);
        strsDeinit(&str_opt_buffs);
        return;
    }
#if !PIKA_NANO_ENABLE
    if ((op->t1 == ARG_TYPE_BYTES) && (op->t2 == ARG_TYPE_BYTES)) {
        uint8_t* bytes1 = arg_getBytes(op->a1);
        uint8_t* bytes2 = arg_getBytes(op->a2);
        size_t size1 = arg_getBytesSize(op->a1);
        size_t size2 = arg_getBytesSize(op->a2);
        op->res = arg_setBytes(op->res, "", NULL, size1 + size2);
        uint8_t* bytes_out = arg_getBytes(op->res);
        __platform_memcpy(bytes_out, bytes1, size1);
        __platform_memcpy(bytes_out + size1, bytes2, size2);
        return;
    }
#endif
    /* match float */
    if ((op->t1 == ARG_TYPE_FLOAT) || op->t2 == ARG_TYPE_FLOAT) {
        op->res = arg_setFloat(op->res, "", op->f1 + op->f2);
        return;
    }
    /* int is default */
    op->res = arg_setInt(op->res, "", op->i1 + op->i2);
    return;
}

static void _OPT_SUB(OperatorInfo* op) {
#if !PIKA_NANO_ENABLE
    if (argType_isObject(op->t1)) {
        if (!argType_isObject(op->t2)) {
            VMState_setErrorCode(op->vm, PIKA_RES_ERR_OPERATION_FAILED);
            __platform_printf("TypeError: unsupported operand +\n");
            op->res = NULL;
            return;
        }
        PikaObj* obj1 = arg_getPtr(op->a1);
        Arg* method_sub = obj_getMethodArg(obj1, "__sub__");
        if (NULL == method_sub) {
            VMState_setErrorCode(op->vm, PIKA_RES_ERR_OPERATION_FAILED);
            __platform_printf("TypeError: unsupported operand +\n");
            op->res = NULL;
            return;
        }
        arg_deinit(method_sub);
        PikaObj* obj2 = arg_getPtr(op->a2);
        obj_setPtr(obj1, "__others", obj2);
        /* clang-format off */
        PIKA_PYTHON(
        __res = __sub__(__others)
        )
        /* clang-format on */
        const uint8_t bytes[] = {
            0x0c, 0x00, 0x00, 0x00, /* instruct array size */
            0x10, 0x81, 0x01, 0x00, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x04, 0x12,
            0x00,
            /* instruct array */
            0x18, 0x00, 0x00, 0x00, /* const pool size */
            0x00, 0x5f, 0x5f, 0x6f, 0x74, 0x68, 0x65, 0x72, 0x73, 0x00, 0x5f,
            0x5f, 0x73, 0x75, 0x62, 0x5f, 0x5f, 0x00, 0x5f, 0x5f, 0x72, 0x65,
            0x73, 0x00, /* const pool */
        };
        pikaVM_runByteCode(obj1, (uint8_t*)bytes);
        Arg* __res = arg_copy(obj_getArg(obj1, "__res"));
        op->res = __res;
        obj_removeArg(obj1, "__others");
        obj_removeArg(obj1, "__res");
        return;
    }
#endif
    if (op->t2 == ARG_TYPE_NONE) {
        if (op->t1 == ARG_TYPE_INT) {
            op->res = arg_setInt(op->res, "", -op->i1);
            return;
        }
        if (op->t1 == ARG_TYPE_FLOAT) {
            op->res = arg_setFloat(op->res, "", -op->f1);
            return;
        }
    }
    if ((op->t1 == ARG_TYPE_FLOAT) || op->t2 == ARG_TYPE_FLOAT) {
        op->res = arg_setFloat(op->res, "", op->f1 - op->f2);
        return;
    }
    op->res = arg_setInt(op->res, "", op->i1 - op->i2);
    return;
}

static void _OPT_EQU(OperatorInfo* op) {
    int8_t is_equ = -1;
    if (op->t1 == ARG_TYPE_NONE && op->t2 == ARG_TYPE_NONE) {
        is_equ = 1;
        goto exit;
    }
    /* type not equl, and type is not int or float */
    if (op->t1 != op->t2) {
        if ((op->t1 != ARG_TYPE_FLOAT) && (op->t1 != ARG_TYPE_INT)) {
            is_equ = 0;
            goto exit;
        }
        if ((op->t2 != ARG_TYPE_FLOAT) && (op->t2 != ARG_TYPE_INT)) {
            is_equ = 0;
            goto exit;
        }
    }
    /* string compire */
    if (op->t1 == ARG_TYPE_STRING) {
        is_equ = strEqu(arg_getStr(op->a1), arg_getStr(op->a2));
        goto exit;
    }
    /* bytes compire */
    if (op->t1 == ARG_TYPE_BYTES) {
        if (arg_getBytesSize(op->a1) != arg_getBytesSize(op->a2)) {
            is_equ = 0;
            goto exit;
        }
        is_equ = 1;
        for (size_t i = 0; i < arg_getBytesSize(op->a1); i++) {
            if (arg_getBytes(op->a1)[i] != arg_getBytes(op->a2)[i]) {
                is_equ = 0;
                goto exit;
            }
        }
        goto exit;
    }
    /* default: int and float */
    is_equ = ((op->f1 - op->f2) * (op->f1 - op->f2) < (pika_float)0.000001);
    goto exit;
exit:
    if (op->opt[0] == '=') {
        op->res = arg_setInt(op->res, "", is_equ);
    } else {
        op->res = arg_setInt(op->res, "", !is_equ);
    }
    return;
}

static void _OPT_POW(OperatorInfo* op) {
    if (op->num == 1) {
        op->res = arg_copy(op->a2);
        arg_setIsDoubleStarred(op->res, 1);
        return;
    }
    if (op->t1 == ARG_TYPE_INT && op->t2 == ARG_TYPE_INT) {
        int res = 1;
        for (int i = 0; i < op->i2; i++) {
            res = res * op->i1;
        }
        op->res = arg_setInt(op->res, "", res);
        return;
    } else if (op->t1 == ARG_TYPE_FLOAT && op->t2 == ARG_TYPE_INT) {
        float res = 1;
        for (int i = 0; i < op->i2; i++) {
            res = res * op->f1;
        }
        op->res = arg_setFloat(op->res, "", res);
        return;
    } else {
#if PIKA_MATH_ENABLE
        float res = 1;
        res = pow(op->f1, op->f2);
        op->res = arg_setFloat(op->res, "", res);
        return;
#else
        VMState_setErrorCode(op->vm, PIKA_RES_ERR_OPERATION_FAILED);
        __platform_printf(
            "Operation float ** float is not enabled, please set "
            "PIKA_MATH_ENABLE\n");
#endif
    }
}

static Arg* VM_instruction_handler_OPT(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    OperatorInfo op = {0};
    op.num = VMState_getInputArgNum(vm);
    arg_newReg(arg_reg1, PIKA_ARG_BUFF_SIZE);
    arg_newReg(arg_reg2, PIKA_ARG_BUFF_SIZE);
    if (op.num == 2) {
        /* tow input */
        op.a2 = stack_popArg(&(vm->stack), &arg_reg2);
        op.a1 = stack_popArg(&(vm->stack), &arg_reg1);
    } else if (op.num == 1) {
        /* only one input */
        op.a2 = stack_popArg(&(vm->stack), &arg_reg2);
        op.a1 = NULL;
    }
    /* init operator info */
    operatorInfo_init(&op, self, vm, data, arg_ret_reg);
    switch (data[0]) {
        case '+':
            _OPT_ADD(&op);
            goto exit;
        case '%':
            if ((op.t1 == ARG_TYPE_INT) && (op.t2 == ARG_TYPE_INT)) {
                op.res = arg_setInt(op.res, "", op.i1 % op.i2);
                goto exit;
            }
            VMState_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
            __platform_printf(
                "TypeError: unsupported operand type(s) for %%: 'float'\n");
            op.res = NULL;
            goto exit;
        case '-':
            _OPT_SUB(&op);
            goto exit;
    }
    if (data[1] == '=' && (data[0] == '!' || data[0] == '=')) {
        _OPT_EQU(&op);
        goto exit;
    }
    if (data[1] == 0) {
        switch (data[0]) {
            case '<':
                op.res = arg_setInt(op.res, "", op.f1 < op.f2);
                goto exit;
            case '*':
                if (op.num == 1) {
                    op.res = arg_copy(op.a2);
                    arg_setIsStarred(op.res, 1);
                    goto exit;
                }
                if ((op.t1 == ARG_TYPE_FLOAT) || op.t2 == ARG_TYPE_FLOAT) {
                    op.res = arg_setFloat(op.res, "", op.f1 * op.f2);
                    goto exit;
                }
                op.res = arg_setInt(op.res, "", op.i1 * op.i2);
                goto exit;
            case '&':
                if ((op.t1 == ARG_TYPE_INT) && (op.t2 == ARG_TYPE_INT)) {
                    op.res = arg_setInt(op.res, "", op.i1 & op.i2);
                    goto exit;
                }
                VMState_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
                __platform_printf(
                    "TypeError: unsupported operand type(s) for &: 'float'\n");
                op.res = NULL;
                goto exit;
            case '|':
                if ((op.t1 == ARG_TYPE_INT) && (op.t2 == ARG_TYPE_INT)) {
                    op.res = arg_setInt(op.res, "", op.i1 | op.i2);
                    goto exit;
                }
                VMState_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
                __platform_printf(
                    "TypeError: unsupported operand type(s) for |: 'float'\n");
                op.res = NULL;
                goto exit;
            case '~':
                if (op.t2 == ARG_TYPE_INT) {
                    op.res = arg_setInt(op.res, "", ~op.i2);
                    goto exit;
                }
                VMState_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
                __platform_printf(
                    "TypeError: unsupported operand type(s) for ~: 'float'\n");
                op.res = NULL;
                goto exit;
            case '/':
                if (0 == op.f2) {
                    VMState_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
                    args_setSysOut(vm->locals->list,
                                   "ZeroDivisionError: division by zero");
                    op.res = NULL;
                    goto exit;
                }
                op.res = arg_setFloat(op.res, "", op.f1 / op.f2);
                goto exit;
            case '>':
                op.res = arg_setInt(op.res, "", op.f1 > op.f2);
                goto exit;
        }
    }
    if (data[1] == 'i' && data[2] == 'n') {
        if (op.t1 == ARG_TYPE_STRING && op.t2 == ARG_TYPE_STRING) {
            if (strstr(arg_getStr(op.a2), arg_getStr(op.a1))) {
                op.res = arg_setInt(op.res, "", 1);
            } else {
                op.res = arg_setInt(op.res, "", 0);
            }
            goto exit;
        }
#if !PIKA_NANO_ENABLE
        if (argType_isObject(op.t2)) {
            PikaObj* obj2 = arg_getPtr(op.a2);
            Arg* __contains__ = obj_getMethodArg(obj2, "__contains__");
            if (NULL != __contains__) {
                arg_deinit(__contains__);
                /* clang-format off */
                PIKA_PYTHON(
                __res = __contains__(__others)
                )
                obj_setArg(obj2, "__others", op.a1);
                /* clang-format on */
                const uint8_t bytes[] = {
                    0x0c, 0x00, 0x00, 0x00, /* instruct array size */
                    0x10, 0x81, 0x01, 0x00, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x04,
                    0x17, 0x00,
                    /* instruct array */
                    0x1d, 0x00, 0x00, 0x00, /* const pool size */
                    0x00, 0x5f, 0x5f, 0x6f, 0x74, 0x68, 0x65, 0x72, 0x73, 0x00,
                    0x5f, 0x5f, 0x63, 0x6f, 0x6e, 0x74, 0x61, 0x69, 0x6e, 0x73,
                    0x5f, 0x5f, 0x00, 0x5f, 0x5f, 0x72, 0x65, 0x73,
                    0x00, /* const pool */
                };
                pikaVM_runByteCode(obj2, (uint8_t*)bytes);
                op.res = arg_setInt(op.res, "", obj_getInt(obj2, "__res"));
                goto exit;
            }
        }
#endif

        VMState_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
        args_setSysOut(vm->locals->list,
                       "Operation 'in' is not supported for this "
                       "type\n");
        op.res = NULL;
        goto exit;
    }
    if (data[0] == '*' && data[1] == '*') {
        _OPT_POW(&op);
        goto exit;
    }
    if (data[0] == '/' && data[1] == '/') {
        if ((op.t1 == ARG_TYPE_INT) && (op.t2 == ARG_TYPE_INT)) {
            op.res = arg_setInt(op.res, "", op.i1 / op.i2);
            goto exit;
        }
        VMState_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
        __platform_printf(
            "TypeError: unsupported operand type(s) for //: 'float'\n");
        op.res = NULL;
        goto exit;
    }
    if (data[1] == 'i' && data[2] == 's') {
#if !PIKA_NANO_ENABLE
        if (argType_isObject(op.t1) && argType_isObject(op.t2)) {
            if (arg_getPtr(op.a1) == arg_getPtr(op.a2)) {
                op.res = arg_setInt(op.res, "", 1);
            } else {
                op.res = arg_setInt(op.res, "", 0);
            }
            goto exit;
        }
#endif
        op.opt = "==";
        _OPT_EQU(&op);
        goto exit;
    }
    if (data[0] == '>' && data[1] == '=') {
        op.res = arg_setInt(
            op.res, "",
            (op.f1 > op.f2) ||
                ((op.f1 - op.f2) * (op.f1 - op.f2) < (pika_float)0.000001));
        goto exit;
    }
    if (data[0] == '<' && data[1] == '=') {
        op.res = arg_setInt(
            op.res, "",
            (op.f1 < op.f2) ||
                ((op.f1 - op.f2) * (op.f1 - op.f2) < (pika_float)0.000001));
        goto exit;
    }
    if (data[0] == '>' && data[1] == '>') {
        if ((op.t1 == ARG_TYPE_INT) && (op.t2 == ARG_TYPE_INT)) {
            op.res = arg_setInt(op.res, "", op.i1 >> op.i2);
            goto exit;
        }
        VMState_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
        __platform_printf(
            "TypeError: unsupported operand type(s) for >>: 'float'\n");
        op.res = NULL;
        goto exit;
    }
    if (data[0] == '<' && data[1] == '<') {
        if ((op.t1 == ARG_TYPE_INT) && (op.t2 == ARG_TYPE_INT)) {
            op.res = arg_setInt(op.res, "", op.i1 << op.i2);
            goto exit;
        }
        VMState_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
        __platform_printf(
            "TypeError: unsupported operand type(s) for <<: 'float'\n");
        op.res = NULL;
        goto exit;
    }
    if (data[0] == ' ' && data[1] == 'a' && data[2] == 'n' && data[3] == 'd' &&
        data[4] == ' ') {
        op.res = arg_setInt(op.res, "", op.i1 && op.i2);
        goto exit;
    }
    if (data[0] == ' ' && data[1] == 'o' && data[2] == 'r' && data[3] == ' ' &&
        data[4] == 0) {
        op.res = arg_setInt(op.res, "", op.i1 || op.i2);
        goto exit;
    }
    if (data[0] == ' ' && data[1] == 'n' && data[2] == 'o' && data[3] == 't' &&
        data[4] == ' ' && data[5] == 0) {
        op.res = arg_setInt(op.res, "", !op.i2);
        goto exit;
    }
exit:
    if (NULL != op.a1) {
        arg_deinit(op.a1);
    }
    if (NULL != op.a2) {
        arg_deinit(op.a2);
    }
    if (NULL != op.res) {
        return op.res;
    }
    return NULL;
}

static Arg* __VM_instruction_handler_DEF(PikaObj* self,
                                         VMState* vm,
                                         char* data,
                                         uint8_t is_class) {
    int thisBlockDeepth = VMState_getBlockDeepthNow(vm);

    PikaObj* hostObj = vm->locals;
    uint8_t is_in_class = 0;
    /* use RunAs object */
    if (obj_getFlag(vm->locals, OBJ_FLAG_RUN_AS)) {
        hostObj = args_getPtr(vm->locals->list, "@r");
        is_in_class = 1;
    }
    int offset = 0;
    /* byteCode */
    while (1) {
        InstructUnit* ins_unit_now =
            VMState_getInstructUnitWithOffset(vm, offset);
        if (!instructUnit_getIsNewLine(ins_unit_now)) {
            offset += instructUnit_getSize();
            continue;
        }
        if (instructUnit_getBlockDeepth(ins_unit_now) == thisBlockDeepth + 1) {
            if (is_in_class) {
                class_defineObjectMethod(hostObj, data, (Method)ins_unit_now,
                                         self, vm->bytecode_frame);
            } else {
                if (is_class) {
                    class_defineRunTimeConstructor(hostObj, data,
                                                   (Method)ins_unit_now, self,
                                                   vm->bytecode_frame);
                } else {
                    class_defineStaticMethod(hostObj, data,
                                             (Method)ins_unit_now, self,
                                             vm->bytecode_frame);
                }
            }
            break;
        }
        offset += instructUnit_getSize();
    }

    return NULL;
}

static Arg* VM_instruction_handler_DEF(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    return __VM_instruction_handler_DEF(self, vm, data, 0);
}

static Arg* VM_instruction_handler_CLS(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    return __VM_instruction_handler_DEF(self, vm, data, 1);
}

static Arg* VM_instruction_handler_RIS(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
#if PIKA_NANO_ENABLE
    return NULL;
#endif
    Arg* err_arg = stack_popArg_alloc(&(vm->stack));
    PIKA_RES err = (PIKA_RES)arg_getInt(err_arg);
    VMState_setErrorCode(vm, err);
    arg_deinit(err_arg);
    return NULL;
}

static Arg* VM_instruction_handler_ASS(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
#if PIKA_NANO_ENABLE
    return NULL;
#endif
    arg_newReg(reg1, PIKA_ARG_BUFF_SIZE);
    arg_newReg(reg2, PIKA_ARG_BUFF_SIZE);
    Arg* arg1 = NULL;
    Arg* arg2 = NULL;
    Arg* res = NULL;
    int n_arg = VMState_getInputArgNum(vm);
    if (n_arg == 1) {
        arg1 = stack_popArg(&vm->stack, &reg1);
    }
    if (n_arg == 2) {
        arg2 = stack_popArg(&vm->stack, &reg2);
        arg1 = stack_popArg(&vm->stack, &reg1);
    }
    /* assert failed */
    if (arg_getType(arg1) == ARG_TYPE_INT && arg_getInt(arg1) == 0) {
        stack_pushArg(&vm->stack, arg_newInt(PIKA_RES_ERR_ASSERT));
        res = VM_instruction_handler_RIS(self, vm, data, arg_ret_reg);
        if (vm->run_state->try_state == TRY_STATE_NONE) {
            if (n_arg == 1) {
                __platform_printf("AssertionError\n");
            }
            if (n_arg == 2) {
                __platform_printf("AssertionError: %s\n", arg_getStr(arg2));
            }
        }
        goto exit;
    }
exit:
    arg_deinit(arg1);
    if (NULL != arg2) {
        arg_deinit(arg2);
    }
    return res;
}

static Arg* VM_instruction_handler_NEL(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    int thisBlockDeepth = VMState_getBlockDeepthNow(vm);
    if (0 == vm->ireg[thisBlockDeepth]) {
        /* set __else flag */
        vm->jmp = fast_atoi(data);
    }
    return NULL;
}

static Arg* VM_instruction_handler_DEL(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    if (_checkLReg(data)) {
        uint8_t reg_index = _getLRegIndex(data);
        VMState_delLReg(vm, reg_index);
        return NULL;
    }
    if (obj_isArgExist(vm->locals, data)) {
        obj_removeArg(vm->locals, data);
        return NULL;
    }
    if (obj_isArgExist(vm->globals, data)) {
        obj_removeArg(vm->globals, data);
        return NULL;
    }
    VMState_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
    __platform_printf("NameError: name '%s' is not defined\n", data);
    return NULL;
}

static Arg* VM_instruction_handler_EST(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    Arg* arg = obj_getArg(vm->locals, data);
    if (arg == NULL) {
        return arg_setInt(arg_ret_reg, "", 0);
    }
    if (ARG_TYPE_NONE == arg_getType(arg)) {
        return arg_setInt(arg_ret_reg, "", 0);
    }
    return arg_setInt(arg_ret_reg, "", 1);
}

static Arg* VM_instruction_handler_BRK(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    /* break jmp signal */
    vm->jmp = VM_JMP_BREAK;
    return NULL;
}

static Arg* VM_instruction_handler_CTN(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    /* continue jmp signal */
    vm->jmp = VM_JMP_CONTINUE;
    return NULL;
}

static Arg* VM_instruction_handler_GLB(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    Arg* global_list_buff = NULL;
    char* global_list = args_getStr(vm->locals->list, "@g");
    /* create new global_list */
    if (NULL == global_list) {
        args_setStr(vm->locals->list, "@g", data);
        obj_setFlag(vm->locals, OBJ_FLAG_GLOBALS);
        goto exit;
    }
    /* append to exist global_list */
    global_list_buff = arg_newStr(global_list);
    global_list_buff = arg_strAppend(global_list_buff, ",");
    global_list_buff = arg_strAppend(global_list_buff, data);
    args_setStr(vm->locals->list, "@g", arg_getStr(global_list_buff));
    goto exit;
exit:
    if (NULL != global_list_buff) {
        arg_deinit(global_list_buff);
    }
    return NULL;
}

static Arg* VM_instruction_handler_IMP(PikaObj* self,
                                       VMState* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    /* the module is already imported, skip. */
    if (obj_isArgExist(self, data)) {
        return NULL;
    }
    /* find cmodule in root object */
    extern volatile PikaObj* __pikaMain;
    if (obj_isArgExist((PikaObj*)__pikaMain, data)) {
        obj_setArg(self, data, obj_getArg((PikaObj*)__pikaMain, data));
        return NULL;
    }
    /* import module from '@lib' */
    if (0 == obj_importModule(self, data)) {
        return NULL;
    }
    VMState_setErrorCode(vm, PIKA_RES_ERR_ARG_NO_FOUND);
    __platform_printf("ModuleNotFoundError: No module named '%s'\r\n", data);
    return NULL;
}

const VM_instruct_handler VM_instruct_handler_table[__INSTRCUTION_CNT] = {
#define __INS_TABLE
#include "__instruction_table.cfg"
};

enum Instruct pikaVM_getInstructFromAsm(char* ins_str) {
#define __INS_COMPIRE
#include "__instruction_table.cfg"
    return NON;
}

static int pikaVM_runInstructUnit(PikaObj* self,
                                  VMState* vm,
                                  InstructUnit* ins_unit) {
    enum Instruct instruct = instructUnit_getInstruct(ins_unit);
    arg_newReg(ret_reg, PIKA_ARG_BUFF_SIZE);
    Arg* return_arg = &ret_reg;
    // char invode_deepth1_str[2] = {0};
    int32_t pc_next = vm->pc + instructUnit_getSize();
    char* data = VMState_getConstWithInstructUnit(vm, ins_unit);
    /* run instruct */
    pika_assert(NULL != vm->run_state);
    return_arg = VM_instruct_handler_table[instruct](self, vm, data, &ret_reg);

    if (vm->error_code != PIKA_RES_OK ||
        VMSignal_getCtrl() == VM_SIGNAL_CTRL_EXIT) {
        /* raise jmp */
        if (vm->run_state->try_state == TRY_STATE_INNER) {
            vm->jmp = VM_JMP_RAISE;
        } else {
            /* exit */
            vm->jmp = VM_JMP_EXIT;
        }
    }

    if (NULL != return_arg) {
        stack_pushArg(&(vm->stack), return_arg);
    }
    goto nextLine;
nextLine:
    /* exit */
    if (VM_JMP_EXIT == vm->jmp) {
        pc_next = VM_PC_EXIT;
        goto exit;
    }
    /* break */
    if (VM_JMP_BREAK == vm->jmp) {
        pc_next = vm->pc + VMState_getAddrOffsetOfBreak(vm);
        goto exit;
    }
    /* continue */
    if (VM_JMP_CONTINUE == vm->jmp) {
        pc_next = vm->pc + VMState_getAddrOffsetOfContinue(vm);
        goto exit;
    }
/* raise */
#if !PIKA_NANO_ENABLE
    if (VM_JMP_RAISE == vm->jmp) {
        int offset = VMState_getAddrOffsetOfRaise(vm);
        if (0 == offset) {
            /* can not found end of try, return */
            pc_next = VM_PC_EXIT;
            vm->run_state->try_result = TRY_RESULT_RAISE;
            goto exit;
        }
        pc_next = vm->pc + offset;
        goto exit;
    }
#endif
    /* static jmp */
    if (vm->jmp != 0) {
        pc_next = vm->pc + VMState_getAddrOffsetFromJmp(vm);
        goto exit;
    }
    /* not jmp */
    pc_next = vm->pc + instructUnit_getSize();

    /* jump to next line */
    if (vm->error_code != 0) {
        while (1) {
            if (pc_next >= (int)VMState_getInstructArraySize(vm)) {
                pc_next = VM_PC_EXIT;
                goto exit;
            }
            InstructUnit* ins_next = instructArray_getByOffset(
                &vm->bytecode_frame->instruct_array, pc_next);
            if (instructUnit_getIsNewLine(ins_next)) {
                goto exit;
            }
            pc_next = pc_next + instructUnit_getSize();
        }
    }

    goto exit;
exit:
    vm->jmp = 0;
    /* reach the end */
    if (pc_next >= (int)VMState_getInstructArraySize(vm)) {
        return VM_PC_EXIT;
    }
    return pc_next;
}

VMParameters* pikaVM_runAsm(PikaObj* self, char* pikaAsm) {
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    VMParameters* res = pikaVM_runByteCodeFrame(self, &bytecode_frame);
    byteCodeFrame_deinit(&bytecode_frame);
    return res;
}

static ByteCodeFrame* _cache_bytecodeframe(PikaObj* self) {
    ByteCodeFrame bytecode_frame_stack = {0};
    ByteCodeFrame* res = NULL;
    if (!obj_isArgExist(self, "@bcn")) {
        /* start form @bc0 */
        obj_setInt(self, "@bcn", 0);
    }
    int bcn = obj_getInt(self, "@bcn");
    char bcn_str[] = "@bcx";
    bcn_str[3] = '0' + bcn;
    /* load bytecode to heap */
    args_setHeapStruct(self->list, bcn_str, bytecode_frame_stack,
                       byteCodeFrame_deinit);
    /* get bytecode_ptr from heap */
    res = args_getHeapStruct(self->list, bcn_str);
    obj_setInt(self, "@bcn", bcn + 1);
    return res;
}

static ByteCodeFrame* _cache_bcf_fn(PikaObj* self, char* py_lines) {
    /* cache 'def' and 'class' to heap */
    if ((NULL == strstr(py_lines, "def ")) &&
        (NULL == strstr(py_lines, "class "))) {
        return NULL;
    }
    return _cache_bytecodeframe(self);
}

static char* _get_data_from_bytecode2(uint8_t* bytecode,
                                      enum Instruct ins1,
                                      enum Instruct ins2) {
    ByteCodeFrame bf = {0};
    char* res = NULL;
    byteCodeFrame_init(&bf);
    byteCodeFrame_loadByteCode(&bf, bytecode);
    while (1) {
        InstructUnit* ins_unit = instructArray_getNow(&bf.instruct_array);
        if (NULL == ins_unit) {
            goto __exit;
        }
        enum Instruct ins = instructUnit_getInstruct(ins_unit);
        if (ins == ins1 || ins == ins2) {
            res = constPool_getByOffset(&bf.const_pool,
                                        ins_unit->const_pool_index);
            goto __exit;
        }
        instructArray_getNext(&bf.instruct_array);
    }
__exit:
    byteCodeFrame_deinit(&bf);
    return res;
}

static ByteCodeFrame* _cache_bcf_fn_bc(PikaObj* self, uint8_t* bytecode) {
    /* save 'def' and 'class' to heap */
    if (NULL == _get_data_from_bytecode2(bytecode, DEF, CLS)) {
        return NULL;
    }
    return _cache_bytecodeframe(self);
}

static VMParameters* __pikaVM_runPyLines(PikaObj* self, char* py_lines) {
    VMParameters* globals = NULL;
    ByteCodeFrame bytecode_frame_stack = {0};
    ByteCodeFrame* bytecode_frame_p = NULL;
    uint8_t is_use_heap_bytecode = 0;

    /*
     * the first obj_run, cache bytecode to heap, to support 'def' and
     * 'class'
     */
    bytecode_frame_p = _cache_bcf_fn(self, py_lines);
    if (NULL == bytecode_frame_p) {
        is_use_heap_bytecode = 0;
        /* get bytecode_ptr from stack */
        bytecode_frame_p = &bytecode_frame_stack;
    }

    /* load or generate byte code frame */
    /* generate byte code */
    byteCodeFrame_init(bytecode_frame_p);
    if (PIKA_RES_OK != Parser_linesToBytes(bytecode_frame_p, py_lines)) {
        __platform_printf("Error: Syntax error.\r\n");
        globals = NULL;
        goto exit;
    }
    /* run byteCode */
    globals = pikaVM_runByteCodeFrame(self, bytecode_frame_p);
    goto exit;
exit:
    if (!is_use_heap_bytecode) {
        byteCodeFrame_deinit(&bytecode_frame_stack);
    }
    return globals;
}

VMParameters* _do_pikaVM_runByteCode(PikaObj* self,
                                     VMParameters* locals,
                                     VMParameters* globals,
                                     uint8_t* bytecode,
                                     RunState* run_state,
                                     PIKA_BOOL is_const_bytecode) {
    ByteCodeFrame bytecode_frame_stack = {0};
    ByteCodeFrame* bytecode_frame_p = NULL;
    uint8_t is_use_heap_bytecode = 1;
    /*
     * the first obj_run, cache bytecode to heap, to support 'def' and
     * 'class'
     */
    bytecode_frame_p = _cache_bcf_fn_bc(self, bytecode);
    if (NULL == bytecode_frame_p) {
        is_use_heap_bytecode = 0;
        /* get bytecode_ptr from stack */
        bytecode_frame_p = &bytecode_frame_stack;
        /* no def/class ins, no need cache bytecode */
        is_const_bytecode = 1;
    }

    /* load or generate byte code frame */
    /* load bytecode */
    _do_byteCodeFrame_loadByteCode(bytecode_frame_p, bytecode,
                                   is_const_bytecode);

    /* run byteCode */

    globals = __pikaVM_runByteCodeFrameWithState(
        self, locals, globals, bytecode_frame_p, 0, run_state);
    goto exit;
exit:
    if (!is_use_heap_bytecode) {
        byteCodeFrame_deinit(&bytecode_frame_stack);
    }
    return globals;
}

VMParameters* pikaVM_runByteCodeFile(PikaObj* self, char* filename) {
    Args buffs = {0};
    Arg* file_arg = arg_loadFile(NULL, filename);
    pika_assert(NULL != file_arg);
    if (NULL == file_arg) {
        return NULL;
    }
    uint8_t* lines = arg_getBytes(file_arg);
    /* clear the void line */
    VMParameters* res = pikaVM_runByteCodeInconstant(self, lines);
    arg_deinit(file_arg);
    strsDeinit(&buffs);
    return res;
}

VMParameters* pikaVM_runSingleFile(PikaObj* self, char* filename) {
    Args buffs = {0};
    Arg* file_arg = arg_loadFile(NULL, filename);
    pika_assert(NULL != file_arg);
    if (NULL == file_arg) {
        return NULL;
    }
    char* lines = (char*)arg_getBytes(file_arg);
    /* clear the void line */
    VMParameters* res = pikaVM_run(self, lines);
    arg_deinit(file_arg);
    strsDeinit(&buffs);
    return res;
}

VMParameters* pikaVM_run(PikaObj* self, char* py_lines) {
    return __pikaVM_runPyLines(self, py_lines);
}

VMParameters* pikaVM_runByteCode(PikaObj* self, const uint8_t* bytecode) {
    RunState run_state = {.try_state = TRY_STATE_NONE,
                          .try_result = TRY_RESULT_NONE};
    return _do_pikaVM_runByteCode(self, self, self, (uint8_t*)bytecode,
                                  &run_state, PIKA_TRUE);
}

VMParameters* pikaVM_runByteCodeInconstant(PikaObj* self, uint8_t* bytecode) {
    RunState run_state = {.try_state = TRY_STATE_NONE,
                          .try_result = TRY_RESULT_NONE};
    return _do_pikaVM_runByteCode(self, self, self, (uint8_t*)bytecode,
                                  &run_state, PIKA_FALSE);
}

void constPool_update(ConstPool* self) {
    self->content_start = (void*)arg_getContent(self->arg_buff);
}

void constPool_init(ConstPool* self) {
    self->arg_buff = NULL;
    self->content_start = NULL;
    self->content_offset_now = 0;
    self->size = 1;
    self->output_redirect_fun = NULL;
    self->output_f = NULL;
}

void constPool_deinit(ConstPool* self) {
    if (NULL != self->arg_buff) {
        arg_deinit(self->arg_buff);
    }
}

void constPool_append(ConstPool* self, char* content) {
    if (NULL == self->arg_buff) {
        self->arg_buff = arg_newStr("");
    }
    uint16_t size = strGetSize(content) + 1;
    if (NULL == self->output_redirect_fun) {
        self->arg_buff = arg_append(self->arg_buff, content, size);
    } else {
        self->output_redirect_fun(self, content);
    };
    constPool_update(self);
    self->size += size;
}

char* constPool_getNow(ConstPool* self) {
    if (self->content_offset_now >= self->size) {
        /* is the end */
        return NULL;
    }
    return (char*)((uintptr_t)constPool_getStart(self) +
                   (uintptr_t)(self->content_offset_now));
}

uint16_t constPool_getOffsetByData(ConstPool* self, char* data) {
    uint16_t ptr_befor = self->content_offset_now;
    /* set ptr_now to begin */
    self->content_offset_now = 0;
    uint16_t offset_out = 65535;
    if (self->arg_buff == NULL) {
        goto __exit;
    }
    while (1) {
        if (NULL == constPool_getNext(self)) {
            goto __exit;
        }
        if (strEqu(data, constPool_getNow(self))) {
            offset_out = self->content_offset_now;
            goto __exit;
        }
    }
__exit:
    /* retore ptr_now */
    self->content_offset_now = ptr_befor;
    return offset_out;
}

char* constPool_getNext(ConstPool* self) {
    self->content_offset_now += strGetSize(constPool_getNow(self)) + 1;
    return constPool_getNow(self);
}

char* constPool_getByIndex(ConstPool* self, uint16_t index) {
    uint16_t ptr_befor = self->content_offset_now;
    /* set ptr_now to begin */
    self->content_offset_now = 0;
    for (uint16_t i = 0; i < index; i++) {
        constPool_getNext(self);
    }
    char* res = constPool_getNow(self);
    /* retore ptr_now */
    self->content_offset_now = ptr_befor;
    return res;
}

void constPool_print(ConstPool* self) {
    uint16_t ptr_befor = self->content_offset_now;
    /* set ptr_now to begin */
    self->content_offset_now = 0;
    while (1) {
        if (NULL == constPool_getNext(self)) {
            goto exit;
        }
        uint16_t offset = self->content_offset_now;
        __platform_printf("%d: %s\r\n", offset, constPool_getNow(self));
    }
exit:
    /* retore ptr_now */
    self->content_offset_now = ptr_befor;
    return;
}

void byteCodeFrame_init(ByteCodeFrame* self) {
    /* init to support append,
       if only load static bytecode,
       can not init */
    constPool_init(&(self->const_pool));
    instructArray_init(&(self->instruct_array));
}

extern const char magic_code_pyo[4];
void _do_byteCodeFrame_loadByteCode(ByteCodeFrame* self,
                                    uint8_t* bytes,
                                    PIKA_BOOL is_const) {
    if (bytes[0] == magic_code_pyo[0] && bytes[1] == magic_code_pyo[1] &&
        bytes[2] == magic_code_pyo[2] && bytes[3] == magic_code_pyo[3]) {
        /* load from file, found magic code, skip head */
        bytes = bytes + sizeof(magic_code_pyo) + sizeof(uint32_t);
    }
    uint32_t* ins_size_p = (uint32_t*)bytes;
    void* ins_start_p = (uint32_t*)((uintptr_t)bytes + sizeof(*ins_size_p));
    uint32_t* const_size_p =
        (uint32_t*)((uintptr_t)ins_start_p + (uintptr_t)(*ins_size_p));
    self->instruct_array.size = *ins_size_p;
    self->instruct_array.content_start = ins_start_p;
    self->const_pool.size = *const_size_p;
    self->const_pool.content_start =
        (char*)((uintptr_t)const_size_p + sizeof(*const_size_p));
    if (!is_const) {
        pika_assert(NULL == self->instruct_array.arg_buff);
        pika_assert(NULL == self->instruct_array.arg_buff);
        self->instruct_array.arg_buff = arg_newBytes(ins_start_p, *ins_size_p);
        self->const_pool.arg_buff =
            arg_newBytes(self->const_pool.content_start, *const_size_p);
        self->instruct_array.content_start =
            arg_getBytes(self->instruct_array.arg_buff);
        self->const_pool.content_start =
            arg_getBytes(self->const_pool.arg_buff);
    }
}

static void byteCodeFrame_loadByteCodeInconst(ByteCodeFrame* self,
                                              uint8_t* bytes) {
    _do_byteCodeFrame_loadByteCode(self, bytes, PIKA_FALSE);
}

void byteCodeFrame_loadByteCode(ByteCodeFrame* self, uint8_t* bytes) {
    _do_byteCodeFrame_loadByteCode(self, bytes, PIKA_TRUE);
}

void byteCodeFrame_deinit(ByteCodeFrame* self) {
    constPool_deinit(&(self->const_pool));
    instructArray_deinit(&(self->instruct_array));
}

void instructArray_init(InstructArray* self) {
    self->arg_buff = NULL;
    self->content_start = NULL;
    self->size = 0;
    self->content_offset_now = 0;
    self->output_redirect_fun = NULL;
    self->output_f = NULL;
}

void instructArray_deinit(InstructArray* self) {
    if (NULL != self->arg_buff) {
        arg_deinit(self->arg_buff);
    }
}

void instructArray_append(InstructArray* self, InstructUnit* ins_unit) {
    if (NULL == self->arg_buff) {
        self->arg_buff = arg_newNull();
    }
    if (NULL == self->output_redirect_fun) {
        self->arg_buff =
            arg_append(self->arg_buff, ins_unit, instructUnit_getSize());
    } else {
        self->output_redirect_fun(self, ins_unit);
    };
    instructArray_update(self);
    self->size += instructUnit_getSize();
}

void instructUnit_init(InstructUnit* ins_unit) {
    ins_unit->deepth = 0;
    ins_unit->const_pool_index = 0;
    ins_unit->isNewLine_instruct = 0;
}

void instructArray_update(InstructArray* self) {
    self->content_start = (void*)arg_getContent(self->arg_buff);
}

InstructUnit* instructArray_getNow(InstructArray* self) {
    if (self->content_offset_now >= self->size) {
        /* is the end */
        return NULL;
    }
    return (InstructUnit*)((uintptr_t)instructArray_getStart(self) +
                           (uintptr_t)(self->content_offset_now));
}

InstructUnit* instructArray_getNext(InstructArray* self) {
    self->content_offset_now += instructUnit_getSize();
    return instructArray_getNow(self);
}

static char* instructUnit_getInstructStr(InstructUnit* self) {
#define __INS_GET_INS_STR
#include "__instruction_table.cfg"
    return "NON";
}

void instructUnit_print(InstructUnit* self) {
    if (instructUnit_getIsNewLine(self)) {
        __platform_printf("B%d\r\n", instructUnit_getBlockDeepth(self));
    }
    __platform_printf("%d %s #%d\r\n", instructUnit_getInvokeDeepth(self),
                      instructUnit_getInstructStr(self),
                      self->const_pool_index);
}

static void instructUnit_printWithConst(InstructUnit* self,
                                        ConstPool* const_pool) {
    // if (instructUnit_getIsNewLine(self)) {
    //     __platform_printf("B%d\r\n", instructUnit_getBlockDeepth(self));
    // }
    __platform_printf("%s %s \t\t(#%d)\r\n", instructUnit_getInstructStr(self),
                      constPool_getByOffset(const_pool, self->const_pool_index),
                      self->const_pool_index);
}

void instructArray_printWithConst(InstructArray* self, ConstPool* const_pool) {
    uint16_t offset_befor = self->content_offset_now;
    self->content_offset_now = 0;
    while (1) {
        InstructUnit* ins_unit = instructArray_getNow(self);
        if (NULL == ins_unit) {
            goto exit;
        }
        instructUnit_printWithConst(ins_unit, const_pool);
        instructArray_getNext(self);
    }
exit:
    self->content_offset_now = offset_befor;
    return;
}

void instructArray_print(InstructArray* self) {
    uint16_t offset_befor = self->content_offset_now;
    self->content_offset_now = 0;
    while (1) {
        InstructUnit* ins_unit = instructArray_getNow(self);
        if (NULL == ins_unit) {
            goto exit;
        }
        instructUnit_print(ins_unit);
        instructArray_getNext(self);
    }
exit:
    self->content_offset_now = offset_befor;
    return;
}

void instructArray_printAsArray(InstructArray* self) {
    uint16_t offset_befor = self->content_offset_now;
    self->content_offset_now = 0;
    uint8_t line_num = 12;
    uint16_t g_i = 0;
    uint8_t* ins_size_p = (uint8_t*)&self->size;
    __platform_printf("0x%02x, ", *(ins_size_p));
    __platform_printf("0x%02x, ", *(ins_size_p + (uintptr_t)1));
    __platform_printf("0x%02x, ", *(ins_size_p + (uintptr_t)2));
    __platform_printf("0x%02x, ", *(ins_size_p + (uintptr_t)3));
    __platform_printf("/* instruct array size */\n");
    while (1) {
        InstructUnit* ins_unit = instructArray_getNow(self);
        if (NULL == ins_unit) {
            goto exit;
        }
        for (int i = 0; i < (int)instructUnit_getSize(); i++) {
            g_i++;
            __platform_printf("0x%02x, ", *((uint8_t*)ins_unit + (uintptr_t)i));
            if (g_i % line_num == 0) {
                __platform_printf("\n");
            }
        }
        instructArray_getNext(self);
    }
exit:
    __platform_printf("/* instruct array */\n");
    self->content_offset_now = offset_befor;
    return;
}

size_t byteCodeFrame_getSize(ByteCodeFrame* bf) {
    return bf->const_pool.size + bf->instruct_array.size;
}

void byteCodeFrame_print(ByteCodeFrame* self) {
    constPool_print(&(self->const_pool));
    instructArray_printWithConst(&(self->instruct_array), &(self->const_pool));
    __platform_printf("---------------\r\n");
    __platform_printf("byte code size: %d\r\n",
                      self->const_pool.size + self->instruct_array.size);
}

void VMState_solveUnusedStack(VMState* vm) {
    uint8_t top = stack_getTop(&(vm->stack));
    for (int i = 0; i < top; i++) {
        Arg* arg = stack_popArg_alloc(&(vm->stack));
        ArgType type = arg_getType(arg);
        if (type == ARG_TYPE_NONE) {
            arg_deinit(arg);
            continue;
        }
        if (vm->line_error_code != 0) {
            arg_deinit(arg);
            continue;
        }
        arg_singlePrint(arg, PIKA_TRUE, "\r\n");
        arg_deinit(arg);
    }
}

static VMParameters* __pikaVM_runByteCodeFrameWithState(
    PikaObj* self,
    VMParameters* locals,
    VMParameters* globals,
    ByteCodeFrame* bytecode_frame,
    uint16_t pc,
    RunState* run_state) {
    pika_assert(NULL != run_state);
    int size = bytecode_frame->instruct_array.size;
    /* locals is the local scope */
    VMState vm = {
        .bytecode_frame = bytecode_frame,
        .locals = locals,
        .globals = globals,
        .jmp = 0,
        .pc = pc,
        .loop_deepth = 0,
        .error_code = PIKA_RES_OK,
        .line_error_code = PIKA_RES_OK,
        .try_error_code = PIKA_RES_OK,
        .run_state = run_state,
        .ins_cnt = 0,
        .in_super = PIKA_FALSE,
        .super_invoke_deepth = 0,
    };
    stack_init(&(vm.stack));
    VMState_initReg(&vm);
    if (PikaVMSignal.vm_cnt == 0) {
        pks_vmSignal_setCtrlElear();
    }
    PikaVMSignal.vm_cnt++;
    while (vm.pc < size) {
        if (vm.pc == VM_PC_EXIT) {
            break;
        }
        InstructUnit* this_ins_unit = VMState_getInstructNow(&vm);
        uint8_t is_new_line = instructUnit_getIsNewLine(this_ins_unit);
        if (is_new_line) {
            VMState_solveUnusedStack(&vm);
            stack_reset(&(vm.stack));
            vm.error_code = 0;
            vm.line_error_code = 0;
        }
        vm.pc = pikaVM_runInstructUnit(self, &vm, this_ins_unit);
        vm.ins_cnt++;
#if PIKA_INSTRUCT_HOOK_ENABLE
        if (vm.ins_cnt % PIKA_INSTRUCT_HOOK_PERIOD == 0) {
            __pks_hook_instruct();
        }
#endif
#if PIKA_EVENT_ENABLE
        VMSignale_pickupEvent();
#endif
        if (0 != vm.error_code) {
            vm.line_error_code = vm.error_code;
            InstructUnit* head_ins_unit = this_ins_unit;
            /* get first ins of a line */
            while (1) {
                if (instructUnit_getIsNewLine(head_ins_unit)) {
                    break;
                }
                head_ins_unit--;
            }
            if (vm.run_state->try_state) {
                vm.try_error_code = vm.error_code;
            }
            /* print inses of a line */
            if (!vm.run_state->try_state) {
                while (1) {
                    if (head_ins_unit != this_ins_unit) {
                        __platform_printf("   ");
                    } else {
                        __platform_printf(" -> ");
                    }
                    instructUnit_printWithConst(head_ins_unit,
                                                &(bytecode_frame->const_pool));
                    head_ins_unit++;
                    if (head_ins_unit > this_ins_unit) {
                        break;
                    }
                }
            }
            __platform_error_handle();
            vm.error_code = 0;
        }
    }
    VMState_solveUnusedStack(&vm);
    stack_deinit(&(vm.stack));
    PikaVMSignal.vm_cnt--;
    return locals;
}

VMParameters* pikaVM_runByteCodeFrame(PikaObj* self,
                                      ByteCodeFrame* byteCode_frame) {
    RunState run_state = {.try_state = TRY_STATE_NONE,
                          .try_result = TRY_RESULT_NONE};
    return __pikaVM_runByteCodeFrameWithState(self, self, self, byteCode_frame,
                                              0, &run_state);
}

void constPool_printAsArray(ConstPool* self) {
    uint8_t* const_size_str = (uint8_t*)&(self->size);
    __platform_printf("0x%02x, ", *(const_size_str));
    __platform_printf("0x%02x, ", *(const_size_str + (uintptr_t)1));
    __platform_printf("0x%02x, ", *(const_size_str + (uintptr_t)2));
    __platform_printf("0x%02x, ", *(const_size_str + (uintptr_t)3));
    __platform_printf("/* const pool size */\n");
    uint16_t ptr_befor = self->content_offset_now;
    uint8_t line_num = 12;
    uint16_t g_i = 0;
    /* set ptr_now to begin */
    self->content_offset_now = 0;
    __platform_printf("0x00, ");
    while (1) {
        if (NULL == constPool_getNext(self)) {
            goto exit;
        }
        char* data_each = constPool_getNow(self);
        /* todo start */
        size_t len = strlen(data_each);
        for (uint32_t i = 0; i < len + 1; i++) {
            __platform_printf("0x%02x, ", *(data_each + (uintptr_t)i));
            g_i++;
            if (g_i % line_num == 0) {
                __platform_printf("\n");
            }
        }
        /* todo end */
    }
exit:
    /* retore ptr_now */
    __platform_printf("/* const pool */\n");
    self->content_offset_now = ptr_befor;
    return;
}

void byteCodeFrame_printAsArray(ByteCodeFrame* self) {
    __platform_printf("const uint8_t bytes[] = {\n");
    instructArray_printAsArray(&(self->instruct_array));
    constPool_printAsArray(&(self->const_pool));
    __platform_printf("};\n");
    __platform_printf("pikaVM_runByteCode(self, (uint8_t*)bytes);\n");
}

PikaObj* pikaVM_runFile(PikaObj* self, char* file_name) {
    Args buffs = {0};
    char* module_name = strsCopy(&buffs, file_name);
    strPopLastToken(module_name, '.');

    __platform_printf("(pikascript) pika compiler:\r\n");
    PikaMaker* maker = New_PikaMaker();
    pikaMaker_compileModuleWithDepends(maker, module_name);
    pikaMaker_linkCompiledModules(maker, "pikaModules_cache.py.a");
    pikaMaker_deinit(maker);
    __platform_printf("(pikascript) all succeed.\r\n\r\n");

    pikaMemMaxReset();
    Obj_linkLibraryFile(self, "pikascript-api/pikaModules_cache.py.a");
    self = pikaVM_runSingleFile(self, file_name);
    strsDeinit(&buffs);
    return self;
}
