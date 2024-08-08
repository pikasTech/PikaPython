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

#include "PikaVM.h"
#include "BaseObj.h"
#include "PikaCompiler.h"
#include "PikaObj.h"
#include "PikaParser.h"
#include "PikaPlatform.h"
#include "dataArg.h"
#include "dataStrs.h"
#if PIKA_MATH_ENABLE
#include <math.h>
#endif

static pika_thread_recursive_mutex_t g_pikaGIL = {0};
volatile VMState g_PikaVMState = {
    .signal_ctrl = VM_SIGNAL_CTRL_NONE,
    .vm_cnt = 0,
#if PIKA_EVENT_ENABLE
    .cq =
        {
            .head = 0,
            .tail = 0,
            .res = {0},
        },
    .sq =
        {
            .head = 0,
            .tail = 0,
            .res = {0},
        },
    .event_pickup_cnt = 0,
    .event_thread = NULL,
    .event_thread_exit = pika_false,
#endif
#if PIKA_DEBUG_BREAK_POINT_MAX > 0
    .break_module_hash = {0},
    .break_point_pc = {0},
    .break_point_cnt = 0,
#endif
};
extern volatile PikaObjState g_PikaObjState;

/* private */
static pika_bool _checkLReg(char* data);
static uint8_t _getLRegIndex(char* data);
static PikaObj* New_Locals(Args* args);
char* string_slice(Args* outBuffs, char* str, int start, int end);

pika_bool pika_GIL_isInit(void) {
    return g_pikaGIL.mutex.is_init;
}

int pika_GIL_ENTER(void) {
    if (!g_pikaGIL.mutex.is_init) {
        if (g_pikaGIL.mutex.bare_lock > 0) {
            return -1;
        }
        g_pikaGIL.mutex.bare_lock = 1;
        return 0;
    }
    int ret = pika_thread_recursive_mutex_lock(&g_pikaGIL);
    // pika_debug("pika_GIL_ENTER");
    if (!g_pikaGIL.mutex.is_first_lock) {
        g_pikaGIL.mutex.is_first_lock = 1;
    }
    return ret;
}

int pika_GIL_getBareLock(void) {
    return g_pikaGIL.mutex.bare_lock;
}

int pika_GIL_EXIT(void) {
    if (!g_pikaGIL.mutex.is_first_lock || !g_pikaGIL.mutex.is_init) {
        g_pikaGIL.mutex.bare_lock = 0;
        return 0;
    }
    return pika_thread_recursive_mutex_unlock(&g_pikaGIL);
}

int pika_GIL_deinit(void) {
    pika_platform_memset(&g_pikaGIL, 0, sizeof(g_pikaGIL));
    return 0;
}

int _VM_lock_init(void) {
    if (g_pikaGIL.mutex.is_init) {
        return 0;
    }
    int ret = pika_thread_recursive_mutex_init(&g_pikaGIL);
    if (0 == ret) {
        g_pikaGIL.mutex.is_init = 1;
    }
    return ret;
}

int _VM_is_first_lock(void) {
    return g_pikaGIL.mutex.is_first_lock;
}

int _VMEvent_getVMCnt(void) {
    return g_PikaVMState.vm_cnt;
}

int _VMEvent_getEventPickupCnt(void) {
#if !PIKA_EVENT_ENABLE
    return -1;
#else
    return g_PikaVMState.event_pickup_cnt;
#endif
}

#if PIKA_EVENT_ENABLE
static pika_bool _ecq_isEmpty(volatile PikaEventQueue* cq) {
    return (pika_bool)(cq->head == cq->tail);
}

static pika_bool _ecq_isFull(volatile PikaEventQueue* cq) {
    return (pika_bool)((cq->tail + 1) % PIKA_EVENT_LIST_SIZE == cq->head);
}
#endif

#if PIKA_SETJMP_ENABLE

static pika_bool _jcq_isEmpty(volatile JmpBufCQ* cq) {
    return (pika_bool)(cq->head == cq->tail);
}

static pika_bool _jcq_isFull(volatile JmpBufCQ* cq) {
    return (pika_bool)((cq->tail + 1) % PIKA_JMP_BUF_LIST_SIZE == cq->head);
}

static jmp_buf* _jcq_pop(volatile JmpBufCQ* cq) {
    if (_jcq_isEmpty(cq)) {
        return NULL;
    }
    jmp_buf* buf = cq->buf[cq->head];
    cq->head = (cq->head + 1) % PIKA_JMP_BUF_LIST_SIZE;
    return buf;
}

static jmp_buf* _jcq_check_pop(volatile JmpBufCQ* cq) {
    if (_jcq_isEmpty(cq)) {
        return NULL;
    }
    return cq->buf[cq->head];
}

static PIKA_RES _jcq_push(volatile JmpBufCQ* cq, jmp_buf* pos) {
    if (_jcq_isFull(cq)) {
        return -1;
    }
    cq->buf[cq->tail] = pos;
    cq->tail = (cq->tail + 1) % PIKA_JMP_BUF_LIST_SIZE;
    return PIKA_RES_OK;
}

static PIKA_RES _jcq_remove(volatile JmpBufCQ* cq, jmp_buf* pos) {
    if (_jcq_isEmpty(cq)) {
        return -1;
    }
    for (int i = cq->head; i != cq->tail;
         i = (i + 1) % PIKA_JMP_BUF_LIST_SIZE) {
        if (cq->buf[i] == pos) {
            /* move */
            for (int j = i; j != cq->tail;
                 j = (j + 1) % PIKA_JMP_BUF_LIST_SIZE) {
                cq->buf[j] = cq->buf[(j + 1) % PIKA_JMP_BUF_LIST_SIZE];
            }
            cq->tail = (cq->tail - 1) % PIKA_JMP_BUF_LIST_SIZE;
            return PIKA_RES_OK;
        }
    }
    return -1;
}
#endif

void _VMEvent_deinit(void) {
#if !PIKA_EVENT_ENABLE
    pika_platform_printf("PIKA_EVENT_ENABLE is not enable");
    pika_platform_panic_handle();
#else
    for (int i = 0; i < PIKA_EVENT_LIST_SIZE; i++) {
        if (NULL != g_PikaVMState.cq.res[i]) {
            arg_deinit(g_PikaVMState.cq.res[i]);
            g_PikaVMState.cq.res[i] = NULL;
        }
        if (NULL != g_PikaVMState.cq.data[i].arg) {
            arg_deinit(g_PikaVMState.cq.data[i].arg);
            g_PikaVMState.cq.data[i].arg = NULL;
        }
        if (NULL != g_PikaVMState.sq.res[i]) {
            arg_deinit(g_PikaVMState.sq.res[i]);
            g_PikaVMState.sq.res[i] = NULL;
        }
        g_PikaVMState.cq.id[i] = 0;
        g_PikaVMState.cq.data[i].arg = NULL;
        g_PikaVMState.cq.listener[i] = NULL;
        g_PikaVMState.sq.id[i] = 0;
        g_PikaVMState.sq.data[i].signal = 0;
        g_PikaVMState.sq.listener[i] = NULL;
    }
    g_PikaVMState.cq.head = 0;
    g_PikaVMState.cq.tail = 0;
    g_PikaVMState.sq.head = 0;
    g_PikaVMState.sq.tail = 0;
    if (NULL != g_PikaVMState.event_thread) {
        g_PikaVMState.event_thread_exit = 1;
        pika_platform_thread_destroy(g_PikaVMState.event_thread);
        g_PikaVMState.event_thread = NULL;
        pika_GIL_EXIT();
        while (!g_PikaVMState.event_thread_exit_done) {
            pika_platform_thread_yield();
        }
        g_PikaVMState.event_thread_exit = 0;
        g_PikaVMState.event_thread_exit_done = 0;
        pika_GIL_ENTER();
    }
#endif
}

PIKA_RES __eventListener_pushEvent(PikaEventListener* lisener,
                                   uintptr_t eventId,
                                   Arg* eventData) {
#if !PIKA_EVENT_ENABLE
    pika_platform_printf("PIKA_EVENT_ENABLE is not enable");
    pika_platform_panic_handle();
    return PIKA_RES_ERR_OPERATION_FAILED;
#else
    if (arg_getType(eventData) == ARG_TYPE_OBJECT_NEW) {
        arg_setType(eventData, ARG_TYPE_OBJECT);
    }
    /* push to event_cq_buff */
    if (_ecq_isFull(&g_PikaVMState.cq)) {
        // pika_debug("event_cq_buff is full");
        arg_deinit(eventData);
        return PIKA_RES_ERR_SIGNAL_EVENT_FULL;
    }
    if (g_PikaVMState.cq.res[g_PikaVMState.cq.tail] != NULL) {
        arg_deinit(g_PikaVMState.cq.res[g_PikaVMState.cq.tail]);
        g_PikaVMState.cq.res[g_PikaVMState.cq.tail] = NULL;
    }
    if (g_PikaVMState.cq.data[g_PikaVMState.cq.tail].arg != NULL) {
        arg_deinit(g_PikaVMState.cq.data[g_PikaVMState.cq.tail].arg);
        g_PikaVMState.cq.data[g_PikaVMState.cq.tail].arg = NULL;
    }
    g_PikaVMState.cq.id[g_PikaVMState.cq.tail] = eventId;
    g_PikaVMState.cq.data[g_PikaVMState.cq.tail].arg = eventData;
    g_PikaVMState.cq.listener[g_PikaVMState.cq.tail] = lisener;
    g_PikaVMState.cq.tail = (g_PikaVMState.cq.tail + 1) % PIKA_EVENT_LIST_SIZE;
    return PIKA_RES_OK;
#endif
}

PIKA_RES __eventListener_pushSignal(PikaEventListener* lisener,
                                    uintptr_t eventId,
                                    int signal) {
#if !PIKA_EVENT_ENABLE
    pika_platform_printf("PIKA_EVENT_ENABLE is not enable");
    pika_platform_panic_handle();
    return PIKA_RES_ERR_OPERATION_FAILED;
#else
    /* push to event_cq_buff */
    if (_ecq_isFull(&g_PikaVMState.sq)) {
        // pika_debug("event_cq_buff is full");
        return PIKA_RES_ERR_SIGNAL_EVENT_FULL;
    }
    g_PikaVMState.sq.id[g_PikaVMState.sq.tail] = eventId;
    g_PikaVMState.sq.data[g_PikaVMState.sq.tail].signal = signal;
    g_PikaVMState.sq.listener[g_PikaVMState.sq.tail] = lisener;
    g_PikaVMState.sq.tail = (g_PikaVMState.sq.tail + 1) % PIKA_EVENT_LIST_SIZE;
    if (_VM_is_first_lock()) {
        pika_GIL_ENTER();
        if (g_PikaVMState.sq.res[g_PikaVMState.sq.tail] != NULL) {
            arg_deinit(g_PikaVMState.sq.res[g_PikaVMState.sq.tail]);
            g_PikaVMState.sq.res[g_PikaVMState.sq.tail] = NULL;
        }
        pika_GIL_EXIT();
    }
    return PIKA_RES_OK;
#endif
}

PIKA_RES __eventListener_popEvent(PikaEventListener** lisener_p,
                                  uintptr_t* id,
                                  Arg** data,
                                  int* signal,
                                  int* head) {
#if !PIKA_EVENT_ENABLE
    pika_platform_printf("PIKA_EVENT_ENABLE is not enable");
    pika_platform_panic_handle();
    return PIKA_RES_ERR_OPERATION_FAILED;
#else
    PikaEventQueue* cq = NULL;
    /* pop from event_cq_buff */
    if (!_ecq_isEmpty(&g_PikaVMState.cq)) {
        cq = (PikaEventQueue*)&g_PikaVMState.cq;
    } else if (!_ecq_isEmpty(&g_PikaVMState.sq)) {
        cq = (PikaEventQueue*)&g_PikaVMState.sq;
    }
    if (NULL == cq) {
        return PIKA_RES_ERR_SIGNAL_EVENT_EMPTY;
    }
    *id = cq->id[g_PikaVMState.cq.head];
    if (cq == &g_PikaVMState.cq) {
        *data = cq->data[g_PikaVMState.cq.head].arg;
    } else {
        *signal = cq->data[g_PikaVMState.cq.head].signal;
        *data = NULL;
    }
    *lisener_p = cq->listener[g_PikaVMState.cq.head];
    *head = cq->head;
    cq->head = (cq->head + 1) % PIKA_EVENT_LIST_SIZE;
    return PIKA_RES_OK;
#endif
}

PIKA_RES __eventListener_popSignalEvent(PikaEventListener** lisener_p,
                                        uintptr_t* id,
                                        int* signal,
                                        int* head) {
#if !PIKA_EVENT_ENABLE
    pika_platform_printf("PIKA_EVENT_ENABLE is not enable");
    pika_platform_panic_handle();
    return PIKA_RES_ERR_OPERATION_FAILED;
#else
    /* pop from event_cq_buff */
    if (_ecq_isEmpty(&g_PikaVMState.sq)) {
        return PIKA_RES_ERR_SIGNAL_EVENT_EMPTY;
    }
    *id = g_PikaVMState.sq.id[g_PikaVMState.sq.head];
    *signal = g_PikaVMState.sq.data[g_PikaVMState.sq.head].signal;
    *lisener_p = g_PikaVMState.sq.listener[g_PikaVMState.sq.head];
    *head = g_PikaVMState.sq.head;
    g_PikaVMState.sq.head = (g_PikaVMState.sq.head + 1) % PIKA_EVENT_LIST_SIZE;
    return PIKA_RES_OK;
#endif
}

void __VMEvent_pickupEvent(char* info) {
#if !PIKA_EVENT_ENABLE
    pika_platform_printf("PIKA_EVENT_ENABLE is not enable\r\n");
    pika_platform_panic_handle();
#else
    int evt_pickup_cnt = _VMEvent_getEventPickupCnt();
    if (evt_pickup_cnt >= PIKA_EVENT_PICKUP_MAX) {
        return;
    }
    PikaObj* event_listener;
    uintptr_t event_id;
    Arg* event_data;
    int signal;
    int head;
    if (PIKA_RES_OK == __eventListener_popEvent(&event_listener, &event_id,
                                                &event_data, &signal, &head)) {
        g_PikaVMState.event_pickup_cnt++;
        pika_debug("pickup_info: %s", info);
        pika_debug("pickup_cnt: %d", g_PikaVMState.event_pickup_cnt);
        Arg* res = NULL;
        if (NULL != event_data) {
            res =
                __eventListener_runEvent(event_listener, event_id, event_data);
        } else {
            event_data = arg_newInt(signal);
            res =
                __eventListener_runEvent(event_listener, event_id, event_data);
            arg_deinit(event_data);
            event_data = NULL;
        }
        // only on muti thread, keep the res
        if (_VM_is_first_lock()) {
            if (NULL == event_data) {
                // from signal
                g_PikaVMState.sq.res[head] = res;
            } else {
                // from event
                g_PikaVMState.cq.res[head] = res;
            }
        } else {
            if (NULL != res) {
                arg_deinit(res);
            }
        }
        g_PikaVMState.event_pickup_cnt--;
    }
#endif
}

VM_SIGNAL_CTRL VMSignal_getCtrl(void) {
    return g_PikaVMState.signal_ctrl;
}

void pika_vm_exit(void) {
    g_PikaVMState.signal_ctrl = VM_SIGNAL_CTRL_EXIT;
}

void pika_vm_exit_await(void) {
    pika_vm_exit();
    while (1) {
        pika_platform_thread_yield();
        if (_VMEvent_getVMCnt() == 0) {
            return;
        }
    }
}

void pika_vmSignal_setCtrlClear(void) {
    g_PikaVMState.signal_ctrl = VM_SIGNAL_CTRL_NONE;
}

/* head declare start */
static uint32_t PikaVMFrame_getInputArgNum(PikaVMFrame* vm);
static VMParameters* __pikaVM_runByteCodeFrameWithState(
    PikaObj* self,
    VMParameters* locals,
    VMParameters* globals,
    ByteCodeFrame* bytecode_frame,
    uint16_t pc,
    PikaVMThread* vm_thread,
    pika_bool in_repl);

static VMParameters* _pikaVM_runByteCodeFrameWithState(
    PikaObj* self,
    VMParameters* locals,
    VMParameters* globals,
    ByteCodeFrame* bytecode_frame,
    uint16_t pc,
    PikaVMThread* vm_thread);

/* head declare end */

static void PikaVMFrame_setErrorCode(PikaVMFrame* vm, int8_t error_code) {
    vm->error.code = error_code;
}

void _do_vsysOut(char* fmt, va_list args);
void PikaVMFrame_setSysOut(PikaVMFrame* vm, char* fmt, ...) {
    pika_assert(NULL != vm);
    if (vm->error.code == PIKA_RES_OK) {
        vm->error.code = PIKA_RES_ERR_RUNTIME_ERROR;
    }
    if (vm->vm_thread->try_state == TRY_STATE_INNER) {
        return;
    }
    va_list args;
    va_start(args, fmt);
    _do_vsysOut(fmt, args);
    va_end(args);
}

static enum InstructIndex PikaVMFrame_getInstructWithOffset(PikaVMFrame* vm,
                                                            int32_t offset) {
    return instructUnit_getInstructIndex(
        PikaVMFrame_getInstructUnitWithOffset(vm, offset));
}

static int PikaVMFrame_getBlockDeepthNow(PikaVMFrame* vm) {
    /* support run byteCode */
    InstructUnit* ins_unit = PikaVMFrame_getInstructNow(vm);
    return instructUnit_getBlockDeepth(ins_unit);
}

#if !PIKA_NANO_ENABLE
static char* PikaVMFrame_getConstWithOffset(PikaVMFrame* vm, int32_t offset) {
    return PikaVMFrame_getConstWithInstructUnit(
        vm, PikaVMFrame_getInstructUnitWithOffset(vm, offset));
}
#endif

static int PikaVMFrame_getInvokeDeepthNow(PikaVMFrame* vm) {
    /* support run byteCode */
    InstructUnit* ins_unit = PikaVMFrame_getInstructNow(vm);
    return instructUnit_getInvokeDeepth(ins_unit);
}

pika_bool PikaVMFrame_checkBreakPoint(PikaVMFrame* vm) {
#if !PIKA_DEBUG_BREAK_POINT_MAX
    return pika_false;
#else
    if (g_PikaVMState.break_point_cnt == 0) {
        return pika_false;
    }
    if (NULL == vm->bytecode_frame->name) {
        return pika_false;
    }
    Hash module_hash = byteCodeFrame_getNameHash(vm->bytecode_frame);
    return pika_debug_check_break_hash(module_hash, vm->pc);
#endif
}

static int32_t PikaVMFrame_getAddrOffsetOfJmpBack(PikaVMFrame* vm) {
    int offset = 0;
    int blockDeepthGot = -1;
    int blockDeepthNow = PikaVMFrame_getBlockDeepthNow(vm);

    /* find loop deepth */
    while (1) {
        offset -= instructUnit_getSize();
        InstructUnit* insUnitThis =
            PikaVMFrame_getInstructUnitWithOffset(vm, offset);
        uint16_t invokeDeepth = instructUnit_getInvokeDeepth(insUnitThis);
        enum InstructIndex ins = instructUnit_getInstructIndex(insUnitThis);
        char* data = PikaVMFrame_getConstWithInstructUnit(vm, insUnitThis);
        if ((0 == invokeDeepth) && (PIKA_INS(JEZ) == ins) && data[0] == '2') {
            InstructUnit* insUnitLast = PikaVMFrame_getInstructUnitWithOffset(
                vm, offset - instructUnit_getSize());
            enum InstructIndex insLast =
                instructUnit_getInstructIndex(insUnitLast);
            /* skip try stmt */
            if (PIKA_INS(GER) == insLast) {
                continue;
            }
            /* skip inner break */
            int blockDeepthThis = instructUnit_getBlockDeepth(insUnitThis);
            if (blockDeepthThis >= blockDeepthNow) {
                continue;
            }
            blockDeepthGot = instructUnit_getBlockDeepth(insUnitThis);
            break;
        }
    }

    offset = 0;
    while (1) {
        offset += instructUnit_getSize();
        InstructUnit* insUnitThis =
            PikaVMFrame_getInstructUnitWithOffset(vm, offset);
        enum InstructIndex ins = instructUnit_getInstructIndex(insUnitThis);
        char* data = PikaVMFrame_getConstWithInstructUnit(vm, insUnitThis);
        int blockDeepthThis = instructUnit_getBlockDeepth(insUnitThis);
        if ((blockDeepthThis == blockDeepthGot) && (PIKA_INS(JMP) == ins) &&
            data[0] == '-' && data[1] == '1') {
            return offset;
        }
    }
}

static int32_t PikaVMFrame_getAddrOffsetFromJmp(PikaVMFrame* vm) {
    int offset = 0;
    /* run byte Code */
    InstructUnit* this_ins_unit = PikaVMFrame_getInstructNow(vm);
    int thisBlockDeepth = instructUnit_getBlockDeepth(this_ins_unit);
    int8_t blockNum = 0;
    int pc_max = (int)PikaVMFrame_getInstructArraySize(vm);
    if (vm->jmp > 0) {
        offset = 0;
        while (1) {
            offset += instructUnit_getSize();
            /* reach the end */
            if (vm->pc + offset >= pc_max) {
                break;
            }
            this_ins_unit = PikaVMFrame_getInstructUnitWithOffset(vm, offset);
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
            this_ins_unit = PikaVMFrame_getInstructUnitWithOffset(vm, offset);
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

static int32_t PikaVMFrame_getAddrOffsetOfBreak(PikaVMFrame* vm) {
    int32_t offset = PikaVMFrame_getAddrOffsetOfJmpBack(vm);
    /* byteCode */
    offset += instructUnit_getSize();
    return offset;
}

#if !PIKA_NANO_ENABLE
static int32_t PikaVMFrame_getAddrOffsetOfRaise(PikaVMFrame* vm) {
    int offset = 0;
    InstructUnit* ins_unit_now = PikaVMFrame_getInstructNow(vm);
    while (1) {
        offset += instructUnit_getSize();
        if (vm->pc + offset >= (int)PikaVMFrame_getInstructArraySize(vm)) {
            return 0;
        }
        ins_unit_now = PikaVMFrame_getInstructUnitWithOffset(vm, offset);
        enum InstructIndex ins = instructUnit_getInstructIndex(ins_unit_now);
        if (PIKA_INS(NTR) == ins) {
            return offset;
        }
        /* if not found except, return */
        if (PIKA_INS(RET) == ins) {
            return 0;
        }
    }
}
#endif

static int32_t PikaVMFrame_getAddrOffsetOfContinue(PikaVMFrame* vm) {
    int32_t offset = PikaVMFrame_getAddrOffsetOfJmpBack(vm);
    /* byteCode */
    return offset;
}

static void VMLocals_delLReg(VMLocals* self, uint8_t index) {
    PikaObj* obj = self->lreg[index];
    if (NULL != obj) {
        obj_enableGC(obj);
        self->lreg[index] = NULL;
        obj_GC(obj);
    }
}

static void Locals_delLReg(PikaObj* self, char* name) {
    if (!_checkLReg(name)) {
        return;
    }
    uint8_t reg_index = _getLRegIndex(name);
    VMLocals* locals = obj_getStruct(self, "@l");
    VMLocals_delLReg(locals, reg_index);
}

static void VMLocals_clearReg(VMLocals* self) {
    for (int i = 0; i < PIKA_REGIST_SIZE; i++) {
        VMLocals_delLReg(self, i);
    }
}

static PikaObj* Locals_getLReg(PikaObj* self, char* name) {
    /* get method host obj from reg */
    if (!_checkLReg(name)) {
        return NULL;
    }
    uint8_t reg_index = _getLRegIndex(name);
    VMLocals* locals = obj_getStruct(self, "@l");
    return locals->lreg[reg_index];
}

static PikaObj* New_Locals(Args* args) {
    PikaObj* self = New_PikaObj(NULL);
    self->constructor = New_Locals;
#if PIKA_KERNAL_DEBUG_ENABLE
    self->name = "Locals";
#endif
    return self;
}

void Locals_deinit(PikaObj* self) {
    VMLocals* tLocals = obj_getStruct(self, "@l");
    if (NULL == tLocals) {
        return;
    }
    VMLocals_clearReg(tLocals);
}

static int arg_getLen(Arg* self) {
    if (arg_isObject(self)) {
        return obj_getSize(arg_getPtr(self));
    }
    if (arg_getType(self) == ARG_TYPE_STRING) {
        int strGetSizeUtf8(char* str);
        return strGetSizeUtf8(arg_getStr(self));
    }
    if (arg_getType(self) == ARG_TYPE_BYTES) {
        return arg_getBytesSize(self);
    }
    return -1;
}

static void PikaVMFrame_initReg(PikaVMFrame* self) {
    for (uint8_t i = 0; i < PIKA_REGIST_SIZE; i++) {
        self->ireg[i] = pika_false;
    }
    pika_platform_memset(self->oreg, 0, sizeof(self->oreg));
}

static pika_bool _checkLReg(char* data) {
    if ((data[0] == '$') && (data[1] == 'l') && (data[2] >= '0') &&
        (data[2] <= '9')) {
        return pika_true;
    }
    return pika_false;
}

static uint8_t _getLRegIndex(char* data) {
    return data[2] - '0';
}

static void VMLocals_setLReg(VMLocals* self, uint8_t index, PikaObj* obj) {
    obj_refcntInc(obj);
    self->lreg[index] = obj;
}

static void Locals_setLReg(PikaObj* self, char* name, PikaObj* obj) {
    if (!_checkLReg(name)) {
        return;
    }
    uint8_t reg_index = _getLRegIndex(name);
    VMLocals* tlocals = obj_getStruct(self, "@l");
    if (NULL == tlocals) {
        /* init locals */
        VMLocals locals = {0};
        obj_setStruct(self, "@l", locals);
        tlocals = obj_getStruct(self, "@l");
    }
    pika_assert(tlocals != NULL);
    obj_setName(obj, name);
    VMLocals_setLReg(tlocals, reg_index, obj);
}

static Arg* VM_instruction_handler_NON(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    return NULL;
}

Arg* _vm_get(PikaVMFrame* vm, PikaObj* self, Arg* aKey, Arg* aObj) {
    ArgType eType = arg_getType(aObj);
    Arg* aObjNew = NULL;
    int iIndex = 0;
    int iLen = arg_getLen(aObj);
    if (ARG_TYPE_INT == arg_getType(aKey)) {
        iIndex = arg_getInt(aKey);
    }

    if (iLen != -1) {
        if (iIndex < 0) {
            iIndex += iLen;
            arg_setInt(aKey, "", iIndex);
        }

        if (iIndex >= iLen) {
            PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OUT_OF_RANGE);
            PikaVMFrame_setSysOut(vm, "IndexError: index out of range");
            return arg_newNone();
        }
    }

    if (ARG_TYPE_STRING == eType) {
#if PIKA_STRING_UTF8_ENABLE
        pika_bool bIsTemp = pika_false;
        aObjNew = arg_newObj(_arg_to_obj(aObj, &bIsTemp));
        eType = arg_getType(aObjNew);
#else
        char* sPyload = arg_getStr(aObj);
        char sCharBuff[] = " ";
        if (iIndex < 0) {
            iIndex = strGetSize(sPyload) + iIndex;
        }
        sCharBuff[0] = sPyload[iIndex];
        return arg_newStr(sCharBuff);
#endif
    }
    if (ARG_TYPE_BYTES == eType) {
        uint8_t* sBytesPyload = arg_getBytes(aObj);
        uint8_t sByteBuff[] = " ";
        sByteBuff[0] = sBytesPyload[iIndex];
        return arg_newInt(sByteBuff[0]);
    }
    if (argType_isObject(eType)) {
        PikaObj* oArg = NULL;
        Arg* aRes = NULL;
        if (aObjNew != NULL) {
            oArg = arg_getPtr(aObjNew);
        } else {
            oArg = arg_getPtr(aObj);
        }
        obj_setArg(oArg, "__key", aKey);
        /* clang-format off */
        PIKA_PYTHON(
        @res_item = __getitem__(__key)
        )
        /* clang-format on */
        const uint8_t bytes[] = {
            0x0c, 0x00, 0x00, 0x00, /* instruct array size */
            0x10, 0x81, 0x01, 0x00, 0x00, 0x02, 0x07, 0x00, 0x00, 0x04, 0x13,
            0x00,
            /* instruct array */
            0x1d, 0x00, 0x00, 0x00, /* const pool size */
            0x00, 0x5f, 0x5f, 0x6b, 0x65, 0x79, 0x00, 0x5f, 0x5f, 0x67, 0x65,
            0x74, 0x69, 0x74, 0x65, 0x6d, 0x5f, 0x5f, 0x00, 0x40, 0x72, 0x65,
            0x73, 0x5f, 0x69, 0x74, 0x65, 0x6d, 0x00, /* const pool */
        };
        if (NULL != vm) {
            aRes = pikaVM_runByteCode_exReturn(oArg, oArg, oArg,
                                               (uint8_t*)bytes, vm->vm_thread,
                                               pika_true, "@res_item");
        } else {
            aRes = pikaVM_runByteCodeReturn(oArg, (uint8_t*)bytes, "@res_item");
        }
        if (NULL != aObjNew) {
            arg_deinit(aObjNew);
        }
        if (NULL == aRes) {
            if (NULL != vm) {
                PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_ARG_NO_FOUND);
            }
            return arg_newNone();
        }
        return aRes;
    }
    return arg_newNone();
}

Arg* _vm_slice(PikaVMFrame* vm,
               PikaObj* self,
               Arg* aEnd,
               Arg* aObj,
               Arg* aStart,
               int step) {
#if PIKA_SYNTAX_SLICE_ENABLE
    /* No interger index only support __getitem__ */
    if (!(arg_getType(aStart) == ARG_TYPE_INT &&
          arg_getType(aEnd) == ARG_TYPE_INT)) {
        return _vm_get(vm, self, aStart, aObj);
    }
    int iLen = arg_getLen(aObj);
    int iStart = arg_getInt(aStart);
    int iEnd = arg_getInt(aEnd);

    if (iStart < 0) {
        iStart += iLen;
    }
    /* magic code, to the end */
    if (iEnd == VM_PC_EXIT) {
        iEnd = iLen;
    }
    if (iEnd < 0) {
        iEnd += iLen;
    }

    if (iStart > iLen) {
        iStart = iLen;
    }

    if (iEnd > iLen) {
        iEnd = iLen;
    }

    if (iStart > iEnd) {
        return arg_newStr("");
    }

    /* __slice__ is equal to __getitem__ */
    if (iEnd - iStart == 1 && arg_getType(aObj) != ARG_TYPE_BYTES) {
        return _vm_get(vm, self, aStart, aObj);
    }

    if (ARG_TYPE_STRING == arg_getType(aObj)) {
        Args buffs = {0};
        Arg* aSliced = NULL;
        char* sSliced = string_slice(&buffs, arg_getStr(aObj), iStart, iEnd);
        if (NULL != sSliced) {
            aSliced = arg_newStr(sSliced);
        } else {
            aSliced = arg_newNone();
        }
        strsDeinit(&buffs);
        return aSliced;
    }

    if (ARG_TYPE_BYTES == arg_getType(aObj)) {
        uint8_t* sBytesOrigin = arg_getBytes(aObj);
        Arg* aSliced = arg_newBytes(sBytesOrigin + iStart, iEnd - iStart);
        return aSliced;
    }

    if (arg_isObject(aObj)) {
        PikaObj* oArg = arg_getPtr(aObj);
        PikaObj* New_PikaStdData_List(Args * args);
        PikaObj* New_PikaStdData_Tuple(Args * args);
        if (oArg->constructor == New_PikaStdData_List ||
            oArg->constructor == New_PikaStdData_Tuple) {
            PikaObj* oSliced = newNormalObj((NewFun)oArg->constructor);
            pikaList_init(oSliced);
            for (int i = iStart; i < iEnd; i++) {
                Arg* aIndex = arg_newInt(i);
                Arg* aItem = _vm_get(vm, self, aIndex, aObj);
                pikaList_append(oSliced, aItem);
                arg_deinit(aIndex);
            }
            return arg_newObj(oSliced);
        }
    }
    return arg_newNone();
#else
    return _vm_get(vm, self, aStart, aObj);
#endif
}

static Arg* VM_instruction_handler_SLC(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
#if PIKA_SYNTAX_SLICE_ENABLE
    uint32_t n_input = PikaVMFrame_getInputArgNum(vm);
    if (n_input < 2) {
        return arg_newNone();
    }
    if (n_input == 2) {
        Arg* key = stack_popArg_alloc(&vm->stack);
        Arg* obj = stack_popArg_alloc(&vm->stack);
        Arg* res = _vm_get(vm, self, key, obj);
        arg_deinit(key);
        arg_deinit(obj);
        return res;
    }
    if (n_input == 3) {
        Arg* end = stack_popArg_alloc(&vm->stack);
        Arg* start = stack_popArg_alloc(&vm->stack);
        if (arg_getType(start) != ARG_TYPE_INT ||
            arg_getType(end) != ARG_TYPE_INT) {
            PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_INVALID_PARAM);
            PikaVMFrame_setSysOut(vm,
                                  "TypeError: slice indices must be integers");
            arg_deinit(end);
            arg_deinit(start);
            return arg_newNone();
        }
        Arg* obj = stack_popArg_alloc(&vm->stack);
        Arg* res = _vm_slice(vm, self, end, obj, start, 1);
        arg_deinit(end);
        arg_deinit(obj);
        arg_deinit(start);
        return res;
    }
    return arg_newNone();
#else
    Arg* key = stack_popArg_alloc(&vm->stack);
    Arg* obj = stack_popArg_alloc(&vm->stack);
    Arg* res = _vm_get(vm, self, key, obj);
    arg_deinit(key);
    arg_deinit(obj);
    return res;
#endif
}

static Arg* VM_instruction_handler_TRY(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    pika_assert(NULL != vm->vm_thread);
    vm->vm_thread->try_state = TRY_STATE_INNER;
    return NULL;
}

static Arg* VM_instruction_handler_EXP(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    pikaVMThread_clearExceptionStack(vm->vm_thread);
    return NULL;
}

static Arg* VM_instruction_handler_NTR(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    vm->vm_thread->try_state = TRY_STATE_NONE;
    pikaVMThread_convertExceptionStack(vm->vm_thread);
    return NULL;
}

static Arg* VM_instruction_handler_NEW(PikaObj* self,
                                       PikaVMFrame* vm,
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
        Arg* aRes = obj_runMethod1(host, "__getattribute__", arg_newStr(name));
        return aRes;
    }
    return NULL;
}

static Arg* _proxy_getattr(PikaObj* host, char* name) {
#if PIKA_NANO_ENABLE
    return NULL;
#endif
    if ('@' != name[0] && obj_getFlag(host, OBJ_FLAG_PROXY_GETATTR)) {
        Arg* aRes = obj_runMethod1(host, "__getattr__", arg_newStr(name));
        return aRes;
    }
    return NULL;
}

static Arg* VM_instruction_handler_REF(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* aRetReg) {
    PikaObj* oHost = NULL;
    char* arg_path = data;
    char* arg_name = strPointToLastToken(arg_path, '.');
    pika_bool is_temp = pika_false;
    pika_bool is_alloc = pika_false;
    PikaObj* oBuiltins = NULL;

    switch (data[0]) {
        case 'T':
            if (strEqu(arg_path, (char*)"True")) {
                return arg_setBool(aRetReg, "", pika_true);
            }
            break;
        case 'F':
            if (strEqu(arg_path, (char*)"False")) {
                return arg_setBool(aRetReg, "", pika_false);
            }
            break;
        case 'N':
            if (strEqu(arg_path, (char*)"None")) {
                return arg_setNone(aRetReg);
            }
            break;
        case 'R':
            if (strEqu(arg_path, (char*)"RuntimeError")) {
                return arg_setInt(aRetReg, "", PIKA_RES_ERR_RUNTIME_ERROR);
            }
            break;
    }

    Arg* aRes = NULL;
    if (arg_path[0] == '.') {
        /* find host from stack */
        Arg* host_arg = stack_popArg_alloc(&(vm->stack));
        if (NULL == host_arg) {
            goto __exit;
        }
        if (arg_isObject(host_arg)) {
            oHost = arg_getPtr(host_arg);
            aRes = arg_copy_noalloc(obj_getArg(oHost, arg_path + 1), aRetReg);
        }
        arg_deinit(host_arg);
        goto __exit;
    }

    /* find in local list first */
    if (NULL == oHost) {
        oHost = obj_getHostObjWithIsTemp(vm->locals, arg_path, &is_temp);
    }

    /* find in global list */
    if (NULL == oHost) {
        oHost = obj_getHostObjWithIsTemp(vm->globals, arg_path, &is_temp);
    }

    /* error cannot found host_object */
    if (NULL == oHost) {
        goto __exit;
    }

    /* proxy */
    if (NULL == aRes) {
        aRes = _proxy_getattribute(oHost, arg_name);
        if (NULL != aRes) {
            is_alloc = pika_true;
        }
    }

    /* find res in host */
    if (NULL == aRes) {
        aRes = args_getArg(oHost->list, arg_name);
    }

    /* find res in host prop */
    if (NULL == aRes) {
        aRes = _obj_getPropArg(oHost, arg_name);
    }

    /* find res in globals */
    if (arg_path == arg_name) {
        if (NULL == aRes) {
            aRes = args_getArg(vm->globals->list, arg_name);
        }

        /* find res in globals prop */
        if (NULL == aRes) {
            aRes = _obj_getPropArg(vm->globals, arg_name);
        }

        /* find res in builtins */
        if (NULL == aRes) {
            oBuiltins = obj_getBuiltins();
            aRes = args_getArg(oBuiltins->list, arg_name);
        }

        if (NULL == aRes) {
            aRes = _obj_getPropArg(oBuiltins, arg_name);
        }
    }

    /* proxy */
    if (NULL == aRes) {
        aRes = _proxy_getattr(oHost, arg_name);
        if (NULL != aRes) {
            is_alloc = pika_true;
        }
    }
__exit:
    if (NULL != oBuiltins) {
        obj_deinit(oBuiltins);
    }
    if (NULL == aRes) {
        PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_ARG_NO_FOUND);
        PikaVMFrame_setSysOut(vm, "NameError: name '%s' is not defined",
                              arg_path);
    } else {
        aRes = methodArg_setHostObj(aRes, oHost);
        if ((arg_getType(aRes) != ARG_TYPE_METHOD_NATIVE_ACTIVE) && !is_alloc) {
            aRes = arg_copy_noalloc(aRes, aRetReg);
        }
        pika_assert_arg_alive(aRes);
    }
    if (is_temp) {
        obj_GC(oHost);
    }
    return aRes;
}

static Arg* VM_instruction_handler_GER(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    PIKA_RES err = pikaVMFrame_checkExceptionStack(vm);
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
                                Arg* aMethod,
                                PikaVMThread* vm_thread,
                                Arg* aReturnCache) {
    pika_assert(NULL != vm_thread);
    Arg* aReturn = NULL;
    /* get method Ptr */
    Method fMethod = methodArg_getPtr(aMethod);
    /* get method type list */
    ArgType methodType = arg_getType(aMethod);
    /* error */
    if (ARG_TYPE_NONE == methodType) {
        return NULL;
    }

    /* redirect to def context */
    if (!argType_isNative(methodType)) {
        self = methodArg_getDefContext(aMethod);
    }

    /* run method */
    if (argType_isNative(methodType)) {
        /* native method */
        PikaObj* oHost = self;
        if (methodType == ARG_TYPE_METHOD_NATIVE_ACTIVE) {
            oHost = methodArg_getHostObj(aMethod);
        }
        fMethod(oHost, locals->list);
        /* get method return */
        aReturn = _get_return_arg(locals);
    } else {
        /* static method and object method */
        /* byteCode */
        ByteCodeFrame* method_bytecodeFrame =
            methodArg_getBytecodeFrame(aMethod);
        uintptr_t insturctArray_start = (uintptr_t)instructArray_getByOffset(
            &(method_bytecodeFrame->instruct_array), 0);
        uint16_t pc = (uintptr_t)fMethod - insturctArray_start;
        locals = _pikaVM_runByteCodeFrameWithState(
            self, locals, self, method_bytecodeFrame, pc, vm_thread);

        /* get method return */
        aReturn = _get_return_arg(locals);
    }
#if PIKA_TYPE_FULL_FEATURE_ENABLE
    PikaObj* oReturn = NULL;
    if (arg_isConstructor(aMethod)) {
        if (arg_isObject(aReturn)) {
            oReturn = arg_getPtr(aReturn);
            obj_setArg(oReturn, "__class__", aMethod);
        }
    }
#endif
    pika_assert_arg_alive(aReturn);
    return aReturn;
}

Arg* obj_runMethodArgWithState(PikaObj* self,
                               PikaObj* method_args_obj,
                               Arg* method_arg,
                               PikaVMThread* vm_thread) {
    return _obj_runMethodArgWithState(self, method_args_obj, method_arg,
                                      vm_thread, NULL);
}

Arg* obj_runMethodArgWithState_noalloc(PikaObj* self,
                                       PikaObj* locals,
                                       Arg* method_arg,
                                       PikaVMThread* vm_thread,
                                       Arg* ret_arg_reg) {
    return _obj_runMethodArgWithState(self, locals, method_arg, vm_thread,
                                      ret_arg_reg);
}

Arg* obj_runMethodArg(PikaObj* self,
                      PikaObj* method_args_obj,
                      Arg* method_arg) {
    PikaVMThread* vm_thread = pikaVMThread_require();

    return obj_runMethodArgWithState(self, method_args_obj, method_arg,
                                     vm_thread);
}

static char* _kw_pos_to_default_all(FunctionArgsInfo* f,
                                    char* sArgName,
                                    int* argc,
                                    Arg* argv[],
                                    Arg* aCall) {
#if PIKA_NANO_ENABLE
    return sArgName;
#endif
    int iDefaultSkip = 0;
    int iDefaultSkiped = 0;
    if (f->i_arg == f->n_arg) {
        iDefaultSkip = f->n_default - f->n_arg + f->n_positional;
    }
    while (strIsContain(sArgName, '=')) {
        strPopLastToken(sArgName, '=');
        Arg* aKeyword = NULL;
        /* load default arg from kws */
        if (f->kw != NULL) {
            aKeyword = pikaDict_get(f->kw, sArgName);
            if (aKeyword != NULL) {
                argv[(*argc)++] = arg_copy(aKeyword);
                pikaDict_removeArg(f->kw, aKeyword);
                goto __next;
            }
        }

        /* load default arg from pos */
        if (NULL != aCall && f->is_default) {
            /* load pos to default with right order */
            if (iDefaultSkiped < iDefaultSkip) {
                iDefaultSkiped++;
                sArgName = strPopLastToken(f->type_list, ',');
                continue;
            }
            /* load default from pos */
            if (f->i_arg > f->n_positional) {
                arg_setNameHash(aCall, hash_time33EndWith(sArgName, ':'));
                argv[(*argc)++] = aCall;
                return (char*)1;
            }
        }

    __next:
        sArgName = strPopLastToken(f->type_list, ',');
    }
    return sArgName;
}

static int _kw_to_pos_one(FunctionArgsInfo* f,
                          char* arg_name,
                          int* argc,
                          Arg* argv[]) {
    if (f->kw == NULL) {
        return 0;
    }
    Arg* pos_arg = pikaDict_get(f->kw, arg_name);
    if (pos_arg == NULL) {
        return 0;
    }
    argv[(*argc)++] = arg_copy(pos_arg);
    pikaDict_removeArg(f->kw, pos_arg);
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
        Arg* pos_arg = pikaDict_get(f->kw, arg_name);
        pika_assert(pos_arg != NULL);
        argv[(*argc)++] = arg_copy(pos_arg);
        pikaDict_removeArg(f->kw, pos_arg);
    }
}

static void _loadLocalsFromArgv(Args* locals, int argc, Arg* argv[]) {
    for (int i = 0; i < argc; i++) {
        Arg* arg = argv[i];
        pika_assert(arg != NULL);
        args_setArg(locals, arg);
    }
}

static void _type_list_parse(FunctionArgsInfo* f) {
    if (f->type_list[0] == 0) {
        f->n_positional = 0;
        return;
    }
    int8_t iArgc = strCountSign(f->type_list, ',') + 1;
    int8_t iStar = strCountSign(f->type_list, '*');
    int8_t iAssign = strCountSign(f->type_list, '=');
    /* default */
    if (iAssign > 0) {
        iArgc -= iAssign;
        f->is_default = pika_true;
        f->n_default = iAssign;
    }
    /* vars */
    if (iStar == 1) {
        f->is_vars = pika_true;
        f->n_positional = iArgc - 1;
        return;
    }
    /* kw */
    if (iStar == 2) {
        f->is_keys = pika_true;
        f->n_positional = iArgc - 1;
        return;
    }
    /* vars and kw */
    if (iStar == 3) {
        f->is_vars = pika_true;
        f->is_keys = pika_true;
        f->n_positional = iArgc - 2;
        return;
    }
    f->n_positional = iArgc;
    return;
}

static void _kw_push(FunctionArgsInfo* f, Arg* call_arg, int i) {
    if (NULL == f->kw) {
        f->kw = New_PikaDict();
    }
    arg_setIsKeyword(call_arg, pika_false);
    Hash kw_hash = call_arg->name_hash;
    char buff[32] = {0};
    _pikaDict_setVal(f->kw, call_arg);
    char* sHash = fast_itoa(buff, kw_hash);
    args_setStr(_OBJ2KEYS(f->kw), sHash, sHash);
    pikaDict_reverse(f->kw);
}

static void _load_call_arg(PikaVMFrame* vm,
                           Arg* call_arg,
                           FunctionArgsInfo* f,
                           int* i,
                           int* argc,
                           Arg* argv[]) {
    /* load the kw arg */
    pika_assert(NULL != call_arg);
    if (arg_getIsKeyword(call_arg)) {
        _kw_push(f, call_arg, *i);
        return;
    }
    /* load variable arg */
    if (f->i_arg > f->n_positional) {
        if (f->is_vars) {
            pikaList_append(f->tuple, call_arg);
            return;
        }
    }
    char* arg_name = strPopLastToken(f->type_list, ',');
    /* load default from kw */
    arg_name = _kw_pos_to_default_all(f, arg_name, argc, argv, call_arg);
    if (((char*)1) == arg_name) {
        /* load default from pos */
        return;
    }
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
    pika_assert(call_arg != NULL);
    argv[(*argc)++] = call_arg;
    (f->n_positional_got)++;
}

static uint32_t _get_n_input_with_unpack(PikaVMFrame* vm, int n_used) {
#if PIKA_NANO_ENABLE
    return PikaVMFrame_getInputArgNum(vm);
#else
    uint32_t n_input = PikaVMFrame_getInputArgNum(vm) - n_used;
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
            if (!arg_isObject(call_arg)) {
                stack_pushArg(&stack_tmp, arg_copy(call_arg));
                goto __continue;
            }
            PikaObj* obj = arg_getPtr(call_arg);
            int len = obj_getSize(obj);
            for (int i_star_arg = len - 1; i_star_arg >= 0; i_star_arg--) {
                Arg* arg_a =
                    obj_runMethod1(obj, "__getitem__", arg_newInt(i_star_arg));
                stack_pushArg(&stack_tmp, arg_a);
                unpack_num++;
            }
            goto __continue;
        }
        if (arg_getIsDoubleStarred(call_arg)) {
            pika_assert(arg_isObject(call_arg));
            PikaObj* New_PikaStdData_Dict(Args * args);
            PikaObj* obj = arg_getPtr(call_arg);
            pika_assert(obj->constructor == New_PikaStdData_Dict);
            Args* dict = _OBJ2DICT(obj);
            int i_item = args_getSize(dict);
            while (pika_true) {
                i_item--;
                if (i_item < 0) {
                    break;
                }
                Arg* item_val = args_getArgByIndex(dict, i_item);
                /* unpack as keyword arg */
                arg_setIsKeyword(item_val, pika_true);
                stack_pushArg(&stack_tmp, arg_copy(item_val));
            }
            goto __continue;
        }
        stack_pushArg(&stack_tmp, arg_copy(call_arg));
    __continue:
        if (NULL != call_arg) {
            arg_deinit(call_arg);
        }
    }
    uint32_t n_input_new = stack_getTop(&stack_tmp);
    for (int i = 0; i < n_input_new; i++) {
        Arg* arg = stack_popArg_alloc(&stack_tmp);
        stack_pushArg(&(vm->stack), arg);
    }
    stack_deinit(&stack_tmp);
    return n_input_new;
#endif
}

#define vars_or_keys_or_default (f.is_vars || f.is_keys || f.is_default)
#define METHOD_TYPE_LIST_MAX_LEN PIKA_LINE_BUFF_SIZE * 2
static int PikaVMFrame_loadArgsFromMethodArg(PikaVMFrame* vm,
                                             PikaObj* oMethodHost,
                                             Args* aLoclas,
                                             Arg* aMethod,
                                             char* sMethodName,
                                             char* sProxyName,
                                             int iNumUsed) {
    int argc = 0;
    Arg** argv = (Arg**)pikaMalloc(sizeof(Arg*) * PIKA_ARG_NUM_MAX);
    char* buffs1 = (char*)pikaMalloc(METHOD_TYPE_LIST_MAX_LEN);
    char* buffs2 = (char*)pikaMalloc(METHOD_TYPE_LIST_MAX_LEN);
    FunctionArgsInfo f = {0};
    char* type_list_buff = NULL;
    /* get method type list */
    f.type_list =
        methodArg_getTypeList(aMethod, buffs1, METHOD_TYPE_LIST_MAX_LEN);
    pika_assert_msg(NULL != f.type_list, "method: %s", sMethodName);
    if (NULL == f.type_list) {
        pika_platform_printf(
            "OverflowError: type list is too long, please use bigger "
            "PIKA_LINE_BUFF_SIZE\r\n");
        pika_platform_panic_handle();
    }
    f.method_type = arg_getType(aMethod);

    /* get arg_num_pos */
    _type_list_parse(&f);
    if (f.method_type == ARG_TYPE_METHOD_OBJECT) {
        /* delete the 'self' */
        f.n_positional--;
    }

    f.n_input = _get_n_input_with_unpack(vm, iNumUsed);

    if (NULL != sProxyName) {
        /* method proxy takes the first arg as method name */
        f.n_input++;
    }

    do {
        /* check arg num */
        if (f.method_type == ARG_TYPE_METHOD_NATIVE_CONSTRUCTOR ||
            f.method_type == ARG_TYPE_METHOD_CONSTRUCTOR || iNumUsed != 0) {
            /* skip for constrctor */
            /* skip for variable args */
            /* n_used != 0 means it is a factory method */
            break;
        }
        /* check position arg num */
        if (!vars_or_keys_or_default) {
            if (f.n_positional != f.n_input) {
                PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_INVALID_PARAM);
                PikaVMFrame_setSysOut(
                    vm,
                    "TypeError: %s() takes %d positional argument but %d "
                    "were "
                    "given",
                    sMethodName, f.n_positional, f.n_input);
                goto __exit;
            }
            break;
        }
#if !PIKA_NANO_ENABLE
        if (f.is_keys) {
            break;
        }
        if (f.is_vars) {
            if (f.n_input < f.n_positional) {
                PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_INVALID_PARAM);
                PikaVMFrame_setSysOut(
                    vm,
                    "TypeError: %s() takes %d positional argument but "
                    "%d "
                    "were "
                    "given",
                    sMethodName, f.n_positional, f.n_input);
                goto __exit;
            }
            break;
        }
        if (f.is_default) {
            int8_t n_min = f.n_positional;
            int8_t n_max = f.n_positional + f.n_default;
            if (f.n_input < n_min || f.n_input > n_max) {
                PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_INVALID_PARAM);
                PikaVMFrame_setSysOut(
                    vm,
                    "TypeError: %s() takes from %d to %d positional "
                    "arguments "
                    "but %d were given",
                    sMethodName, n_min, n_max, f.n_input);
                goto __exit;
            }
        }
#endif
    } while (0);

    if (vars_or_keys_or_default) {
        f.n_arg = f.n_input;
    } else {
        f.n_arg = f.n_positional;
    }

    /* create tuple/dict for vars/keys */
    if (vars_or_keys_or_default) {
        if (strGetSize(f.type_list) > METHOD_TYPE_LIST_MAX_LEN) {
            pika_platform_printf(
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
                f.kw = New_PikaDict();
                /* remove the format arg */
                strPopLastToken(f.type_list, ',');
                continue;
            }
        }
    }

    /* load args */
    for (int i = 0; i < f.n_arg; i++) {
        Arg* call_arg = NULL;
        f.i_arg = f.n_arg - i;
        if (NULL != sProxyName && i == f.n_arg - 1) {
            call_arg = arg_newStr(sProxyName);
        } else {
            call_arg = stack_popArg_alloc(&(vm->stack));
        }
        if (NULL == call_arg) {
            call_arg = arg_newNone();
        }
        _load_call_arg(vm, call_arg, &f, &i, &argc, argv);
    }

/* only default */
#if !PIKA_NANO_ENABLE
    if (strIsContain(f.type_list, '=')) {
        char* arg_name = strPopLastToken(f.type_list, ',');
        _kw_pos_to_default_all(&f, arg_name, &argc, argv, NULL);
    }
    /* load kw to pos */
    _kw_to_pos_all(&f, &argc, argv);
#endif

    if (f.tuple != NULL) {
        pikaList_reverse(f.tuple);
        /* load variable tuple */
        Arg* argi =
            arg_setPtr(NULL, f.var_tuple_name, ARG_TYPE_OBJECT, f.tuple);
        argv[argc++] = argi;
    }

    if (f.kw != NULL) {
        if (NULL == f.kw_dict_name) {
            f.kw_dict_name = "__kwargs";
        }
        /* load kw dict */
        PikaObj* New_PikaStdData_Dict(Args * args);
        Arg* argi = arg_setPtr(NULL, f.kw_dict_name, ARG_TYPE_OBJECT, f.kw);
        argv[argc++] = argi;
    }

    /* load 'self' as the first arg when call object method */
    if (f.method_type == ARG_TYPE_METHOD_OBJECT) {
        PikaObj* method_self = NULL;
        method_self = methodArg_getHostObj(aMethod);
        if (NULL == method_self) {
            method_self = oMethodHost;
        }

        Arg* call_arg = arg_setRef(NULL, "self", method_self);
        pika_assert(call_arg != NULL);
        argv[argc++] = call_arg;
    }
    _loadLocalsFromArgv(aLoclas, argc, argv);
__exit:
    pikaFree(buffs1, METHOD_TYPE_LIST_MAX_LEN);
    pikaFree(buffs2, METHOD_TYPE_LIST_MAX_LEN);
    pikaFree(argv, sizeof(Arg*) * PIKA_ARG_NUM_MAX);
    return f.n_arg;
}

#if PIKA_BUILTIN_STRUCT_ENABLE
PikaObj* New_PikaStdData_List(Args* args);
PikaObj* New_PikaStdData_Tuple(Args* args);
#endif

static Arg* _vm_create_list_or_tuple(PikaObj* self,
                                     PikaVMFrame* vm,
                                     pika_bool is_list) {
#if PIKA_BUILTIN_STRUCT_ENABLE
    NewFun constructor = is_list ? New_PikaStdData_List : New_PikaStdData_Tuple;
    uint32_t n_arg = PikaVMFrame_getInputArgNum(vm);
    PikaObj* list = newNormalObj(constructor);
    pikaList_init(list);
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
        pikaList_append(list, arg);
    }
    stack_deinit(&stack);
    return arg_newObj(list);
#else
    return VM_instruction_handler_NON(self, vm, "", NULL);
#endif
}

static Arg* VM_instruction_handler_LST(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
#if PIKA_BUILTIN_STRUCT_ENABLE
    PikaObj* list = newNormalObj(New_PikaStdData_List);
    pikaList_init(list);
    vm->oreg[PikaVMFrame_getInvokeDeepthNow(vm)] = list;
#endif
    return NULL;
}

static Arg* VM_instruction_handler_NLS(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
#if PIKA_BUILTIN_STRUCT_ENABLE
    PikaObj* list = vm->oreg[PikaVMFrame_getInvokeDeepthNow(vm)];
    vm->oreg[PikaVMFrame_getInvokeDeepthNow(vm)] = NULL;
    return arg_newObj(list);
#else
    return VM_instruction_handler_NON(self, vm, "", NULL);
#endif
}

static Arg* VM_instruction_handler_TPL(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    return _vm_create_list_or_tuple(self, vm, pika_false);
}

#if PIKA_BUILTIN_STRUCT_ENABLE
PikaObj* New_PikaStdData_Dict(Args* args);
#endif

static Arg* VM_instruction_handler_DCT(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
#if PIKA_BUILTIN_STRUCT_ENABLE
    uint32_t n_arg = PikaVMFrame_getInputArgNum(vm);
    PikaObj* dict = New_PikaDict();
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
        pikaDict_set(dict, arg_getStr(key_arg), val_arg);
        arg_deinit(key_arg);
    }
    stack_deinit(&stack);
    return arg_newObj(dict);
#else
    return VM_instruction_handler_NON(self, vm, data, arg_ret_reg);
#endif
}

static Arg* VM_instruction_handler_RET(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    /* exit jmp signal */
    vm->jmp = VM_JMP_EXIT;
    Arg* aReturn = stack_popArg_alloc(&(vm->stack));
    /* set gc root to avoid gc */
    arg_setObjFlag(aReturn, OBJ_FLAG_GC_ROOT);
    method_returnArg(vm->locals->list, aReturn);
    return NULL;
}

InstructUnit* byteCodeFrame_findInstructUnit(ByteCodeFrame* bcframe,
                                             int32_t iPcStart,
                                             enum InstructIndex index,
                                             int32_t* iOffset_p,
                                             pika_bool bIsForward) {
    /* find instruct unit */
    int instructArray_size = instructArray_getSize(&(bcframe->instruct_array));
    while (1) {
        *iOffset_p += (bIsForward ? -1 : 1) * instructUnit_getSize();
        if (iPcStart + *iOffset_p >= instructArray_size) {
            return NULL;
        }
        if (iPcStart + *iOffset_p < 0) {
            return NULL;
        }
        InstructUnit* unit = instructArray_getByOffset(
            &(bcframe->instruct_array), iPcStart + *iOffset_p);
        if (index == instructUnit_getInstructIndex(unit)) {
            return unit;
        }
    }
}

InstructUnit* byteCodeFrame_findInsForward(ByteCodeFrame* bcframe,
                                           int32_t pc_start,
                                           enum InstructIndex index,
                                           int32_t* p_offset) {
    return byteCodeFrame_findInstructUnit(bcframe, pc_start, index, p_offset,
                                          pika_true);
}

Hash byteCodeFrame_getNameHash(ByteCodeFrame* bcframe) {
    if (0 == bcframe->name_hash) {
        bcframe->name_hash = hash_time33(bcframe->name);
    }
    return bcframe->name_hash;
}

InstructUnit* byteCodeFrame_findInsUnitBackward(ByteCodeFrame* bcframe,
                                                int32_t pc_start,
                                                enum InstructIndex index,
                                                int32_t* p_offset) {
    return byteCodeFrame_findInstructUnit(bcframe, pc_start, index, p_offset,
                                          pika_false);
}

char* _find_super_class_name(ByteCodeFrame* bcframe, int32_t pc_start) {
    /* find super class */
    int32_t offset = 0;
    char* super_class_name = NULL;
    byteCodeFrame_findInsForward(bcframe, pc_start, PIKA_INS(CLS), &offset);
    InstructUnit* unit_run = byteCodeFrame_findInsUnitBackward(
        bcframe, pc_start, PIKA_INS(RUN), &offset);
    super_class_name = constPool_getByOffset(
        &(bcframe->const_pool), instructUnit_getConstPoolIndex(unit_run));
    return super_class_name;
}

#if !PIKA_NANO_ENABLE
static char* _find_self_name(PikaVMFrame* vm) {
    /* find super class */
    int offset = 0;
    char* self_name = NULL;
    while (1) {
        offset -= instructUnit_getSize();
        if (vm->pc + offset >= (int)PikaVMFrame_getInstructArraySize(vm)) {
            return 0;
        }
        if ((PIKA_INS(CLS) == PikaVMFrame_getInstructWithOffset(vm, offset))) {
            break;
        }
    }

    while (1) {
        offset += instructUnit_getSize();
        if (vm->pc + offset >= (int)PikaVMFrame_getInstructArraySize(vm)) {
            return 0;
        }
        if ((PIKA_INS(OUT) ==
             instructUnit_getInstructIndex(
                 PikaVMFrame_getInstructUnitWithOffset(vm, offset)))) {
            self_name = PikaVMFrame_getConstWithOffset(vm, offset);
            return self_name;
        }
    }
}
#endif

PikaObj* New_builtins_object(Args* args);

static Arg* _VM_instruction_eval(PikaObj* self,
                                 PikaVMFrame* vm,
                                 char* sRunPath,
                                 pika_bool* bIsEval) {
    Arg* aReturn = NULL;
    Args buffs = {0};
    *bIsEval = pika_false;
    if (sRunPath[0] != 'e') {
        return NULL;
    }
    if (!strEqu(sRunPath, "eval") && !strEqu(sRunPath, "exec")) {
        return NULL;
    }
    /* eval || exec */
    *bIsEval = pika_true;
    ByteCodeFrame bcFrame = {0};
    /* generate byte code */
    byteCodeFrame_init(&bcFrame);
    Arg* aCode = stack_popArg_alloc(&(vm->stack));
    char* sCode = arg_getStr(aCode);
    char* sCmd = strsAppend(&buffs, "@res = ", sCode);
    if (PIKA_RES_OK != pika_lines2Bytes(&bcFrame, sCmd)) {
        PikaVMFrame_setSysOut(vm, PIKA_ERR_STRING_SYNTAX_ERROR);
        aReturn = NULL;
        goto __exit;
    }
    _pikaVM_runByteCodeFrameWithState(self, vm->locals, vm->globals, &bcFrame,
                                      0, vm->vm_thread);

    aReturn = obj_getArg(vm->locals, "@res");
    if (NULL == aReturn) {
        aReturn = obj_getArg(vm->globals, "@res");
    }
    if (strEqu(sRunPath, "eval")) {
        aReturn = arg_copy(aReturn);
    }
    goto __exit;
__exit:
    byteCodeFrame_deinit(&bcFrame);
    arg_deinit(aCode);
    strsDeinit(&buffs);
    return aReturn;
}

static Arg* VM_instruction_handler_RUN(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* aReturnRegistor) {
    Arg* aReturn = NULL;
    VMParameters* oSublocals = NULL;
    VMParameters* oSublocalsInit = NULL;
    char* sRunPath = data;
    char* sArgName = NULL;
    char* sProxyName = NULL;
    PikaObj* oMethodHost = NULL;
    PikaObj* oThis = NULL;
    Arg* aMethod = NULL;
    Arg* aStack = NULL;
    pika_bool bIsTemp = pika_false;
    pika_bool bSkipInit = pika_false;
    pika_bool bIsEval = pika_false;
    int iNumUsed = 0;
    PikaObj* oBuiltin = NULL;
    arg_newReg(arg_reg1, 32);
    pika_assert(NULL != vm->vm_thread);

    if (NULL != sRunPath) {
        sArgName = strPointToLastToken(sRunPath, '.');
    }

    /* inhert */
    if (vm->pc - 2 * (int)instructUnit_getSize() >= 0) {
        if (PIKA_INS(CLS) == PikaVMFrame_getInstructWithOffset(
                                 vm, -2 * (int)instructUnit_getSize())) {
            bSkipInit = pika_true;
        }
    }

    /* tuple or single arg */
    if (NULL == sRunPath || sRunPath[0] == 0) {
        if (PikaVMFrame_getInputArgNum(vm) == 1) {
            /* return arg directly */
            aReturn = stack_popArg(&(vm->stack), aReturnRegistor);
            goto __exit;
        }
        /* create a tuple */
        aReturn = _vm_create_list_or_tuple(self, vm, pika_false);
        goto __exit;
    }

#if !PIKA_NANO_ENABLE
    /* support for super() */
    if (strEqu(sRunPath, "super")) {
        sRunPath = _find_super_class_name(vm->bytecode_frame, vm->pc);
        sArgName = strPointToLastToken(sRunPath, '.');
        vm->in_super = pika_true;
        vm->super_invoke_deepth = PikaVMFrame_getInvokeDeepthNow(vm);
        bSkipInit = pika_true;
    }
#endif

    /* return tiny obj */
    if (strEqu(sRunPath, "TinyObj")) {
        aReturn = arg_newMetaObj(New_builtins_object);
        if (NULL != aReturn) {
            goto __exit;
        }
    }

    /* eval and exec */
    aReturn = _VM_instruction_eval(self, vm, sRunPath, &bIsEval);
    if (bIsEval) {
        goto __exit;
    }

    /* get method host obj from reg */
    if (NULL == oMethodHost) {
        oMethodHost = Locals_getLReg(vm->locals, sRunPath);
    }

#if !PIKA_NANO_ENABLE
    /* get method host obj from stack */
    if (NULL == oMethodHost && sRunPath[0] == '.') {
        /* get method host obj from stack */
        Arg** stack_tmp = (Arg**)pikaMalloc(sizeof(Arg*) * PIKA_ARG_NUM_MAX);
        uint32_t n_arg = PikaVMFrame_getInputArgNum(vm);
        if (n_arg > PIKA_ARG_NUM_MAX) {
            pika_platform_printf(
                "[ERROR] Too many args in RUN instruction, please use "
                "bigger "
                "#define PIKA_ARG_NUM_MAX\n");
            pika_platform_panic_handle();
        }
        for (int i = 0; i < n_arg; i++) {
            stack_tmp[i] = stack_popArg_alloc(&(vm->stack));
        }
        aStack = stack_tmp[n_arg - 1];
        if (sRunPath[1] == '\0') {
            /* for .(...) */
            aMethod = aStack;
            pika_assert(arg_isCallable(aMethod));
        } else {
            /* for .xxx(...) */
            oMethodHost = _arg_to_obj(aStack, &bIsTemp);
            pika_assert(NULL != oMethodHost);
        }
        if (NULL != aStack) {
            iNumUsed++;
        }
        /* push back other args to stack */
        for (int i = n_arg - 2; i >= 0; i--) {
            stack_pushArg(&(vm->stack), stack_tmp[i]);
        }
        pikaFree(stack_tmp, sizeof(Arg*) * PIKA_ARG_NUM_MAX);
    }
#endif
    /* use RunAs object */
    if (obj_getFlag(vm->locals, OBJ_FLAG_RUN_AS)) {
        PikaObj* oContext = args_getPtr(vm->locals->list, "@r");
        oMethodHost = obj_getHostObjWithIsTemp(oContext, sRunPath, &bIsTemp);
    }

    /* get method host obj from local scope */
    if (NULL == oMethodHost) {
        oMethodHost = obj_getHostObjWithIsTemp(vm->locals, sRunPath, &bIsTemp);
    }

    /* get method host obj from global scope */
    if (NULL == oMethodHost) {
        oMethodHost = obj_getHostObjWithIsTemp(vm->globals, sRunPath, &bIsTemp);
    }

    /* method host obj is not found */
    if (NULL == oMethodHost) {
        /* error, not found object */
        PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_ARG_NO_FOUND);
        PikaVMFrame_setSysOut(vm, "Error: method '%s' no found.", sRunPath);
        goto __exit;
    }

    pika_assert(obj_checkAlive(oMethodHost));

#if !PIKA_NANO_ENABLE
    if (!bSkipInit && vm->in_super &&
        PikaVMFrame_getInvokeDeepthNow(vm) == vm->super_invoke_deepth - 1) {
        vm->in_super = pika_false;
        oThis = obj_getPtr(vm->locals, _find_self_name(vm));
    }
#endif

    /* get object this */
    if (NULL == oThis) {
        oThis = oMethodHost;
    }

    /* get method in object */
    if (NULL == aMethod) {
        aMethod = obj_getMethodArg_noalloc(oMethodHost, sArgName, &arg_reg1);
    }

    if (sArgName == sRunPath) {
        /* find method in locals */
        if (NULL == aMethod) {
            aMethod = obj_getMethodArg_noalloc(vm->locals, sArgName, &arg_reg1);
        }
        /* find method in global */
        if (NULL == aMethod) {
            aMethod =
                obj_getMethodArg_noalloc(vm->globals, sArgName, &arg_reg1);
            if (aMethod != NULL) {
                oThis = vm->globals;
            }
        }
        /* find method in builtin */
        if (NULL == aMethod) {
            oBuiltin = obj_getBuiltins();
            aMethod = obj_getMethodArg_noalloc(oBuiltin, sArgName, &arg_reg1);
            if (aMethod != NULL) {
                oThis = oBuiltin;
            }
        }
    }

    if (NULL == aMethod) {
        /* get proxy method */
        if (obj_getFlag(oMethodHost, OBJ_FLAG_PROXY_METHOD)) {
            if (strCountSign(sArgName, '.') == 0) {
                /* __proxy__ takes the first arg as the method name */
                sProxyName = sArgName;
                sArgName = "__proxy__";
                aMethod =
                    obj_getMethodArg_noalloc(oMethodHost, sArgName, &arg_reg1);
            }
        }
    }

    /* assert method exist */
    if (NULL == aMethod || ARG_TYPE_NONE == arg_getType(aMethod)) {
        /* error, method no found */
        PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_ARG_NO_FOUND);
        PikaVMFrame_setSysOut(vm, "NameError: name '%s' is not defined",
                              sRunPath);
        goto __exit;
    }

    /* assert methodd type */
    if (!argType_isCallable(arg_getType(aMethod))) {
        /* error, method no found */
        PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_ARG_NO_FOUND);
        PikaVMFrame_setSysOut(vm, "TypeError: '%s' object is not callable",
                              sRunPath);
        goto __exit;
    }

    /* create sub local scope */
    oSublocals = New_Locals(NULL);
    oThis->vmFrame = vm;

    /* load args from PikaVMFrame to sub_local->list */
    iNumUsed += PikaVMFrame_loadArgsFromMethodArg(
        vm, oThis, oSublocals->list, aMethod, sRunPath, sProxyName, iNumUsed);

    /* load args failed */
    if (pikaVMFrame_checkErrorStack(vm) != PIKA_RES_OK) {
        goto __exit;
    }

    /* run method arg */
    aReturn = obj_runMethodArgWithState_noalloc(oThis, oSublocals, aMethod,
                                                vm->vm_thread, aReturnRegistor);
    if (bSkipInit) {
        if (arg_getType(aReturn) == ARG_TYPE_OBJECT_NEW) {
            pika_assert(NULL != aReturn);
            arg_setType(aReturn, ARG_TYPE_OBJECT);
        }
    }

    if (vm->vm_thread->try_result != TRY_RESULT_NONE) {
        /* try result */
        vm->error.code = vm->vm_thread->try_result;
    }

    /* __init__() */
    if (NULL != aReturn && ARG_TYPE_OBJECT_NEW == arg_getType(aReturn)) {
        pika_assert(NULL != aReturn);
        arg_setType(aReturn, ARG_TYPE_OBJECT);
        /* init object */
        PikaObj* oNew = arg_getPtr(aReturn);
        obj_setName(oNew, sRunPath);
        Arg* aMethod =
            obj_getMethodArgWithFullPath_noalloc(oNew, "__init__", &arg_reg1);
        oSublocalsInit = New_Locals(NULL);
        Arg* aReturnInit = NULL;
        if (NULL == aMethod) {
            goto __init_exit;
        }
        PikaVMFrame_loadArgsFromMethodArg(vm, oNew, oSublocalsInit->list,
                                          aMethod, "__init__", NULL, iNumUsed);
        /* load args failed */
        if (pikaVMFrame_checkErrorStack(vm) != PIKA_RES_OK) {
            goto __init_exit;
        }
        aReturnInit = obj_runMethodArgWithState(oNew, oSublocalsInit, aMethod,
                                                vm->vm_thread);
    __init_exit:
        if (NULL != aReturnInit) {
            arg_deinit(aReturnInit);
        }
#if PIKA_GC_MARK_SWEEP_ENABLE
        pika_assert(obj_getFlag(oSublocals, OBJ_FLAG_GC_ROOT));
#endif
        obj_deinit(oSublocalsInit);
        if (NULL != aMethod) {
            arg_deinit(aMethod);
        }
    }

    goto __exit;
__exit:
    if (NULL != aMethod) {
        arg_deinit(aMethod);
    }
    if (NULL != oSublocals) {
#if PIKA_GC_MARK_SWEEP_ENABLE
        pika_assert(obj_getFlag(oSublocals, OBJ_FLAG_GC_ROOT));
#endif
        obj_deinit(oSublocals);
    }
    if (NULL != oBuiltin) {
        obj_deinit(oBuiltin);
    }
    if (NULL != aStack && aMethod != aStack) {
        arg_deinit(aStack);
    }
    if (NULL != oMethodHost && bIsTemp) {
        /* class method */
        obj_GC(oMethodHost);
    }
    pika_assert_arg_alive(aReturn);
    return aReturn;
}

static Arg* VM_instruction_handler_STR(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    if (NULL == data) {
        return arg_setStr(arg_ret_reg, "", "");
    }
    if (strIsContain(data, '\\')) {
        Args buffs = {0};
        size_t i_out = 0;
        char* transfered_str = strsTransfer(&buffs, data, &i_out);
        Arg* return_arg = arg_ret_reg;
        return_arg = arg_setStr(return_arg, "", transfered_str);
        strsDeinit(&buffs);
        return return_arg;
    }
    return arg_setStr(arg_ret_reg, "", data);
}

static Arg* VM_instruction_handler_BYT(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    if (strIsContain(data, '\\')) {
        Args buffs = {0};
        size_t i_out = 0;
        char* transfered_str = strsTransfer(&buffs, data, &i_out);
        Arg* return_arg = New_arg(NULL);
        return_arg =
            arg_setBytes(return_arg, "", (uint8_t*)transfered_str, i_out);
        strsDeinit(&buffs);
        return return_arg;
    }
    return arg_newBytes((uint8_t*)data, strGetSize(data));
}

static pika_bool _proxy_setattr(PikaObj* self, char* name, Arg* arg) {
#if PIKA_NANO_ENABLE
    return pika_false;
#endif
    if (obj_getFlag(self, OBJ_FLAG_PROXY_SETATTR)) {
        obj_setStr(self, "@name", name);
        obj_setArg_noCopy(self, "@value", arg);
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
        return pika_true;
    }
    return pika_false;
}

static Arg* VM_instruction_handler_OUT(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* aRetReg) {
    char* sArgPath = data;
    pika_assert(sArgPath != NULL);
    char* sArgName = strPointToLastToken(sArgPath, '.');
    PikaObj* oHost = NULL;
    pika_bool bIsTemp = pika_false;
    arg_newReg(aOutReg, PIKA_ARG_BUFF_SIZE);
    PIKA_RES res = PIKA_RES_OK;
    Arg* aOut = stack_popArg(&vm->stack, &aOutReg);
    if (NULL == aOut) {
        goto __exit;
    }
    ArgType eOutArgType = arg_getType(aOut);
    if (PikaVMFrame_getInvokeDeepthNow(vm) > 0) {
        /* in block, is a kw arg */
        arg_setIsKeyword(aOut, pika_true);
        arg_setName(aOut, sArgPath);
        Arg* res = arg_copy_noalloc(aOut, aRetReg);
        arg_deinit(aOut);
        return res;
    }

    if (_checkLReg(sArgPath)) {
        if (argType_isObject(eOutArgType)) {
            PikaObj* obj = arg_getPtr(aOut);
            Locals_setLReg(vm->locals, sArgPath, obj);
            arg_deinit(aOut);
        }
        goto __exit;
    }

    PikaObj* oContext = vm->locals;
    /* match global_list */
    if (obj_getFlag(vm->locals, OBJ_FLAG_GLOBALS)) {
        char* sGlobalList = args_getStr(vm->locals->list, "@g");
        /* use a arg as buff */
        Arg* aGlobalList = arg_newStr(sGlobalList);
        char* sGlobalListBuff = arg_getStr(aGlobalList);
        /* for each arg arg in global_list */
        for (int i = 0; i < strCountSign(sGlobalList, ',') + 1; i++) {
            char* sGlobalArg = strPopFirstToken(&sGlobalListBuff, ',');
            /* matched global arg, context set to global */
            if (strEqu(sGlobalArg, sArgPath)) {
                oContext = vm->globals;
            }
        }
        arg_deinit(aGlobalList);
    }
    /* use RunAs object */
    if (obj_getFlag(vm->locals, OBJ_FLAG_RUN_AS)) {
        oContext = args_getPtr(vm->locals->list, "@r");
    }
    /* set free object to nomal object */
    if (ARG_TYPE_OBJECT_NEW == eOutArgType) {
        pika_assert(NULL != aOut);
        arg_setType(aOut, ARG_TYPE_OBJECT);
    }

    /* ouput arg to context */
    if (sArgPath == sArgName) {
        res = obj_setArg_noCopy(oContext, sArgPath, aOut);
        goto __exit;
    }

    oHost = obj_getHostObjWithIsTemp(oContext, sArgPath, &bIsTemp);

    if (NULL == oHost) {
        oHost = obj_getHostObjWithIsTemp(vm->globals, sArgPath, &bIsTemp);
    }

    if (oHost != NULL) {
        if (_proxy_setattr(oHost, sArgName, aOut)) {
            goto __exit;
        }
        res = obj_setArg_noCopy(oHost, sArgName, aOut);
        goto __exit;
    }

    res = obj_setArg_noCopy(oContext, sArgPath, aOut);
__exit:
    if (res != PIKA_RES_OK) {
        PikaVMFrame_setErrorCode(vm, res);
        PikaVMFrame_setSysOut(vm, "Error: can't set '%s'", sArgPath);
    }
    return NULL;
}

/* run as */
static Arg* VM_instruction_handler_RAS(PikaObj* self,
                                       PikaVMFrame* vm,
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
                                       PikaVMFrame* vm,
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
        pika_platform_memcpy(strtoll_buff + 1, data + 2, strGetSize(data) - 2);
        return arg_setInt(arg_ret_reg, "", strtoll(strtoll_buff, NULL, 0));
    }
    if (data[1] == 'b' || data[1] == 'B') {
        char strtoll_buff[10] = {0};
        pika_platform_memcpy(strtoll_buff, data + 2, strGetSize(data) - 2);
        return arg_setInt(arg_ret_reg, "", strtoll(strtoll_buff, NULL, 2));
    }
    /* float */
    if (strIsContain(data, '.') ||
        (strIsContain(data, 'e') || strIsContain(data, 'E'))) {
        return arg_setFloat(arg_ret_reg, "", strtod(data, NULL));
    }
    /* int */
    int64_t i64 = 0;
    if (PIKA_RES_OK != fast_atoi_safe(data, &i64)) {
        PikaVMFrame_setSysOut(vm, "ValueError: invalid literal for int(): '%s'",
                              data);
        PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
        return NULL;
    }
    return arg_setInt(arg_ret_reg, "", i64);
}

static Arg* VM_instruction_handler_JMP(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    vm->jmp = fast_atoi(data);
    return NULL;
}

static Arg* VM_instruction_handler_SER(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    PikaVMError error = {0};
    error.code = fast_atoi(data);
    pikaVMThread_pushError(vm->vm_thread, &error);
    return NULL;
}

static Arg* _VM_JEZ(PikaObj* self,
                    PikaVMFrame* vm,
                    char* data,
                    Arg* arg_ret_reg,
                    int bAssert) {
    int thisBlockDeepth = PikaVMFrame_getBlockDeepthNow(vm);
    int jmp_expect = fast_atoi(data);
    vm->ireg[thisBlockDeepth] = (pika_bool)!bAssert;

    if (0 == bAssert) {
        /* jump */
        vm->jmp = jmp_expect;
    }

    /* restore loop deepth */
    if (2 == jmp_expect && 0 == bAssert) {
        int block_deepth_now = PikaVMFrame_getBlockDeepthNow(vm);
        vm->loop_deepth = block_deepth_now;
    }

    return NULL;
}

static Arg* VM_instruction_handler_JEZ(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    Arg* aAssert = stack_popArg(&(vm->stack), arg_ret_reg);
    pika_bool bAssert = 0;
    if (NULL != aAssert) {
        PIKA_RES res = _transeBool(aAssert, &bAssert);
        if (PIKA_RES_OK != res) {
            bAssert = 0;
        }
        arg_deinit(aAssert);
    }
    return _VM_JEZ(self, vm, data, arg_ret_reg, bAssert);
}

static Arg* VM_instruction_handler_JNZ(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    Arg* aAssert = stack_popArg(&(vm->stack), arg_ret_reg);
    pika_bool bAssert = 0;
    if (NULL != aAssert) {
        PIKA_RES res = _transeBool(aAssert, &bAssert);
        if (PIKA_RES_OK != res) {
            bAssert = 0;
        }
        arg_deinit(aAssert);
    }
    return _VM_JEZ(self, vm, data, arg_ret_reg, !bAssert);
}

static uint32_t PikaVMFrame_getInputArgNum(PikaVMFrame* vm) {
    InstructUnit* ins_unit_now = PikaVMFrame_getInstructNow(vm);
    uint8_t invoke_deepth_this = instructUnit_getInvokeDeepth(ins_unit_now);
    int32_t pc_this = vm->pc;
    uint32_t num = 0;
    while (1) {
        ins_unit_now--;
        pc_this -= instructUnit_getSize();
        uint8_t invode_deepth = instructUnit_getInvokeDeepth(ins_unit_now);
        if (pc_this < 0) {
            break;
        }
        if (invode_deepth == invoke_deepth_this + 1) {
            if (instructUnit_getInstructIndex(ins_unit_now) == PIKA_INS(OUT)) {
                continue;
            }
            if (instructUnit_getInstructIndex(ins_unit_now) == PIKA_INS(NLS)) {
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

int operatorInfo_init(OperatorInfo* info,
                      PikaObj* self,
                      PikaVMFrame* vm,
                      char* data,
                      Arg* arg_ret_reg) {
    if (info->a1 == NULL && info->a2 == NULL) {
        return -1;
    }
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
        } else if (info->t1 == ARG_TYPE_BOOL) {
            info->i1 = arg_getBool(info->a1);
            info->f1 = (pika_float)info->i1;
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
    } else if (info->t2 == ARG_TYPE_BOOL) {
        info->i2 = arg_getBool(info->a2);
        info->f2 = (pika_float)info->i2;
    }
    return 0;
}

static Arg* _OPT_Method_ex(PikaObj* host,
                           Arg* arg,
                           OperatorInfo* op,
                           char* method_name,
                           PIKA_RES err_no,
                           char* errinfo) {
    Arg* method = obj_getMethodArgWithFullPath(host, method_name);
    if (NULL == method) {
        PikaVMFrame_setErrorCode(op->vm, err_no);
        PikaVMFrame_setSysOut(op->vm, errinfo);
        return NULL;
    }
    Arg* res = obj_runMethodArg1(host, method, arg_copy(arg));
    return res;
}

static Arg* _OPT_Method(OperatorInfo* op,
                        char* method_name,
                        PIKA_RES err_no,
                        char* errinfo) {
    PikaObj* obj1 = arg_getPtr(op->a1);
    return _OPT_Method_ex(obj1, op->a2, op, method_name, err_no, errinfo);
}

static void _OPT_ADD(OperatorInfo* op) {
#if !PIKA_NANO_ENABLE
    if (argType_isObject(op->t1)) {
        if (!argType_isObject(op->t2)) {
            PikaVMFrame_setErrorCode(op->vm, PIKA_RES_ERR_OPERATION_FAILED);
            PikaVMFrame_setSysOut(op->vm, "TypeError: unsupported operand +");
            op->res = NULL;
            return;
        }
        op->res = _OPT_Method(op, "__add__", PIKA_RES_ERR_OPERATION_FAILED,
                              "TypeError: unsupported operand +");
        return;
    }
#endif
    // Check if either argument is a string and the other is not a string
    if ((op->t1 == ARG_TYPE_STRING && op->t2 != ARG_TYPE_STRING) ||
        (op->t2 == ARG_TYPE_STRING && op->t1 != ARG_TYPE_STRING)) {
        PikaVMFrame_setErrorCode(op->vm, PIKA_RES_ERR_OPERATION_FAILED);
        PikaVMFrame_setSysOut(
            op->vm, "TypeError: unsupported operand + between str and non-str");
        op->res = NULL;
        return;
    }
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
        pika_platform_memcpy(bytes_out, bytes1, size1);
        pika_platform_memcpy(bytes_out + size1, bytes2, size2);
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
            PikaVMFrame_setErrorCode(op->vm, PIKA_RES_ERR_OPERATION_FAILED);
            PikaVMFrame_setSysOut(op->vm, "TypeError: unsupported operand +");
            op->res = NULL;
            return;
        }
        op->res = _OPT_Method(op, "__sub__", PIKA_RES_ERR_OPERATION_FAILED,
                              "TypeError: unsupported operand -");
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
        goto __exit;
    }
    /* type not equl, and type is not int or float or bool*/
    if (!argType_isEqual(op->t1, op->t2)) {
        if ((op->t1 != ARG_TYPE_FLOAT) && (op->t1 != ARG_TYPE_INT) &&
            (op->t1 != ARG_TYPE_BOOL)) {
            is_equ = 0;
            goto __exit;
        }
        if ((op->t2 != ARG_TYPE_FLOAT) && (op->t2 != ARG_TYPE_INT) &&
            (op->t2 != ARG_TYPE_BOOL)) {
            is_equ = 0;
            goto __exit;
        }
    }
    /* string compire */
    if (op->t1 == ARG_TYPE_STRING) {
        is_equ = strEqu(arg_getStr(op->a1), arg_getStr(op->a2));
        goto __exit;
    }
    /* bytes compire */
    if (op->t1 == ARG_TYPE_BYTES) {
        if (arg_getBytesSize(op->a1) != arg_getBytesSize(op->a2)) {
            is_equ = 0;
            goto __exit;
        }
        is_equ = 1;
        for (size_t i = 0; i < arg_getBytesSize(op->a1); i++) {
            if (arg_getBytes(op->a1)[i] != arg_getBytes(op->a2)[i]) {
                is_equ = 0;
                goto __exit;
            }
        }
        goto __exit;
    }
    if (argType_isCallable(op->t1) && argType_isCallable(op->t2)) {
        is_equ = (arg_getPtr(op->a1) == arg_getPtr(op->a2));
        goto __exit;
    }
    if (argType_isObject(op->t1) && argType_isObject(op->t2)) {
        is_equ = (arg_getPtr(op->a1) == arg_getPtr(op->a2));
        if (is_equ) {
            goto __exit;
        }
        Arg* __eq__ =
            obj_getMethodArgWithFullPath(arg_getPtr(op->a1), "__eq__");
        if (NULL == __eq__) {
            goto __exit;
        }
        arg_deinit(__eq__);
        Arg* res = _OPT_Method(op, "__eq__", PIKA_RES_ERR_OPERATION_FAILED,
                               "TypeError: unsupported operand ==");
        if (NULL == res) {
            is_equ = 0;
            goto __exit;
        }
        is_equ = arg_getBool(res);
        arg_deinit(res);
        goto __exit;
    }
    /* default: int bool, and float */
    is_equ = ((op->f1 - op->f2) * (op->f1 - op->f2) < (pika_float)0.000001);
    goto __exit;
__exit:
    if (op->opt[0] == '=') {
        op->res = arg_setBool(op->res, "", is_equ);
    } else {
        op->res = arg_setBool(op->res, "", !is_equ);
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
        int lhs = op->i1;
        int rhs = op->i2;
        if (rhs < 0)
            rhs = -rhs;
        int64_t ret = 1;
        while (rhs) {
            if (rhs & 1)
                ret *= lhs;
            lhs *= lhs;
            rhs >>= 1;
        }
        if (op->i2 < 0) {
            op->res = arg_setFloat(op->res, "", 1.0 / ret);
        } else {
            op->res = arg_setInt(op->res, "", ret);
        }
        return;
    }
    if (op->t1 == ARG_TYPE_FLOAT && op->t2 == ARG_TYPE_INT) {
        float res = 1;
        for (int i = 0; i < op->i2; i++) {
            res = res * op->f1;
        }
        op->res = arg_setFloat(op->res, "", res);
        return;
    }
#if PIKA_MATH_ENABLE
    float res = 1;
    res = pow(op->f1, op->f2);
    op->res = arg_setFloat(op->res, "", res);
    return;
#else
    PikaVMFrame_setErrorCode(op->vm, PIKA_RES_ERR_OPERATION_FAILED);
    PikaVMFrame_setSysOut(op->vm,
                          "Operation float ** float is not enabled, please set "
                          "PIKA_MATH_ENABLE");
#endif
}

static void _OPT_MUL(OperatorInfo* op) {
    if (op->num == 1) {
        op->res = arg_copy(op->a2);
        arg_setIsStarred(op->res, 1);
        return;
    }
    if ((op->t1 == ARG_TYPE_FLOAT) || op->t2 == ARG_TYPE_FLOAT) {
        op->res = arg_setFloat(op->res, "", op->f1 * op->f2);
        return;
    }
    if ((op->t1 == ARG_TYPE_STRING && op->t2 == ARG_TYPE_INT) ||
        (op->t1 == ARG_TYPE_INT && op->t2 == ARG_TYPE_STRING)) {
        char* str = NULL;
        int64_t num = 0;
        if (op->t1 == ARG_TYPE_STRING) {
            str = arg_getStr(op->a1);
            num = op->i2;
        } else {
            str = arg_getStr(op->a2);
            num = op->i1;
        }
        Args str_opt_buffs = {0};
        char* opt_str_out = strsRepeat(&str_opt_buffs, str, num);
        op->res = arg_setStr(op->res, "", opt_str_out);
        strsDeinit(&str_opt_buffs);
        return;
    }
    if ((op->t1 == ARG_TYPE_BYTES && op->t2 == ARG_TYPE_INT) ||
        (op->t1 == ARG_TYPE_INT && op->t2 == ARG_TYPE_BYTES)) {
        uint8_t* bytes = NULL;
        size_t size = 0;
        int64_t num = 0;
        if (op->t1 == ARG_TYPE_BYTES) {
            bytes = arg_getBytes(op->a1);
            size = arg_getBytesSize(op->a1);
            num = op->i2;
        } else {
            bytes = arg_getBytes(op->a2);
            size = arg_getBytesSize(op->a2);
            num = op->i1;
        }
        op->res = arg_setBytes(op->res, "", NULL, size * num);
        uint8_t* bytes_out = arg_getBytes(op->res);
        for (int i = 0; i < num; i++) {
            pika_platform_memcpy(bytes_out + i * size, bytes, size);
        }
        return;
    }
    if (argType_isObject(op->t1) || argType_isObject(op->t2)) {
        Arg* __mul__ = NULL;
        PikaObj* obj = NULL;
        Arg* arg = NULL;
        if (argType_isObject(op->t1)) {
            __mul__ =
                obj_getMethodArgWithFullPath(arg_getPtr(op->a1), "__mul__");
            obj = arg_getPtr(op->a1);
            arg = op->a2;
        }
        if (NULL == __mul__) {
            if (argType_isObject(op->t2)) {
                __mul__ =
                    obj_getMethodArgWithFullPath(arg_getPtr(op->a2), "__mul__");
                obj = arg_getPtr(op->a2);
                arg = op->a1;
            }
        }
        if (NULL == __mul__) {
            PikaVMFrame_setErrorCode(op->vm, PIKA_RES_ERR_OPERATION_FAILED);
            PikaVMFrame_setSysOut(op->vm, "TypeError: unsupported operand *");
            op->res = NULL;
            return;
        }
        op->res = obj_runMethodArg1(obj, __mul__, arg_copy(arg));
        return;
    }
    op->res = arg_setInt(op->res, "", op->i1 * op->i2);
    return;
}

static Arg* VM_instruction_handler_OPT(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    OperatorInfo op = {0};
    op.num = PikaVMFrame_getInputArgNum(vm);
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
    int ret = operatorInfo_init(&op, self, vm, data, arg_ret_reg);
    if (0 != ret) {
        PikaVMFrame_setSysOut(vm, PIKA_ERR_STRING_SYNTAX_ERROR);
        PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_SYNTAX_ERROR);
        return NULL;
    }
    switch (data[0]) {
        case '+':
            _OPT_ADD(&op);
            goto __exit;
        case '%':
            if ((op.t1 == ARG_TYPE_INT) && (op.t2 == ARG_TYPE_INT)) {
                op.res = arg_setInt(op.res, "", op.i1 % op.i2);
                goto __exit;
            }
#if PIKA_MATH_ENABLE
            if (op.t1 == ARG_TYPE_FLOAT || op.t2 == ARG_TYPE_FLOAT) {
                op.res = arg_setFloat(op.res, "", fmod(op.f1, op.f2));
                goto __exit;
            }
#endif
            PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
            PikaVMFrame_setSysOut(
                vm, "TypeError: unsupported operand type(s) for %%: 'float'");
            op.res = NULL;
            goto __exit;
        case '-':
            _OPT_SUB(&op);
            goto __exit;
    }
    if (data[1] == '=' && (data[0] == '!' || data[0] == '=')) {
        _OPT_EQU(&op);
        goto __exit;
    }
    if (data[1] == 0) {
        switch (data[0]) {
            case '<':
                op.res = arg_setBool(op.res, "", op.f1 < op.f2);
                goto __exit;
            case '*':
                _OPT_MUL(&op);
                goto __exit;
            case '&':
                if ((op.t1 == ARG_TYPE_INT) && (op.t2 == ARG_TYPE_INT)) {
                    op.res = arg_setInt(op.res, "", op.i1 & op.i2);
                    goto __exit;
                }
                PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
                PikaVMFrame_setSysOut(vm,
                                      "TypeError: unsupported operand "
                                      "type(s) for &: 'float'");
                op.res = NULL;
                goto __exit;
            case '|':
                if ((op.t1 == ARG_TYPE_INT) && (op.t2 == ARG_TYPE_INT)) {
                    op.res = arg_setInt(op.res, "", op.i1 | op.i2);
                    goto __exit;
                }
                PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
                PikaVMFrame_setSysOut(vm,
                                      "TypeError: unsupported operand "
                                      "type(s) for |: 'float'");
                op.res = NULL;
                goto __exit;
            case '~':
                if (op.t2 == ARG_TYPE_INT) {
                    op.res = arg_setInt(op.res, "", ~op.i2);
                    goto __exit;
                }
                PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
                PikaVMFrame_setSysOut(vm,
                                      "TypeError: unsupported operand "
                                      "type(s) for ~: 'float'");
                op.res = NULL;
                goto __exit;
            case '/':
                if (0 == op.f2) {
                    PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
                    PikaVMFrame_setSysOut(
                        vm, "ZeroDivisionError: division by zero");
                    op.res = NULL;
                    goto __exit;
                }
                op.res = arg_setFloat(op.res, "", op.f1 / op.f2);
                goto __exit;
            case '>':
                op.res = arg_setInt(op.res, "", op.f1 > op.f2);
                goto __exit;
            case '^':
                if ((op.t1 == ARG_TYPE_INT) && (op.t2 == ARG_TYPE_INT)) {
                    op.res = arg_setInt(op.res, "", op.i1 ^ op.i2);
                    goto __exit;
                }
                PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
                PikaVMFrame_setSysOut(vm,
                                      "TypeError: unsupported operand "
                                      "type(s) for ^: 'float'");
                op.res = NULL;
                goto __exit;
        }
    }
    if (data[1] == 'i' && data[2] == 'n') {
        if (op.t1 == ARG_TYPE_STRING && op.t2 == ARG_TYPE_STRING) {
            if (strstr(arg_getStr(op.a2), arg_getStr(op.a1))) {
                op.res = arg_setBool(op.res, "", pika_true);
            } else {
                op.res = arg_setBool(op.res, "", pika_false);
            }
            goto __exit;
        }
        if (op.t1 == ARG_TYPE_BYTES) {
            op.res = arg_setBool(op.res, "", _bytes_contains(op.a1, op.a2));
            goto __exit;
        }
#if !PIKA_NANO_ENABLE
        if (argType_isObject(op.t2)) {
            PikaObj* obj2 = arg_getPtr(op.a2);
            Arg* __contains__ =
                obj_getMethodArgWithFullPath(obj2, "__contains__");
            if (NULL != __contains__) {
                arg_deinit(__contains__);
                op.res = _OPT_Method_ex(obj2, op.a1, &op, "__contains__",
                                        PIKA_RES_ERR_OPERATION_FAILED,
                                        "TypeError: unsupported operand in");
                goto __exit;
            }
            PikaObj* local = New_TinyObj(NULL);
            obj_setArg(local, "@list", op.a2);
            obj_setArg(local, "@val", op.a1);
            /* clang-format off */
            PIKA_PYTHON(
            @res_contains = 0
            for @item in @list:
                if @item == @val:
                    @res_contains = 1
                    break
            )
            /* clang-format on */
            const uint8_t bytes[] = {
                0x48, 0x00, 0x00, 0x00, /* instruct array size */
                0x00, 0x85, 0x01, 0x00, 0x00, 0x04, 0x03, 0x00, 0x10, 0x81,
                0x11, 0x00, 0x00, 0x02, 0x17, 0x00, 0x00, 0x04, 0x1c, 0x00,
                0x00, 0x82, 0x20, 0x00, 0x00, 0x04, 0x2d, 0x00, 0x00, 0x0d,
                0x2d, 0x00, 0x00, 0x07, 0x33, 0x00, 0x11, 0x81, 0x2d, 0x00,
                0x11, 0x01, 0x35, 0x00, 0x01, 0x08, 0x3a, 0x00, 0x01, 0x07,
                0x3d, 0x00, 0x02, 0x85, 0x3d, 0x00, 0x02, 0x04, 0x03, 0x00,
                0x02, 0x8e, 0x00, 0x00, 0x00, 0x86, 0x3f, 0x00, 0x00, 0x8c,
                0x1c, 0x00,
                /* instruct array */
                0x42, 0x00, 0x00, 0x00, /* const pool size */
                0x00, 0x30, 0x00, 0x40, 0x72, 0x65, 0x73, 0x5f, 0x63, 0x6f,
                0x6e, 0x74, 0x61, 0x69, 0x6e, 0x73, 0x00, 0x40, 0x6c, 0x69,
                0x73, 0x74, 0x00, 0x69, 0x74, 0x65, 0x72, 0x00, 0x24, 0x6c,
                0x30, 0x00, 0x24, 0x6c, 0x30, 0x2e, 0x5f, 0x5f, 0x6e, 0x65,
                0x78, 0x74, 0x5f, 0x5f, 0x00, 0x40, 0x69, 0x74, 0x65, 0x6d,
                0x00, 0x32, 0x00, 0x40, 0x76, 0x61, 0x6c, 0x00, 0x3d, 0x3d,
                0x00, 0x31, 0x00, 0x2d, 0x31, 0x00, /* const pool */
            };
            pikaVM_runByteCode(local, (uint8_t*)bytes);
            op.res =
                arg_setBool(op.res, "", obj_getInt(local, "@res_contains"));
            obj_deinit(local);
            goto __exit;
        }
#endif

        PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
        PikaVMFrame_setSysOut(vm,
                              "Operation 'in' is not supported for this "
                              "type");
        op.res = NULL;
        goto __exit;
    }
    if (data[0] == '*' && data[1] == '*') {
        _OPT_POW(&op);
        goto __exit;
    }
    if (data[0] == '/' && data[1] == '/') {
        if ((op.t1 == ARG_TYPE_INT) && (op.t2 == ARG_TYPE_INT)) {
            op.res = arg_setInt(op.res, "", op.i1 / op.i2);
            goto __exit;
        }
#if PIKA_MATH_ENABLE
        if ((op.t1 == ARG_TYPE_FLOAT) || (op.t2 == ARG_TYPE_FLOAT)) {
            op.res = arg_setFloat(op.res, "", floor(op.f1 / op.f2));
            goto __exit;
        }
#endif
        PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
        PikaVMFrame_setSysOut(
            vm,
            "Operation float \\\\ float is not enabled, please set "
            "PIKA_MATH_ENABLE");
        op.res = NULL;
        goto __exit;
    }
    if (data[1] == 'i' && data[2] == 's') {
#if !PIKA_NANO_ENABLE
        if (argType_isObject(op.t1) && argType_isObject(op.t2)) {
            if (arg_getPtr(op.a1) == arg_getPtr(op.a2)) {
                op.res = arg_setBool(op.res, "", pika_true);
            } else {
                op.res = arg_setBool(op.res, "", pika_false);
            }
            goto __exit;
        }
        if ((op.t1 != op.t2) && (op.t1 != ARG_TYPE_NONE) &&
            (op.t2 != ARG_TYPE_NONE)) {
            op.res = arg_setInt(op.res, "", 0);
            goto __exit;
        }
#endif
        op.opt = "==";
        _OPT_EQU(&op);
        goto __exit;
    }
    if (data[0] == '>' && data[1] == '=') {
        op.res = arg_setBool(
            op.res, "",
            (op.f1 > op.f2) ||
                ((op.f1 - op.f2) * (op.f1 - op.f2) < (pika_float)0.000001));
        goto __exit;
    }
    if (data[0] == '<' && data[1] == '=') {
        op.res = arg_setBool(
            op.res, "",
            (op.f1 < op.f2) ||
                ((op.f1 - op.f2) * (op.f1 - op.f2) < (pika_float)0.000001));
        goto __exit;
    }
    if (data[0] == '>' && data[1] == '>') {
        if ((op.t1 == ARG_TYPE_INT) && (op.t2 == ARG_TYPE_INT)) {
            op.res = arg_setInt(op.res, "", op.i1 >> op.i2);
            goto __exit;
        }
        PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
        PikaVMFrame_setSysOut(
            vm, "TypeError: unsupported operand type(s) for >>: 'float'");
        op.res = NULL;
        goto __exit;
    }
    if (data[0] == '<' && data[1] == '<') {
        if ((op.t1 == ARG_TYPE_INT) && (op.t2 == ARG_TYPE_INT)) {
            op.res = arg_setInt(op.res, "", op.i1 << op.i2);
            goto __exit;
        }
        PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
        PikaVMFrame_setSysOut(
            vm, "TypeError: unsupported operand type(s) for <<: 'float'");
        op.res = NULL;
        goto __exit;
    }
    if (data[0] == ' ' && data[1] == 'a' && data[2] == 'n' && data[3] == 'd' &&
        data[4] == ' ') {
        op.res = arg_setBool(op.res, "", op.i1 && op.i2);
        goto __exit;
    }
    if (data[0] == ' ' && data[1] == 'o' && data[2] == 'r' && data[3] == ' ' &&
        data[4] == 0) {
        op.res = arg_setBool(op.res, "", op.i1 || op.i2);
        goto __exit;
    }
    if (data[0] == ' ' && data[1] == 'n' && data[2] == 'o' && data[3] == 't' &&
        data[4] == ' ' && data[5] == 0) {
        pika_bool bTrue = pika_false;
        _transeBool(op.a2, &bTrue);
        op.res = arg_setBool(op.res, "", !bTrue);
        goto __exit;
    }
    PikaVMFrame_setSysOut(vm, "Error: unknown operator '%s'", data);
    PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
__exit:
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
                                         PikaVMFrame* vm,
                                         char* data,
                                         uint8_t is_class) {
    int thisBlockDeepth = PikaVMFrame_getBlockDeepthNow(vm);

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
            PikaVMFrame_getInstructUnitWithOffset(vm, offset);
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
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    return __VM_instruction_handler_DEF(self, vm, data, 0);
}

static Arg* VM_instruction_handler_CLS(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    return __VM_instruction_handler_DEF(self, vm, data, 1);
}

static Arg* VM_instruction_handler_RIS(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
#if PIKA_NANO_ENABLE
    return NULL;
#endif
    Arg* err_arg = stack_popArg_alloc(&(vm->stack));
    if (ARG_TYPE_INT == arg_getType(err_arg)) {
        PIKA_RES err = (PIKA_RES)arg_getInt(err_arg);
        if (PIKA_RES_ERR_RUNTIME_ERROR != err) {
            PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_INVALID_PARAM);
            PikaVMFrame_setSysOut(
                vm, "TypeError: exceptions must derive from BaseException");
            goto __exit;
        }
        PikaVMFrame_setErrorCode(vm, err);
    }
    if (arg_isConstructor(err_arg)) {
        MethodProp* method_prop = methodArg_getProp(err_arg);
        PikaVMFrame_setErrorCode(vm, (uintptr_t)method_prop->ptr);
        goto __exit;
    }
__exit:
    arg_deinit(err_arg);
    return NULL;
}

static Arg* VM_instruction_handler_ASS(PikaObj* self,
                                       PikaVMFrame* vm,
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
    uint32_t n_arg = PikaVMFrame_getInputArgNum(vm);
    if (n_arg == 1) {
        arg1 = stack_popArg(&vm->stack, &reg1);
    }
    if (n_arg == 2) {
        arg2 = stack_popArg(&vm->stack, &reg2);
        arg1 = stack_popArg(&vm->stack, &reg1);
    }
    /* assert failed */
    if ((arg_getType(arg1) == ARG_TYPE_INT && arg_getInt(arg1) == 0) ||
        (arg_getType(arg1) == ARG_TYPE_BOOL &&
         arg_getBool(arg1) == pika_false)) {
        stack_pushArg(&vm->stack, arg_newInt(PIKA_RES_ERR_ASSERT));
        res = VM_instruction_handler_RIS(self, vm, data, arg_ret_reg);
        // if (vm->vm_thread->try_state == TRY_STATE_NONE) {
        if (n_arg == 1) {
            pika_platform_printf("AssertionError\n");
        }
        if (n_arg == 2) {
            pika_platform_printf("AssertionError: %s\n", arg_getStr(arg2));
        }
        // }
        goto __exit;
    }
__exit:
    arg_deinit(arg1);
    if (NULL != arg2) {
        arg_deinit(arg2);
    }
    return res;
}

static Arg* VM_instruction_handler_NEL(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    int thisBlockDeepth = PikaVMFrame_getBlockDeepthNow(vm);
    if (0 == vm->ireg[thisBlockDeepth]) {
        /* set __else flag */
        vm->jmp = fast_atoi(data);
    }
    return NULL;
}

static Arg* VM_instruction_handler_DEL(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    if (_checkLReg(data)) {
        Locals_delLReg(vm->locals, data);
        goto __exit;
    }
    if (obj_isArgExist(vm->locals, data)) {
        obj_removeArg(vm->locals, data);
        goto __exit;
    }
    if (obj_isArgExist(vm->globals, data)) {
        obj_removeArg(vm->globals, data);
        goto __exit;
    }
    PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_OPERATION_FAILED);
    PikaVMFrame_setSysOut(vm, "NameError: name '%s' is not defined", data);
__exit:
    return NULL;
}

static Arg* VM_instruction_handler_EST(PikaObj* self,
                                       PikaVMFrame* vm,
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
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    /* break jmp signal */
    vm->jmp = VM_JMP_BREAK;
    return NULL;
}

static Arg* VM_instruction_handler_CTN(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    /* continue jmp signal */
    vm->jmp = VM_JMP_CONTINUE;
    return NULL;
}

static Arg* VM_instruction_handler_GLB(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    Arg* global_list_buff = NULL;
    char* global_list = args_getStr(vm->locals->list, "@g");
    /* create new global_list */
    if (NULL == global_list) {
        args_setStr(vm->locals->list, "@g", data);
        obj_setFlag(vm->locals, OBJ_FLAG_GLOBALS);
        goto __exit;
    }
    /* append to exist global_list */
    global_list_buff = arg_newStr(global_list);
    global_list_buff = arg_strAppend(global_list_buff, ",");
    global_list_buff = arg_strAppend(global_list_buff, data);
    args_setStr(vm->locals->list, "@g", arg_getStr(global_list_buff));
    goto __exit;
__exit:
    if (NULL != global_list_buff) {
        arg_deinit(global_list_buff);
    }
    return NULL;
}

static Arg* VM_instruction_handler_IMP(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    Args buffs = {0};
    char* sModuleNameRedirect = NULL;
    if (NULL == data) {
        goto __exit;
    }
    sModuleNameRedirect = LibObj_redirectModule(pika_getLibObj(), &buffs, data);
    if (NULL != sModuleNameRedirect) {
        data = sModuleNameRedirect;
    }
    /* the module is already imported, skip. */
    if (obj_isArgExist(self, data)) {
        goto __exit;
    }
    extern volatile PikaObj* __pikaMain;
    /* the module is already imported to root object, import it to self. */
    if (obj_isArgExist((PikaObj*)__pikaMain, data)) {
        obj_setArg(self, data, obj_getArg((PikaObj*)__pikaMain, data));
        goto __exit;
    }
    if (NULL == sModuleNameRedirect) {
        /* find cmodule in root object */
        char* cmodule_try = strsGetFirstToken(&buffs, data, '.');
        if (obj_isArgExist((PikaObj*)__pikaMain, cmodule_try)) {
            obj_setArg(self, cmodule_try,
                       obj_getArg((PikaObj*)__pikaMain, cmodule_try));
            goto __exit;
        }
    }

    /* import module from '@lib' */
    if (0 == obj_importModule(self, data)) {
        goto __exit;
    }
    PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_ARG_NO_FOUND);
    PikaVMFrame_setSysOut(vm, "ModuleNotFoundError: No module named '%s'",
                          data);
__exit:
    strsDeinit(&buffs);
    return NULL;
}

static Arg* VM_instruction_handler_INH(PikaObj* self,
                                       PikaVMFrame* vm,
                                       char* data,
                                       Arg* arg_ret_reg) {
    /* find module bytecode */
    uint8_t* bytecode = pika_getByteCodeFromModule(data);
    if (NULL == bytecode) {
        PikaVMFrame_setErrorCode(vm, PIKA_RES_ERR_ARG_NO_FOUND);
        PikaVMFrame_setSysOut(vm, "ModuleNotFoundError: No module named '%s'",
                              data);
        return NULL;
    }
    pikaVM_runByteCode(self, bytecode);
    return NULL;
}

#if PIKA_INSTRUCT_EXTENSION_ENABLE
const VMInstructionSet VM_default_instruction_set = {
#define __INS_OPCODE
    .instructions =
        (const VMInstruction[]){
#include "__instruction_table.h"
        },
    .count = __INSTRUCTION_CNT,
    .op_idx_start = PIKA_INS(NON),
    .op_idx_end = PIKA_INS(NON) + __INSTRUCTION_CNT - 1,
};

#ifndef PIKA_INSTRUCT_SIGNATURE_DICT
#define PIKA_INSTRUCT_SIGNATURE_DICT 0
#endif

typedef struct VMInstructionSetItem VMInstructionSetItem;
struct VMInstructionSetItem {
    VMInstructionSetItem* next;
    const VMInstructionSet* ins_set;
};

static struct {
    const VMInstructionSetItem default_ins_set;
    VMInstructionSetItem* list;
    VMInstructionSetItem* recent;
#if PIKA_INSTRUCT_SIGNATURE_DICT_COUNT > 0
    const uint16_t signature_dict[PIKA_INSTRUCT_SIGNATURE_DICT_COUNT];
#endif
} g_PikaVMInsSet = {
    .default_ins_set =
        {
            .ins_set = &VM_default_instruction_set,
            .next = NULL,
        },
    .list = (VMInstructionSetItem*)&g_PikaVMInsSet.default_ins_set,
    .recent = (VMInstructionSetItem*)&g_PikaVMInsSet.default_ins_set,
#if PIKA_INSTRUCT_SIGNATURE_DICT_COUNT > 0
    .signature_dict = {PIKA_INSTRUCT_SIGNATURE_DICT},
#endif
};

pika_bool pikaVM_registerInstructionSet(VMInstructionSet* ins_set) {
    pika_assert(NULL != ins_set);

#if PIKA_INSTRUCT_SIGNATURE_DICT_COUNT > 0
    uint16_t signature = ins_set->signature;

    pika_bool ins_set_valid = pika_false;
    for (int n = 0;
         n < sizeof(g_PikaVMInsSet.signature_dict) / sizeof(uint16_t); n++) {
        if (g_PikaVMInsSet.signature_dict[n] == signature) {
            ins_set_valid = pika_true;
            break;
        }
    }
    if (!ins_set_valid) {
        return pika_false;
    }
#endif

    /* check whether the target instruction set exists or not */
    VMInstructionSetItem* list_item = g_PikaVMInsSet.list;
    do {
        if (list_item->ins_set->signature == signature) {
            return pika_true; /* already exist */
        }

        list_item = list_item->next;
    } while (NULL != list_item->next);

    VMInstructionSetItem* item =
        pika_platform_malloc(sizeof(VMInstructionSetItem));
    if (NULL == item) {
        return pika_false;
    }
    item->ins_set = ins_set;
    item->next = NULL;

    /* add item to the tail of VM.list */
    list_item->next = item;

    return pika_true;
}

static const VMInstruction* instructUnit_getInstruct(
    enum InstructIndex ins_idx) {
    VMInstructionSetItem* item = g_PikaVMInsSet.recent;

    if ((ins_idx >= item->ins_set->op_idx_start) &&
        (ins_idx <= item->ins_set->op_idx_end)) {
        return &(
            item->ins_set->instructions[ins_idx - item->ins_set->op_idx_start]);
    }

    /* search list */
    item = g_PikaVMInsSet.list;
    do {
        if ((ins_idx >= item->ins_set->op_idx_start) &&
            (ins_idx <= item->ins_set->op_idx_end)) {
            g_PikaVMInsSet.recent = item;
            return &(item->ins_set
                         ->instructions[ins_idx - item->ins_set->op_idx_start]);
        }
        item = item->next;
    } while (NULL != item->next);

    return NULL;
}

static enum InstructIndex __find_ins_idx_in_ins_set(
    char* ins_str,
    const VMInstructionSet* set) {
    const VMInstruction* ins = set->instructions;
    uint_fast16_t count = set->count;

    do {
        if (0 == strncmp(ins_str, ins->op_str, ins->op_str_len)) {
            return (enum InstructIndex)ins->op_idx;
        }
        ins++;
    } while (--count);
    return __INSTRUCTION_UNKNOWN;
}

enum InstructIndex pikaVM_getInstructFromAsm(char* ins_str) {
    enum InstructIndex ins_idx =
        __find_ins_idx_in_ins_set(ins_str, g_PikaVMInsSet.recent->ins_set);

    if (__INSTRUCTION_UNKNOWN == ins_idx) {
        VMInstructionSetItem* item = g_PikaVMInsSet.list;

        do {
            ins_idx = __find_ins_idx_in_ins_set(ins_str, item->ins_set);
            if (__INSTRUCTION_UNKNOWN != ins_idx) {
                g_PikaVMInsSet.recent = item;
                return ins_idx;
            }
            item = item->next;
        } while (NULL != item->next);

        return PIKA_INS(NON);
    }

    return ins_idx;
}

#else

pika_bool pikaVM_registerInstructionSet(VMInstructionSet* ins_set) {
    return pika_false;
}

enum InstructIndex pikaVM_getInstructFromAsm(char* ins_str) {
#define __INS_COMPARE
#include "__instruction_table.h"
    return NON;
}

const VM_instruct_handler VM_instruct_handler_table[__INSTRUCTION_CNT] = {
#define __INS_TABLE
#include "__instruction_table.h"
};
#endif

extern volatile PikaObj* __pikaMain;
static enum shellCTRL __obj_shellLineHandler_debug(PikaObj* self,
                                                   char* input_line,
                                                   struct ShellConfig* config) {
    /* continue */
    if (strEqu("c", input_line)) {
        return SHELL_CTRL_EXIT;
    }
    /* next */
    if (strEqu("n", input_line)) {
        return SHELL_CTRL_EXIT;
    }
    /* launch shell */
    if (strEqu("sh", input_line)) {
        /* exit pika shell */
        pikaScriptShell((PikaObj*)__pikaMain);
        return SHELL_CTRL_CONTINUE;
    }
    /* quit */
    if (strEqu("q", input_line)) {
        obj_setInt(self, "enable", 0);
        return SHELL_CTRL_EXIT;
    }
    /* print */
    if (strIsStartWith(input_line, "p ")) {
        char* path = input_line + 2;
        Arg* asm_buff = arg_newStr("print(");
        asm_buff = arg_strAppend(asm_buff, path);
        asm_buff = arg_strAppend(asm_buff, ")\n");
        pikaVM_run_ex_cfg cfg = {0};
        cfg.globals = config->globals;
        cfg.in_repl = pika_true;
        pikaVM_run_ex(config->locals, arg_getStr(asm_buff), &cfg);
        arg_deinit(asm_buff);
        return SHELL_CTRL_CONTINUE;
    }
    pikaVM_run_ex_cfg cfg = {0};
    cfg.globals = config->globals;
    cfg.in_repl = pika_true;
    pikaVM_run_ex(config->locals, input_line, &cfg);
    return SHELL_CTRL_CONTINUE;
}

void pika_debug_set_trace(PikaObj* self) {
    if (!obj_getInt(self, "enable")) {
        return;
    }
    char* name = "stdin";
    pika_assert(NULL != self->vmFrame);
    if (NULL != self->vmFrame->bytecode_frame->name) {
        name = self->vmFrame->bytecode_frame->name;
    }
    pika_platform_printf("%s:%d\n", name, self->vmFrame->pc);
    struct ShellConfig cfg = {
        .prefix = "(Pdb-pika) ",
        .handler = __obj_shellLineHandler_debug,
        .fn_getchar = __platform_getchar,
        .locals = self->vmFrame->locals,
        .globals = self->vmFrame->globals,
    };
    _do_pikaScriptShell(self, &cfg);
    shConfig_deinit(&cfg);
}

static int pikaVM_runInstructUnit(PikaObj* self,
                                  PikaVMFrame* vm,
                                  InstructUnit* ins_unit) {
    enum InstructIndex instruct = instructUnit_getInstructIndex(ins_unit);
    arg_newReg(ret_reg, PIKA_ARG_BUFF_SIZE);
    Arg* return_arg = &ret_reg;
    // char invode_deepth1_str[2] = {0};
    int32_t pc_next = vm->pc + instructUnit_getSize();
    char* data = PikaVMFrame_getConstWithInstructUnit(vm, ins_unit);
    /* run instruct */
    pika_assert(NULL != vm->vm_thread);
    if (PikaVMFrame_checkBreakPoint(vm)) {
        pika_debug_set_trace(self);
    }

#if PIKA_INSTRUCT_EXTENSION_ENABLE
    const VMInstruction* ins = instructUnit_getInstruct(instruct);
    if (NULL == ins) {
        /* todo: unsupported instruction */
        pika_assert(NULL != ins);
    }
    pika_assert(NULL != ins->handler);

    return_arg = ins->handler(self, vm, data, &ret_reg);
#else
    return_arg = VM_instruct_handler_table[instruct](self, vm, data, &ret_reg);
#endif

    if (pikaVMFrame_checkErrorStack(vm) != PIKA_RES_OK ||
        VMSignal_getCtrl() == VM_SIGNAL_CTRL_EXIT) {
        /* raise jmp */
        if (vm->vm_thread->try_state == TRY_STATE_INNER) {
            vm->jmp = VM_JMP_RAISE;
        } else {
            /* exit */
            vm->jmp = VM_JMP_EXIT;
        }
    }

#if PIKA_BUILTIN_STRUCT_ENABLE
    int invoke_deepth = PikaVMFrame_getInvokeDeepthNow(vm);
    if (invoke_deepth > 0) {
        PikaObj* oReg = vm->oreg[invoke_deepth - 1];
        if (NULL != oReg && NULL != return_arg) {
            pikaList_append(oReg, return_arg);
            return_arg = NULL;
        }
    }
#endif

    if (NULL != return_arg) {
        stack_pushArg(&(vm->stack), return_arg);
    }
    goto __next_line;
__next_line:
    /* exit */
    if (VM_JMP_EXIT == vm->jmp) {
        pc_next = VM_PC_EXIT;
        goto __exit;
    }
    /* break */
    if (VM_JMP_BREAK == vm->jmp) {
        pc_next = vm->pc + PikaVMFrame_getAddrOffsetOfBreak(vm);
        goto __exit;
    }
    /* continue */
    if (VM_JMP_CONTINUE == vm->jmp) {
        pc_next = vm->pc + PikaVMFrame_getAddrOffsetOfContinue(vm);
        goto __exit;
    }
/* raise */
#if !PIKA_NANO_ENABLE
    if (VM_JMP_RAISE == vm->jmp) {
        int offset = PikaVMFrame_getAddrOffsetOfRaise(vm);
        if (0 == offset) {
            /* can not found end of try, return */
            pc_next = VM_PC_EXIT;
            vm->vm_thread->try_result = TRY_RESULT_RAISE;
            goto __exit;
        }
        pc_next = vm->pc + offset;
        vm->vm_thread->try_result = TRY_RESULT_NONE;
        goto __exit;
    }
#endif
    /* static jmp */
    if (vm->jmp != 0) {
        pc_next = vm->pc + PikaVMFrame_getAddrOffsetFromJmp(vm);
        goto __exit;
    }
    /* not jmp */
    pc_next = vm->pc + instructUnit_getSize();

    /* jump to next line */
    if (pikaVMFrame_checkErrorStack(vm) != PIKA_RES_OK) {
        while (1) {
            if (pc_next >= (int)PikaVMFrame_getInstructArraySize(vm)) {
                pc_next = VM_PC_EXIT;
                goto __exit;
            }
            InstructUnit* ins_next = instructArray_getByOffset(
                &vm->bytecode_frame->instruct_array, pc_next);
            if (instructUnit_getIsNewLine(ins_next)) {
                goto __exit;
            }
            pc_next = pc_next + instructUnit_getSize();
        }
    }

    goto __exit;
__exit:
    vm->jmp = 0;
    /* reach the end */
    if (pc_next >= (int)PikaVMFrame_getInstructArraySize(vm)) {
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
                                      enum InstructIndex ins1,
                                      enum InstructIndex ins2) {
    ByteCodeFrame bf = {0};
    char* res = NULL;
    byteCodeFrame_init(&bf);
    byteCodeFrame_loadByteCode(&bf, bytecode);
    while (1) {
        InstructUnit* ins_unit = instructArray_getNow(&bf.instruct_array);
        if (NULL == ins_unit) {
            goto __exit;
        }
        enum InstructIndex ins = instructUnit_getInstructIndex(ins_unit);
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
    if (NULL ==
        _get_data_from_bytecode2(bytecode, PIKA_INS(DEF), PIKA_INS(CLS))) {
        return NULL;
    }
    return _cache_bytecodeframe(self);
}

VMParameters* pikaVM_run_ex(PikaObj* self,
                            char* py_lines,
                            pikaVM_run_ex_cfg* cfg) {
    ByteCodeFrame bytecode_frame_stack = {0};
    ByteCodeFrame* bytecode_frame_p = NULL;
    uint8_t is_use_heap_bytecode = 0;
    PikaObj* globals = self;
    if (NULL != cfg->globals) {
        globals = cfg->globals;
    }
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
    if (PIKA_RES_OK != pika_lines2Bytes(bytecode_frame_p, py_lines)) {
        obj_setSysOut(self, PIKA_ERR_STRING_SYNTAX_ERROR);
        globals = NULL;
        goto __exit;
    }
    /* run byteCode */
    if (NULL != cfg->module_name) {
        byteCodeFrame_setName(bytecode_frame_p, cfg->module_name);
    }
    globals = _pikaVM_runByteCodeFrameGlobals(self, globals, bytecode_frame_p,
                                              cfg->in_repl);
    goto __exit;
__exit:
    if (!is_use_heap_bytecode) {
        byteCodeFrame_deinit(&bytecode_frame_stack);
    }
    return globals;
}

VMParameters* pikaVM_runByteCode_ex(PikaObj* self,
                                    uint8_t* bytecode,
                                    pikaVM_runBytecode_ex_cfg* cfg) {
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
        cfg->is_const_bytecode = pika_true;
    }

    /* load or generate byte code frame */
    /* load bytecode */
    _do_byteCodeFrame_loadByteCode(bytecode_frame_p, bytecode, cfg->name,
                                   cfg->is_const_bytecode);

    /* run byteCode */

    cfg->globals = _pikaVM_runByteCodeFrameWithState(
        self, cfg->locals, cfg->globals, bytecode_frame_p, 0, cfg->vm_thread);
    goto __exit;
__exit:
    if (!is_use_heap_bytecode) {
        byteCodeFrame_deinit(&bytecode_frame_stack);
    }
    return cfg->globals;
}

VMParameters* pikaVM_runByteCodeFile(PikaObj* self, char* filename) {
    Args buffs = {0};
    Arg* file_arg = arg_loadFile(NULL, filename);
    pika_assert(NULL != file_arg);
    if (NULL == file_arg) {
        pika_platform_printf("Error: can not open file '%s'\n", filename);
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
    if (NULL == file_arg) {
        pika_platform_printf("FileNotFoundError: %s\n", filename);
        return NULL;
    }
    char* lines = (char*)arg_getBytes(file_arg);
    lines = strsFilePreProcess(&buffs, lines);
    /* clear the void line */
    pikaVM_run_ex_cfg cfg = {0};
    cfg.in_repl = pika_false;
    char* module_name = strsPathGetFileName(&buffs, filename);
    module_name = strsPopToken(&buffs, &module_name, '.');
    cfg.module_name = module_name;
    VMParameters* res = pikaVM_run_ex(self, lines, &cfg);
    arg_deinit(file_arg);
    strsDeinit(&buffs);
    return res;
}

VMParameters* pikaVM_run(PikaObj* self, char* py_lines) {
    pikaVM_run_ex_cfg cfg = {0};
    cfg.in_repl = pika_false;
    return pikaVM_run_ex(self, py_lines, &cfg);
}

VMParameters* pikaVM_runByteCode(PikaObj* self, const uint8_t* bytecode) {
    pika_assert(NULL != self);
    PikaVMThread* vm_thread = pikaVMThread_require();

    pikaVM_runBytecode_ex_cfg cfg = {0};
    cfg.locals = self;
    cfg.globals = self;
    cfg.name = NULL;
    cfg.vm_thread = vm_thread;
    cfg.is_const_bytecode = pika_true;
    return pikaVM_runByteCode_ex(self, (uint8_t*)bytecode, &cfg);
}

Arg* pikaVM_runByteCodeReturn(PikaObj* self,
                              const uint8_t* bytecode,
                              char* returnName) {
    pikaVM_runByteCode(self, bytecode);
    Arg* ret = args_getArg(self->list, returnName);
    if (NULL == ret) {
        return NULL;
    }
    ret = arg_copy(ret);
    /* set gc root to avoid be free */
    arg_setObjFlag(ret, OBJ_FLAG_GC_ROOT);
    obj_removeArg(self, returnName);
    return ret;
}

Arg* pikaVM_runByteCode_exReturn(PikaObj* self,
                                 VMParameters* locals,
                                 VMParameters* globals,
                                 uint8_t* bytecode,
                                 PikaVMThread* vm_thread,
                                 pika_bool is_const_bytecode,
                                 char* return_name) {
    pikaVM_runBytecode_ex_cfg cfg = {0};
    cfg.locals = locals;
    cfg.globals = globals;
    cfg.vm_thread = vm_thread;
    cfg.is_const_bytecode = is_const_bytecode;
    pikaVM_runByteCode_ex(self, bytecode, &cfg);
    Arg* ret = args_getArg(self->list, return_name);
    if (NULL == ret) {
        return NULL;
    }
    ret = arg_copy(ret);
    /* set gc root to avoid be free */
    arg_setObjFlag(ret, OBJ_FLAG_GC_ROOT);
    obj_removeArg(self, return_name);
    return ret;
}

VMParameters* pikaVM_runByteCodeInconstant(PikaObj* self, uint8_t* bytecode) {
    PikaVMThread* vm_thread = pikaVMThread_require();

    pikaVM_runBytecode_ex_cfg cfg = {0};
    cfg.locals = self;
    cfg.globals = self;
    cfg.vm_thread = vm_thread;
    cfg.is_const_bytecode = pika_false;
    return pikaVM_runByteCode_ex(self, (uint8_t*)bytecode, &cfg);
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
            goto __exit;
        }
        uint16_t offset = self->content_offset_now;
        pika_platform_printf("%d: %s\r\n", offset, constPool_getNow(self));
    }
__exit:
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
    self->name = NULL;
    self->label_pc = -1;
}

extern const char magic_code_pyo[4];
void _do_byteCodeFrame_loadByteCode(ByteCodeFrame* self,
                                    uint8_t* bytes,
                                    char* name,
                                    pika_bool is_const) {
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
    byteCodeFrame_setName(self, name);
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
    pika_assert(NULL != self->const_pool.content_start);
}

void byteCodeFrame_setName(ByteCodeFrame* self, char* name) {
    if (name != NULL && self->name == NULL) {
        self->name = pika_platform_malloc(strGetSize(name) + 1);
        pika_platform_memcpy(self->name, name, strGetSize(name) + 1);
    }
}

void byteCodeFrame_loadByteCode(ByteCodeFrame* self, uint8_t* bytes) {
    _do_byteCodeFrame_loadByteCode(self, bytes, NULL, pika_true);
}

void byteCodeFrame_deinit(ByteCodeFrame* self) {
    constPool_deinit(&(self->const_pool));
    instructArray_deinit(&(self->instruct_array));
    if (NULL != self->name) {
        pika_platform_free(self->name);
    }
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
        self->arg_buff = arg_newNone();
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

#if PIKA_INSTRUCT_EXTENSION_ENABLE

static const char* __find_ins_str_in_ins_set(enum InstructIndex op_idx,
                                             const VMInstructionSet* set) {
    const VMInstruction* ins = set->instructions;
    uint_fast16_t count = set->count;

    do {
        if (ins->op_idx == op_idx) {
            return ins->op_str;
        }
        ins++;
    } while (--count);
    return NULL;
}

static char* instructUnit_getInstructStr(InstructUnit* self) {
    enum InstructIndex op_idx = instructUnit_getInstructIndex(self);

    const char* ins_str =
        __find_ins_str_in_ins_set(op_idx, g_PikaVMInsSet.recent->ins_set);
    if (NULL != ins_str) {
        return (char*)ins_str;
    }
    VMInstructionSetItem* item = g_PikaVMInsSet.list;
    do {
        ins_str = __find_ins_str_in_ins_set(op_idx, item->ins_set);
        if (NULL != ins_str) {
            g_PikaVMInsSet.recent = item;
            return (char*)ins_str;
        }
        item = item->next;
    } while (NULL != item->next);
    return "NON";
}
#else
static char* instructUnit_getInstructStr(InstructUnit* self) {
#define __INS_GET_INS_STR
#include "__instruction_table.h"
    return "NON";
}
#endif

void instructUnit_print(InstructUnit* self) {
    if (instructUnit_getIsNewLine(self)) {
        pika_platform_printf("B%d\r\n", instructUnit_getBlockDeepth(self));
    }
    pika_platform_printf("%d %s #%d\r\n", instructUnit_getInvokeDeepth(self),
                         instructUnit_getInstructStr(self),
                         self->const_pool_index);
}

static void instructUnit_printWithConst(InstructUnit* self,
                                        ConstPool* const_pool) {
    // if (instructUnit_getIsNewLine(self)) {
    //     pika_platform_printf("B%d\r\n",
    //     instructUnit_getBlockDeepth(self));
    // }
    pika_platform_printf(
        "%s %s \t\t(#%d)\r\n", instructUnit_getInstructStr(self),
        constPool_getByOffset(const_pool, self->const_pool_index),
        self->const_pool_index);
}

void instructArray_printWithConst(InstructArray* self, ConstPool* const_pool) {
    uint16_t offset_befor = self->content_offset_now;
    self->content_offset_now = 0;
    while (1) {
        InstructUnit* ins_unit = instructArray_getNow(self);
        if (NULL == ins_unit) {
            goto __exit;
        }
        instructUnit_printWithConst(ins_unit, const_pool);
        instructArray_getNext(self);
    }
__exit:
    self->content_offset_now = offset_befor;
    return;
}

void instructArray_print(InstructArray* self) {
    uint16_t offset_befor = self->content_offset_now;
    self->content_offset_now = 0;
    while (1) {
        InstructUnit* ins_unit = instructArray_getNow(self);
        if (NULL == ins_unit) {
            goto __exit;
        }
        instructUnit_print(ins_unit);
        instructArray_getNext(self);
    }
__exit:
    self->content_offset_now = offset_befor;
    return;
}

void instructArray_printAsArray(InstructArray* self) {
    uint16_t offset_befor = self->content_offset_now;
    self->content_offset_now = 0;
    uint8_t line_num = 12;
    uint16_t g_i = 0;
    uint8_t* ins_size_p = (uint8_t*)&self->size;
    pika_platform_printf("0x%02x, ", *(ins_size_p));
    pika_platform_printf("0x%02x, ", *(ins_size_p + (uintptr_t)1));
    pika_platform_printf("0x%02x, ", *(ins_size_p + (uintptr_t)2));
    pika_platform_printf("0x%02x, ", *(ins_size_p + (uintptr_t)3));
    pika_platform_printf("/* instruct array size */\n");
    while (1) {
        InstructUnit* ins_unit = instructArray_getNow(self);
        if (NULL == ins_unit) {
            goto __exit;
        }
        for (int i = 0; i < (int)instructUnit_getSize(); i++) {
            g_i++;
            pika_platform_printf("0x%02x, ",
                                 *((uint8_t*)ins_unit + (uintptr_t)i));
            if (g_i % line_num == 0) {
                pika_platform_printf("\n");
            }
        }
        instructArray_getNext(self);
    }
__exit:
    pika_platform_printf("/* instruct array */\n");
    self->content_offset_now = offset_befor;
    return;
}

size_t byteCodeFrame_getSize(ByteCodeFrame* bf) {
    return bf->const_pool.size + bf->instruct_array.size;
}

void byteCodeFrame_print(ByteCodeFrame* self) {
    constPool_print(&(self->const_pool));
    instructArray_printWithConst(&(self->instruct_array), &(self->const_pool));
    pika_platform_printf("---------------\r\n");
    pika_platform_printf("byte code size: %d\r\n",
                         self->const_pool.size + self->instruct_array.size);
}

PIKA_WEAK void pika_hook_unused_stack_arg(PikaVMFrame* vm, Arg* arg) {
    if (vm->in_repl) {
        arg_print(arg, pika_true, "\r\n");
    }
}

void PikaVMFrame_solveUnusedStack(PikaVMFrame* vm) {
    uint8_t top = stack_getTop(&(vm->stack));
    for (int i = 0; i < top; i++) {
        Arg* arg = stack_popArg_alloc(&(vm->stack));
        ArgType type = arg_getType(arg);
        if (type == ARG_TYPE_NONE) {
            arg_deinit(arg);
            continue;
        }
        if (vm->error.line_code != 0) {
            arg_deinit(arg);
            continue;
        }
        pika_hook_unused_stack_arg(vm, arg);
        arg_deinit(arg);
    }
}

PikaVMFrame* PikaVMFrame_create(VMParameters* locals,
                                VMParameters* globals,
                                ByteCodeFrame* bytecode_frame,
                                int32_t pc,
                                PikaVMThread* vm_thread) {
    PikaVMFrame* vm = (PikaVMFrame*)pikaMalloc(sizeof(PikaVMFrame));
    vm->bytecode_frame = bytecode_frame;
    vm->locals = locals;
    vm->globals = globals;
    vm->pc = pc;
    vm->vm_thread = vm_thread;
    vm->jmp = 0;
    vm->loop_deepth = 0;
    pika_platform_memset(&vm->error, 0, sizeof(PikaVMError));
    vm->ins_cnt = 0;
    vm->in_super = pika_false;
    vm->super_invoke_deepth = 0;
    vm->in_repl = pika_false;
    stack_init(&(vm->stack));
    PikaVMFrame_initReg(vm);
    return vm;
}

int PikaVMFrame_destroy(PikaVMFrame* vm) {
    // PikaVMError* err = pikaVMThread_popError(vm->vm_thread);
    // if (NULL != err) {
    //     pikaFree(err, sizeof(PikaVMError));
    // }
    stack_deinit(&(vm->stack));
    pikaFree(vm, sizeof(PikaVMFrame));
    return 0;
}

int pikaVMFrame_checkErrorCode(PikaVMFrame* state) {
    pika_assert(NULL != state);
    return state->error.code;
}

int pikaVMFrame_checkErrorStack(PikaVMFrame* vm) {
    pika_assert(NULL != vm);
    PIKA_RES code = pikaVMFrame_checkErrorCode(vm);
    if (code != PIKA_RES_OK) {
        return code;
    }
    if (NULL == vm->vm_thread->error_stack) {
        return 0;
    }

    for (PikaVMError* current = vm->vm_thread->error_stack; current != NULL;
         current = current->next) {
        if (0 != current->code) {
            return current->code;
        }
    }
    return 0;
}

int pikaVMFrame_checkExceptionStack(PikaVMFrame* vm) {
    pika_assert(NULL != vm);
    if (NULL == vm->vm_thread->exception_stack) {
        return 0;
    }
    for (PikaVMError* current = vm->vm_thread->exception_stack; current != NULL;
         current = current->next) {
        if (0 != current->code) {
            return current->code;
        }
    }
    return 0;
}

int pikaVMThread_pushError(PikaVMThread* vmThread, PikaVMError* error) {
    pika_assert(NULL != vmThread);
    pika_assert(NULL != error);
    for (PikaVMError* current = vmThread->error_stack; current != NULL;
         current = current->next) {
        if (current == error) {
            return 0;
        }
    }

    PikaVMError* error_new = (PikaVMError*)pikaMalloc(sizeof(PikaVMError));
    if (error_new == NULL) {
        return -1;
    }
    pika_platform_memcpy(error_new, error, sizeof(PikaVMError));
    error_new->next = vmThread->error_stack;
    vmThread->error_stack = error_new;
    vmThread->error_stack_deepth++;
    if (vmThread->error_stack_deepth > vmThread->error_stack_deepth_max) {
        vmThread->error_stack_deepth_max = vmThread->error_stack_deepth;
    }
    return 0;
}

PikaVMError* pikaVMThread_popError(PikaVMThread* vmThread) {
    pika_assert(NULL != vmThread);
    PikaVMError* error = vmThread->error_stack;
    if (error != NULL) {
        vmThread->error_stack = error->next;
        vmThread->error_stack_deepth--;
    }
    return error;
}

PikaVMError* pikaVMThread_getErrorCurrent(PikaVMThread* vmThread) {
    pika_assert(NULL != vmThread);
    return vmThread->error_stack;
}

int pikaVMThread_clearErrorStack(PikaVMThread* vmThread) {
    pika_assert(NULL != vmThread);
    PikaVMError* current = vmThread->error_stack;
    while (current != NULL) {
        PikaVMError* next = current->next;
        pikaFree(current, sizeof(PikaVMError));
        vmThread->error_stack_deepth--;
        current = next;
    }
    pika_assert(vmThread->error_stack_deepth == 0);
    vmThread->error_stack = NULL;
    return 0;
}

int pikaVMThread_clearExceptionStack(PikaVMThread* vmThread) {
    pika_assert(NULL != vmThread);
    PikaVMError* current = vmThread->exception_stack;
    while (current != NULL) {
        PikaVMError* next = current->next;
        pikaFree(current, sizeof(PikaVMError));
        current = next;
    }
    vmThread->exception_stack = NULL;
    return 0;
}

int pikaVMThread_convertExceptionStack(PikaVMThread* vmThread) {
    // Convert error stack to exception stack
    pika_assert(NULL != vmThread);
    if (NULL != vmThread->error_stack) {
        vmThread->exception_stack = vmThread->error_stack;
    }
    vmThread->error_stack_deepth = 0;
    vmThread->error_stack = NULL;
    return 0;
}

static VMParameters* __pikaVM_runByteCodeFrameWithState(
    PikaObj* self,
    VMParameters* locals,
    VMParameters* globals,
    ByteCodeFrame* bytecode_frame,
    uint16_t pc,
    PikaVMThread* vm_thread,
    pika_bool in_repl) {
    pika_assert(NULL != vm_thread);
    int size = bytecode_frame->instruct_array.size;
    /* locals is the local scope */

    if (g_PikaVMState.vm_cnt == 0) {
        pika_vmSignal_setCtrlClear();
    }
    PikaVMFrame* vm =
        PikaVMFrame_create(locals, globals, bytecode_frame, pc, vm_thread);
    vm->in_repl = in_repl;
    vm_thread->invoke_deepth++;
    g_PikaVMState.vm_cnt++;
    while (vm->pc < size) {
        if (vm->pc == VM_PC_EXIT) {
            break;
        }
        InstructUnit* this_ins_unit = PikaVMFrame_getInstructNow(vm);
        uint8_t is_new_line = instructUnit_getIsNewLine(this_ins_unit);
        if (is_new_line) {
            PikaVMFrame_solveUnusedStack(vm);
            stack_reset(&(vm->stack));
            vm->error.code = 0;
            vm->error.line_code = 0;
        }
        self->vmFrame = vm;
        vm->pc = pikaVM_runInstructUnit(self, vm, this_ins_unit);
        vm->ins_cnt++;
#if PIKA_INSTRUCT_HOOK_ENABLE
        if (vm->ins_cnt % PIKA_INSTRUCT_HOOK_PERIOD == 0) {
            pika_hook_instruct();
        }
#endif
        if (vm->ins_cnt % PIKA_INSTRUCT_YIELD_PERIOD == 0) {
            _pikaVM_yield();
        }
        // push vm frame error to thread error stack
        if (pikaVMFrame_checkErrorCode(vm) != PIKA_RES_OK) {
            pikaVMThread_pushError(vm->vm_thread, &(vm->error));
        }
        // handle error
        if (pikaVMFrame_checkErrorStack(vm) != PIKA_RES_OK) {
            vm->error.line_code = vm->error.code;
            InstructUnit* head_ins_unit = this_ins_unit;
            /* get first ins of a line */
            while (1) {
                if (instructUnit_getIsNewLine(head_ins_unit)) {
                    break;
                }
                head_ins_unit--;
            }
            if (vm->vm_thread->try_state == TRY_STATE_NONE) {
                while (1) {
                    if (head_ins_unit != this_ins_unit) {
                        pika_platform_printf("   ");
                    } else {
                        pika_platform_printf(" -> ");
                    }
                    instructUnit_printWithConst(head_ins_unit,
                                                &(bytecode_frame->const_pool));
                    head_ins_unit++;
                    if (head_ins_unit > this_ins_unit) {
                        break;
                    }
                }
            }
            pika_platform_error_handle();
        }
    }
    PikaVMFrame_solveUnusedStack(vm);
    PikaVMFrame_destroy(vm);
    self->vmFrame = NULL;
    VMParameters* result = locals;
    pika_assert(vm_thread->invoke_deepth > 0);
    vm_thread->invoke_deepth--;
    uint8_t vm_thread_deepth = vm_thread->invoke_deepth;
    uint8_t vm_thread_is_sub_thread = vm_thread->is_sub_thread;
    if (vm_thread_deepth == 0) {
        pikaVMThread_delete();
    }
    g_PikaVMState.vm_cnt--;
    if ((vm_thread_deepth == 0) && (!vm_thread_is_sub_thread)) {
        if (VMSignal_getCtrl() == VM_SIGNAL_CTRL_EXIT) {
            // wait other sub thread to exit
            while (_VMEvent_getVMCnt() > 0) {
                pika_GIL_EXIT();
                pika_platform_thread_yield();
                pika_GIL_ENTER();
            }
        }
    }
    return result;
}

pika_bool pika_debug_check_break(char* module_name, int pc_break) {
    Hash h = hash_time33(module_name);
    for (int i = 0; i < g_PikaVMState.break_point_cnt; i++) {
        if (g_PikaVMState.break_module_hash[i] == h &&
            g_PikaVMState.break_point_pc[i] == pc_break) {
            return pika_true;
        }
    }
    return pika_false;
}

pika_bool pika_debug_check_break_hash(Hash module_hash, int pc_break) {
    for (int i = 0; i < g_PikaVMState.break_point_cnt; i++) {
        if (g_PikaVMState.break_module_hash[i] == module_hash &&
            g_PikaVMState.break_point_pc[i] == pc_break) {
            return pika_true;
        }
    }
    return pika_false;
}

PIKA_RES pika_debug_set_break(char* module_name, int pc_break) {
    if (pika_debug_check_break(module_name, pc_break)) {
        return PIKA_RES_OK;
    }
    if (g_PikaVMState.break_point_cnt >= PIKA_DEBUG_BREAK_POINT_MAX) {
        return PIKA_RES_ERR_RUNTIME_ERROR;
    }
    Hash h = hash_time33(module_name);
    g_PikaVMState.break_module_hash[g_PikaVMState.break_point_cnt] = h;
    g_PikaVMState.break_point_pc[g_PikaVMState.break_point_cnt] = pc_break;
    g_PikaVMState.break_point_cnt++;
    return PIKA_RES_OK;
}

PIKA_RES pika_debug_reset_break(char* module_name, int pc_break) {
    if (!pika_debug_check_break(module_name, pc_break)) {
        return PIKA_RES_OK;
    }
    Hash h = hash_time33(module_name);
    for (int i = 0; i < g_PikaVMState.break_point_cnt; i++) {
        if (g_PikaVMState.break_module_hash[i] == h &&
            g_PikaVMState.break_point_pc[i] == pc_break) {
            // Move subsequent break points one position forward
            for (int j = i; j < g_PikaVMState.break_point_cnt - 1; j++) {
                g_PikaVMState.break_module_hash[j] =
                    g_PikaVMState.break_module_hash[j + 1];
                g_PikaVMState.break_point_pc[j] =
                    g_PikaVMState.break_point_pc[j + 1];
            }
            // Decrease the count of break points
            g_PikaVMState.break_point_cnt--;
            return PIKA_RES_OK;
        }
    }
    return PIKA_RES_ERR_RUNTIME_ERROR;
}

static VMParameters* _pikaVM_runByteCodeFrameWithState(
    PikaObj* self,
    VMParameters* locals,
    VMParameters* globals,
    ByteCodeFrame* bytecode_frame,
    uint16_t pc,
    PikaVMThread* vm_thread) {
    return __pikaVM_runByteCodeFrameWithState(
        self, locals, globals, bytecode_frame, pc, vm_thread, pika_false);
}

static PikaVMThread* g_pika_vm_state_head = NULL;

int pikaVMThread_init(PikaVMThread* vmThread, uint64_t thread_id) {
    vmThread->thread_id = thread_id;
    vmThread->invoke_deepth = 0;
    vmThread->error_stack = NULL;
    vmThread->exception_stack = NULL;
    vmThread->error_stack_deepth = 0;
    vmThread->error_stack_deepth_max = 0;
    pika_platform_memset(&vmThread->try_state, 0, sizeof(TRY_STATE));
    pika_platform_memset(&vmThread->try_result, 0, sizeof(TRY_RESULT));
    vmThread->next = NULL;
    vmThread->is_sub_thread = 0;
    return 0;
}

PikaVMThread* pikaVMThread_create(uint64_t thread_id) {
    PikaVMThread* vm_state = (PikaVMThread*)pikaMalloc(sizeof(PikaVMThread));
    if (vm_state == NULL) {
        return NULL;
    }
    pikaVMThread_init(vm_state, thread_id);
    return vm_state;
}

void pikaVMThread_destroy(PikaVMThread* state) {
    pikaVMThread_clearErrorStack(state);
    pikaVMThread_clearExceptionStack(state);
    if (state != NULL) {
        pikaFree(state, sizeof(PikaVMThread));
    }
}

PikaVMThread* pikaVMThread_find_node_by_thread_id(uint64_t thread_id) {
    PikaVMThread* current = g_pika_vm_state_head;
    while (current != NULL) {
        if (current->thread_id == thread_id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

PikaVMThread* pikaVMThread_require(void) {
    uint64_t current_thread_id = pika_platform_thread_self();
    PikaVMThread* state =
        pikaVMThread_find_node_by_thread_id(current_thread_id);
    if (state != NULL) {
        return state;
    }

    PikaVMThread* new_state = pikaVMThread_create(current_thread_id);
    if (new_state == NULL) {
        return NULL;
    }

    new_state->next = g_pika_vm_state_head;
    g_pika_vm_state_head = new_state;

    return new_state;
}

void pikaVMThread_delete(void) {
    uint64_t current_thread_id = pika_platform_thread_self();
    PikaVMThread* current = g_pika_vm_state_head;
    PikaVMThread* previous = NULL;

    while (current != NULL) {
        if (current->thread_id == current_thread_id) {
            if (previous == NULL) {
                g_pika_vm_state_head = current->next;
            } else {
                previous->next = current->next;
            }
            pikaVMThread_destroy(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

VMParameters* _pikaVM_runByteCodeFrame(PikaObj* self,
                                       ByteCodeFrame* byteCode_frame,
                                       pika_bool in_repl) {
    PikaVMThread* vm_thread = pikaVMThread_require();

    return __pikaVM_runByteCodeFrameWithState(self, self, self, byteCode_frame,
                                              0, vm_thread, in_repl);
}

VMParameters* _pikaVM_runByteCodeFrameGlobals(PikaObj* self,
                                              PikaObj* globals,
                                              ByteCodeFrame* byteCode_frame,
                                              pika_bool in_repl) {
    PikaVMThread* vm_thread = pikaVMThread_require();

    return __pikaVM_runByteCodeFrameWithState(
        self, self, globals, byteCode_frame, 0, vm_thread, in_repl);
}

VMParameters* pikaVM_runByteCodeFrame(PikaObj* self,
                                      ByteCodeFrame* byteCode_frame) {
    return _pikaVM_runByteCodeFrame(self, byteCode_frame, pika_false);
}

void constPool_printAsArray(ConstPool* self) {
    uint8_t* const_size_str = (uint8_t*)&(self->size);
    pika_platform_printf("0x%02x, ", *(const_size_str));
    pika_platform_printf("0x%02x, ", *(const_size_str + (uintptr_t)1));
    pika_platform_printf("0x%02x, ", *(const_size_str + (uintptr_t)2));
    pika_platform_printf("0x%02x, ", *(const_size_str + (uintptr_t)3));
    pika_platform_printf("/* const pool size */\n");
    uint16_t ptr_befor = self->content_offset_now;
    uint8_t line_num = 12;
    uint16_t g_i = 0;
    /* set ptr_now to begin */
    self->content_offset_now = 0;
    pika_platform_printf("0x00, ");
    while (1) {
        if (NULL == constPool_getNext(self)) {
            goto __exit;
        }
        char* data_each = constPool_getNow(self);
        /* todo start */
        size_t len = strlen(data_each);
        for (uint32_t i = 0; i < len + 1; i++) {
            pika_platform_printf("0x%02x, ", *(data_each + (uintptr_t)i));
            g_i++;
            if (g_i % line_num == 0) {
                pika_platform_printf("\n");
            }
        }
        /* todo end */
    }
__exit:
    /* retore ptr_now */
    pika_platform_printf("/* const pool */\n");
    self->content_offset_now = ptr_befor;
    return;
}

void byteCodeFrame_printAsArray(ByteCodeFrame* self) {
    pika_platform_printf("const uint8_t bytes[] = {\n");
    instructArray_printAsArray(&(self->instruct_array));
    constPool_printAsArray(&(self->const_pool));
    pika_platform_printf("};\n");
    pika_platform_printf("pikaVM_runByteCode(self, (uint8_t*)bytes);\n");
}

PikaObj* pikaVM_runFile(PikaObj* self, char* file_name) {
    Args buffs = {0};
    char* module_name = strsPathGetFileName(&buffs, file_name);
    strPopLastToken(module_name, '.');
    char* pwd = strsPathGetFolder(&buffs, file_name);
    pika_platform_printf("(pikascript) pika compiler:\r\n");
    PikaMaker* maker = New_PikaMaker();
    pikaMaker_setPWD(maker, pwd);
    pikaMaker_compileModuleWithDepends(maker, module_name);
    _do_pikaMaker_linkCompiledModules(maker, "pikaModules_cache.py.a",
                                      pika_false);
    pikaMaker_deinit(maker);
    pika_platform_printf("(pikascript) all succeed.\r\n\r\n");

    pikaMemMaxReset();
    char* libfile_path =
        strsPathJoin(&buffs, pwd, "pikascript-api/pikaModules_cache.py.a");
    if (PIKA_RES_OK == obj_linkLibraryFile(self, libfile_path)) {
        obj_runModule(self, module_name);
    }
    strsDeinit(&buffs);
    return self;
}

void _pikaVM_yield(void) {
#if PIKA_EVENT_ENABLE
    if (!pika_GIL_isInit()) {
        _VMEvent_pickupEvent();
    }
#endif
    /*
     * [Warning!] Can not use pika_GIL_ENTER() and pika_GIL_EXIT() here,
     * because yield() is called not atomically, and it will cause data
     * race.
     */
    // pika_GIL_EXIT();
    // pika_GIL_ENTER();
}
