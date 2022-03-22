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

#define __PIKA_OBJ_CLASS_IMPLEMENT
#include "PikaVM.h"
#include "BaseObj.h"
#include "PikaObj.h"
#include "PikaParser.h"
#include "PikaPlatform.h"
#include "dataStrs.h"

static InstructUnit* VMState_getInstructNow(VMState* vs) {
    return instructArray_getByOffset(&(vs->bytecode_frame->instruct_array),
                                     vs->pc);
}

static InstructUnit* VMState_getInstructWithOffset(VMState* vs,
                                                   int32_t offset) {
    return instructArray_getByOffset(&(vs->bytecode_frame->instruct_array),
                                     vs->pc + offset);
}

static int VMState_getBlockDeepthNow(VMState* vs) {
    /* support run byteCode */
    InstructUnit* ins_unit = VMState_getInstructNow(vs);
    return instructUnit_getBlockDeepth(ins_unit);
}

static char* VMState_getConstWithInstructUnit(VMState* vs,
                                              InstructUnit* ins_unit) {
    return constPool_getByOffset(&(vs->bytecode_frame->const_pool),
                                 instructUnit_getConstPoolIndex(ins_unit));
}

static int32_t VMState_getAddrOffsetOfJUM(VMState* vs) {
    int offset = 0;
    InstructUnit* ins_unit_now = VMState_getInstructNow(vs);
    while (1) {
        offset += instructUnit_getSize(ins_unit_now);
        ins_unit_now = VMState_getInstructWithOffset(vs, offset);
        uint16_t invoke_deepth = instructUnit_getInvokeDeepth(ins_unit_now);
        enum Instruct ins = instructUnit_getInstruct(ins_unit_now);
        char* data = VMState_getConstWithInstructUnit(vs, ins_unit_now);
        if ((0 == invoke_deepth) && (JMP == ins) && strEqu(data, "-1")) {
            return offset;
        }
    }
}

static size_t VMState_getInstructArraySize(VMState* vs) {
    return instructArray_getSize(&(vs->bytecode_frame->instruct_array));
}

static int32_t VMState_getAddrOffsetFromJmp(VMState* vs) {
    int offset = 0;
    /* run byte Code */
    InstructUnit* this_ins_unit = VMState_getInstructNow(vs);
    int thisBlockDeepth = instructUnit_getBlockDeepth(this_ins_unit);
    int8_t blockNum = 0;

    if (vs->jmp > 0) {
        offset = 0;
        while (1) {
            offset += instructUnit_getSize();
            /* reach the end */
            if (vs->pc + offset >= (int)VMState_getInstructArraySize(vs)) {
                break;
            }
            this_ins_unit = VMState_getInstructWithOffset(vs, offset);
            if (instructUnit_getIsNewLine(this_ins_unit)) {
                uint8_t blockDeepth =
                    instructUnit_getBlockDeepth(this_ins_unit);
                if (blockDeepth <= thisBlockDeepth) {
                    blockNum++;
                }
            }
            if (blockNum >= vs->jmp) {
                break;
            }
        }
    }
    if (vs->jmp < 0) {
        while (1) {
            offset -= instructUnit_getSize();
            this_ins_unit = VMState_getInstructWithOffset(vs, offset);
            if (instructUnit_getIsNewLine(this_ins_unit)) {
                uint8_t blockDeepth =
                    instructUnit_getBlockDeepth(this_ins_unit);
                if (blockDeepth == thisBlockDeepth) {
                    blockNum--;
                }
            }
            if (blockNum <= vs->jmp) {
                break;
            }
        }
    }
    return offset;
}

static int32_t VMState_getAddrOffsetOfBreak(VMState* vs) {
    int32_t offset = VMState_getAddrOffsetOfJUM(vs);
    /* byteCode */
    offset += instructUnit_getSize();
    return offset;
}

static int32_t VMState_getAddrOffsetOfContinue(VMState* vs) {
    int32_t offset = VMState_getAddrOffsetOfJUM(vs);
    /* byteCode */
    return offset;
}

int32_t __clearInvokeQueues(VMParameters* vm_pars) {
    for (char deepthChar = '0'; deepthChar < '9'; deepthChar++) {
        char deepth[2] = {0};
        deepth[0] = deepthChar;
        Queue* queue = (Queue*)args_getPtr(vm_pars->list, deepth);
        if (NULL != queue) {
            args_deinit(queue);
            args_removeArg(vm_pars->list, args_getArg(vm_pars->list, deepth));
        }
    }
    return 0;
}

typedef Arg* (*VM_instruct_handler)(PikaObj* self, VMState* vs, char* data);

static Arg* VM_instruction_handler_NON(PikaObj* self, VMState* vs, char* data) {
    return NULL;
}

static Arg* VM_instruction_handler_NEW(PikaObj* self, VMState* vs, char* data) {
    Arg* origin_arg = obj_getArg(vs->locals, data);
    Arg* new_arg = arg_copy(origin_arg);
    origin_arg = arg_setType(origin_arg, ARG_TYPE_POINTER);
    arg_setType(new_arg, ARG_TYPE_FREE_OBJECT);
    return new_arg;
}

static Arg* VM_instruction_handler_REF(PikaObj* self, VMState* vs, char* data) {
    if (strEqu(data, (char*)"True")) {
        return arg_setInt(NULL, "", 1);
    }
    if (strEqu(data, (char*)"False")) {
        return arg_setInt(NULL, "", 0);
    }
    /* find in local list first */
    Arg* arg = arg_copy(obj_getArg(vs->locals, data));
    if (NULL == arg) {
        /* find in global list second */
        arg = arg_copy(obj_getArg(vs->globals, data));
    }
    ArgType arg_type = arg_getType(arg);
    if (ARG_TYPE_OBJECT == arg_type) {
        arg = arg_setType(arg, ARG_TYPE_POINTER);
    }
    return arg;
}

static Arg* VM_instruction_handler_RUN(PikaObj* self, VMState* vs, char* data) {
    Args buffs = {0};
    Arg* return_arg = NULL;
    VMParameters* sub_locals = NULL;
    char* methodPath = data;
    PikaObj* method_host_obj;
    Arg* method_arg;
    Method method_ptr;
    ArgType method_type = ARG_TYPE_NONE;
    char* method_dec;
    char* type_list;
    char* sys_out;
    ByteCodeFrame* method_bytecodeFrame;
    /* return arg directly */
    if (strEqu(data, "")) {
        return_arg = arg_copy(queue_popArg(vs->q1));
        goto RUN_exit;
    }

    /* get method host obj */
    method_host_obj = obj_getObj(self, methodPath, 1);
    if (NULL == method_host_obj) {
        method_host_obj = obj_getObj(vs->locals, methodPath, 1);
    }
    if (NULL == method_host_obj) {
        /* error, not found object */
        args_setErrorCode(vs->locals->list, 1);
        args_setSysOut(vs->locals->list, "[error] runner: object no found.");
        goto RUN_exit;
    }
    /* get method in local */
    method_arg = obj_getMethodArg(method_host_obj, methodPath);
    if (NULL == method_arg) {
        method_arg = obj_getMethodArg(vs->globals, methodPath);
    }
    /* assert method*/
    if (NULL == method_arg) {
        /* error, method no found */
        args_setErrorCode(vs->locals->list, 2);
        args_setSysOut(vs->locals->list, "[error] runner: method no found.");
        goto RUN_exit;
    }
    /* get method Ptr */
    method_ptr = methodArg_getPtr(method_arg);
    /* get method Decleartion */
    method_dec = strsCopy(&buffs, methodArg_getDec(method_arg));
    method_type = arg_getType(method_arg);
    method_bytecodeFrame = methodArg_getBytecodeFrame(method_arg);
    arg_deinit(method_arg);

    /* get type list */
    type_list = strsCut(&buffs, method_dec, '(', ')');

    if (type_list == NULL) {
        /* typeList no found */
        args_setErrorCode(vs->locals->list, 3);
        args_setSysOut(vs->locals->list, "[error] runner: type list no found.");
        goto RUN_exit;
    }

    sub_locals = New_PikaObj();
    Arg* call_arg = NULL;
    uint8_t call_arg_index = 0;
    /* load pars */
    while (1) {
        /* load 'self' as the first arg when call object method */
        if ((method_type == ARG_TYPE_OBJECT_METHOD) && (call_arg_index == 0)) {
            call_arg = arg_setPtr(NULL, "", ARG_TYPE_POINTER, method_host_obj);
        } else {
            call_arg = arg_copy(queue_popArg(vs->q1));
        }
        /* exit when there is no arg in queue */
        if (NULL == call_arg) {
            break;
        }
        char* argDef = strsPopToken(&buffs, type_list, ',');
        char* argName = strsGetFirstToken(&buffs, argDef, ':');
        call_arg = arg_setName(call_arg, argName);
        args_setArg(sub_locals->list, call_arg);
        call_arg_index++;
    }

    obj_setErrorCode(method_host_obj, 0);
    obj_setSysOut(method_host_obj, "");

    /* run method */
    if (method_type == ARG_TYPE_NATIVE_METHOD) {
        /* native method */
        method_ptr(method_host_obj, sub_locals->list);
        /* get method return */
        return_arg = arg_copy(args_getArg(sub_locals->list, (char*)"return"));
    } else {
        /* static method and object method */
        /* byteCode */
        uint16_t pc = (uintptr_t)method_ptr -
                      (uintptr_t)instructArray_getByOffset(
                          &(method_bytecodeFrame->instruct_array), 0);
        sub_locals = pikaVM_runByteCodeWithState(
            method_host_obj, sub_locals, vs->globals, method_bytecodeFrame, pc);

        /* get method return */
        return_arg = arg_copy(args_getArg(sub_locals->list, (char*)"return"));
    }
    /* transfer sysOut */
    sys_out = obj_getSysOut(method_host_obj);
    if (NULL != sys_out) {
        args_setSysOut(vs->locals->list, sys_out);
    }
    /* transfer errCode */
    if (0 != obj_getErrorCode(method_host_obj)) {
        /* method error */
        args_setErrorCode(vs->locals->list, 6);
    }

    goto RUN_exit;
RUN_exit:
    if (NULL != sub_locals) {
        obj_deinit(sub_locals);
    }
    strsDeinit(&buffs);
    return return_arg;
}

static Arg* VM_instruction_handler_STR(PikaObj* self, VMState* vs, char* data) {
    Arg* strArg = New_arg(NULL);
    return arg_setStr(strArg, "", data);
}

typedef enum {
    IS_INIT_OBJ_TRUE,
    IS_INIT_OBJ_FALSE,
} is_init_obj_t;

static Arg* __VM_OUT(PikaObj* self,
                     VMState* vs,
                     char* data,
                     is_init_obj_t is_init_obj) {
    Arg* outArg = arg_copy(queue_popArg(vs->q0));
    ArgType outArg_type = arg_getType(outArg);
    PikaObj* hostObj = vs->locals;
    /* match global_list */
    if (args_isArgExist(vs->locals->list, "__gl")) {
        char* global_list = args_getStr(vs->locals->list, "__gl");
        /* use a arg as buff */
        Arg* global_list_arg = arg_setStr(NULL, "", global_list);
        char* global_list_buff = arg_getStr(global_list_arg);
        /* for each arg arg in global_list */
        char token_buff[32] = {0};
        for (int i = 0; i < strCountSign(global_list, ',') + 1; i++) {
            char* global_arg = strPopToken(token_buff, global_list_buff, ',');
            /* matched global arg, hostObj set to global */
            if (strEqu(global_arg, data)) {
                hostObj = vs->globals;
            }
        }
        arg_deinit(global_list_arg);
    }
    /* use RunAs object */
    if (args_isArgExist(vs->locals->list, "__runAs")) {
        hostObj = args_getPtr(vs->locals->list, "__runAs");
    }
    /* set free object to nomal object */
    if (ARG_TYPE_FREE_OBJECT == outArg_type) {
        arg_setType(outArg, ARG_TYPE_OBJECT);
    }
    /* ouput arg to locals */
    obj_setArg(hostObj, data, outArg);
    if ((ARG_TYPE_MATE_OBJECT == outArg_type) ||
        (ARG_TYPE_FREE_OBJECT == outArg_type)) {
        if (is_init_obj == IS_INIT_OBJ_TRUE) {
            /* found a mate_object */
            /* init object */
            PikaObj* new_obj = obj_getObj(hostObj, data, 0);
            /* run __init__() when init obj */
            Arg* init_method_arg = NULL;
            init_method_arg = obj_getMethodArg(new_obj, "__init__");
            if (NULL != init_method_arg) {
                arg_deinit(init_method_arg);
                // pikaVM_runAsm(new_obj,
                //               "B0\n"
                //               "0 RUN __init__\n");
                const uint8_t bytes[] = {
                    0x04, 0x00,             /* instruct array size */
                    0x00, 0x82, 0x01, 0x00, /* instruct array */
                    0x0a, 0x00,             /* const pool size */
                    0x00, 0x5f, 0x5f, 0x69, 0x6e,
                    0x69, 0x74, 0x5f, 0x5f, 0x00, /* const pool */
                };
                pikaVM_runByteCode(new_obj, (uint8_t*)bytes);
            }
        }
    }
    arg_deinit(outArg);
    return NULL;
}

static Arg* VM_instruction_handler_OUT(PikaObj* self, VMState* vs, char* data) {
    return __VM_OUT(self, vs, data, IS_INIT_OBJ_TRUE);
}

/* run as */
static Arg* VM_instruction_handler_RAS(PikaObj* self, VMState* vs, char* data) {
    if (strEqu(data, "$origin")) {
        /* use origin object to run */
        obj_removeArg(vs->locals, "__runAs");
        return NULL;
    }
    /* use "data" object to run */
    PikaObj* runAs = obj_getObj(vs->locals, data, 0);
    args_setPtr(vs->locals->list, "__runAs", runAs);
    return NULL;
}

static Arg* VM_instruction_handler_NUM(PikaObj* self, VMState* vs, char* data) {
    Arg* numArg = New_arg(NULL);
    if (strIsContain(data, '.')) {
        return arg_setFloat(numArg, "", atof(data));
    }
    return arg_setInt(numArg, "", fast_atoi(data));
}

static Arg* VM_instruction_handler_JMP(PikaObj* self, VMState* vs, char* data) {
    vs->jmp = fast_atoi(data);
    return NULL;
}

static Arg* VM_instruction_handler_JEZ(PikaObj* self, VMState* vs, char* data) {
    int thisBlockDeepth;
    thisBlockDeepth = VMState_getBlockDeepthNow(vs);
    Arg* assertArg = arg_copy(queue_popArg(vs->q0));
    int assert = arg_getInt(assertArg);
    arg_deinit(assertArg);
    char __else[] = "__else0";
    __else[6] = '0' + thisBlockDeepth;
    args_setInt(self->list, __else, !assert);
    if (0 == assert) {
        /* set __else flag */
        vs->jmp = fast_atoi(data);
    }
    return NULL;
}

static Arg* VM_instruction_handler_OPT(PikaObj* self, VMState* vs, char* data) {
    Arg* outArg = NULL;
    Arg* arg1 = arg_copy(queue_popArg(vs->q1));
    Arg* arg2 = arg_copy(queue_popArg(vs->q1));
    ArgType type_arg1 = arg_getType(arg1);
    ArgType type_arg2 = arg_getType(arg2);
    int num1_i = 0;
    int num2_i = 0;
    float num1_f = 0.0;
    float num2_f = 0.0;
    /* get int and float num */
    if (type_arg1 == ARG_TYPE_INT) {
        num1_i = arg_getInt(arg1);
        num1_f = (float)num1_i;
    } else if (type_arg1 == ARG_TYPE_FLOAT) {
        num1_f = arg_getFloat(arg1);
        num1_i = (int)num1_f;
    }
    if (type_arg2 == ARG_TYPE_INT) {
        num2_i = arg_getInt(arg2);
        num2_f = (float)num2_i;
    } else if (type_arg2 == ARG_TYPE_FLOAT) {
        num2_f = arg_getFloat(arg2);
        num2_i = (int)num2_f;
    }
    if (strEqu("+", data)) {
        if ((type_arg1 == ARG_TYPE_STRING) && (type_arg2 == ARG_TYPE_STRING)) {
            char* num1_s = NULL;
            char* num2_s = NULL;
            Args str_opt_buffs = {0};
            num1_s = arg_getStr(arg1);
            num2_s = arg_getStr(arg2);
            char* opt_str_out = strsAppend(&str_opt_buffs, num1_s, num2_s);
            outArg = arg_setStr(outArg, "", opt_str_out);
            strsDeinit(&str_opt_buffs);
            goto OPT_exit;
        }
        /* match float */
        if ((type_arg1 == ARG_TYPE_FLOAT) || type_arg2 == ARG_TYPE_FLOAT) {
            outArg = arg_setFloat(outArg, "", num1_f + num2_f);
            goto OPT_exit;
        }
        /* int is default */
        outArg = arg_setInt(outArg, "", num1_i + num2_i);
        goto OPT_exit;
    }
    if (strEqu("-", data)) {
        if (type_arg2 == ARG_TYPE_NONE) {
            if (type_arg1 == ARG_TYPE_INT) {
                outArg = arg_setInt(outArg, "", -num1_i);
                goto OPT_exit;
            }
            if (type_arg1 == ARG_TYPE_FLOAT) {
                outArg = arg_setFloat(outArg, "", -num1_f);
                goto OPT_exit;
            }
        }
        if ((type_arg1 == ARG_TYPE_FLOAT) || type_arg2 == ARG_TYPE_FLOAT) {
            outArg = arg_setFloat(outArg, "", num1_f - num2_f);
            goto OPT_exit;
        }
        outArg = arg_setInt(outArg, "", num1_i - num2_i);
        goto OPT_exit;
    }
    if (strEqu("*", data)) {
        if ((type_arg1 == ARG_TYPE_FLOAT) || type_arg2 == ARG_TYPE_FLOAT) {
            outArg = arg_setFloat(outArg, "", num1_f * num2_f);
            goto OPT_exit;
        }
        outArg = arg_setInt(outArg, "", num1_i * num2_i);
        goto OPT_exit;
    }
    if (strEqu("/", data)) {
        outArg = arg_setFloat(outArg, "", num1_f / num2_f);
        goto OPT_exit;
    }
    if (strEqu("<", data)) {
        outArg = arg_setInt(outArg, "", num1_f < num2_f);
        goto OPT_exit;
    }
    if (strEqu(">", data)) {
        outArg = arg_setInt(outArg, "", num1_f > num2_f);
        goto OPT_exit;
    }
    if (strEqu("%", data)) {
        outArg = arg_setInt(outArg, "", num1_i % num2_i);
        goto OPT_exit;
    }
    if (strEqu("**", data)) {
        float res = 1;
        for (int i = 0; i < num2_i; i++) {
            res = res * num1_f;
        }
        outArg = arg_setFloat(outArg, "", res);
        goto OPT_exit;
    }
    if (strEqu("//", data)) {
        outArg = arg_setInt(outArg, "", num1_i / num2_i);
        goto OPT_exit;
    }
    if (strEqu("==", data)) {
        /* string compire */
        if ((type_arg1 == ARG_TYPE_STRING) && (type_arg2 == ARG_TYPE_STRING)) {
            outArg = arg_setInt(outArg, "",
                                strEqu(arg_getStr(arg1), arg_getStr(arg2)));
            goto OPT_exit;
        }
        /* default: int and float */
        outArg =
            arg_setInt(outArg, "",
                       (num1_f - num2_f) * (num1_f - num2_f) < (float)0.000001);
        goto OPT_exit;
    }
    if (strEqu("!=", data)) {
        outArg = arg_setInt(
            outArg, "",
            !((num1_f - num2_f) * (num1_f - num2_f) < (float)0.000001));
        goto OPT_exit;
    }
    if (strEqu(">=", data)) {
        outArg = arg_setInt(
            outArg, "",
            (num1_f > num2_f) ||
                ((num1_f - num2_f) * (num1_f - num2_f) < (float)0.000001));
        goto OPT_exit;
    }
    if (strEqu("<=", data)) {
        outArg = arg_setInt(
            outArg, "",
            (num1_f < num2_f) ||
                ((num1_f - num2_f) * (num1_f - num2_f) < (float)0.000001));
        goto OPT_exit;
    }
    if (strEqu("&", data)) {
        outArg = arg_setInt(outArg, "", num1_i & num2_i);
        goto OPT_exit;
    }
    if (strEqu("|", data)) {
        outArg = arg_setInt(outArg, "", num1_i | num2_i);
        goto OPT_exit;
    }
    if (strEqu("~", data)) {
        outArg = arg_setInt(outArg, "", ~num1_i);
        goto OPT_exit;
    }
    if (strEqu(">>", data)) {
        outArg = arg_setInt(outArg, "", num1_i >> num2_i);
        goto OPT_exit;
    }
    if (strEqu("<<", data)) {
        outArg = arg_setInt(outArg, "", num1_i << num2_i);
        goto OPT_exit;
    }
    if (strEqu(" and ", data)) {
        outArg = arg_setInt(outArg, "", num1_i && num2_i);
        goto OPT_exit;
    }
    if (strEqu(" or ", data)) {
        outArg = arg_setInt(outArg, "", num1_i || num2_i);
        goto OPT_exit;
    }
    if (strEqu(" not ", data)) {
        outArg = arg_setInt(outArg, "", !num1_i);
        goto OPT_exit;
    }
OPT_exit:
    arg_deinit(arg1);
    arg_deinit(arg2);
    if (NULL != outArg) {
        return outArg;
    }
    return NULL;
}

static Arg* VM_instruction_handler_DEF(PikaObj* self, VMState* vs, char* data) {
    int thisBlockDeepth = VMState_getBlockDeepthNow(vs);

    PikaObj* hostObj = vs->locals;
    uint8_t is_in_class = 0;
    /* use RunAs object */
    if (args_isArgExist(vs->locals->list, "__runAs")) {
        hostObj = args_getPtr(vs->locals->list, "__runAs");
        is_in_class = 1;
    }
    int offset = 0;
    /* byteCode */
    while (1) {
        InstructUnit* ins_unit_now = VMState_getInstructWithOffset(vs, offset);
        if (!instructUnit_getIsNewLine(ins_unit_now)) {
            offset += instructUnit_getSize();
            continue;
        }
        if (instructUnit_getBlockDeepth(ins_unit_now) == thisBlockDeepth + 1) {
            if (is_in_class) {
                class_defineObjectMethod(hostObj, data, (Method)ins_unit_now,
                                         vs->bytecode_frame);
            } else {
                class_defineStaticMethod(hostObj, data, (Method)ins_unit_now,
                                         vs->bytecode_frame);
            }
            break;
        }
        offset += instructUnit_getSize();
    }

    return NULL;
}

static Arg* VM_instruction_handler_RET(PikaObj* self, VMState* vs, char* data) {
    /* exit jmp signal */
    vs->jmp = -999;
    Arg* returnArg = arg_copy(queue_popArg(vs->q0));
    method_returnArg(vs->locals->list, returnArg);
    return NULL;
}

static Arg* VM_instruction_handler_NEL(PikaObj* self, VMState* vs, char* data) {
    int thisBlockDeepth = VMState_getBlockDeepthNow(vs);
    char __else[] = "__else0";
    __else[6] = '0' + thisBlockDeepth;
    if (0 == args_getInt(self->list, __else)) {
        /* set __else flag */
        vs->jmp = fast_atoi(data);
    }
    return NULL;
}

static Arg* VM_instruction_handler_DEL(PikaObj* self, VMState* vs, char* data) {
    obj_removeArg(vs->locals, data);
    return NULL;
}

static Arg* VM_instruction_handler_EST(PikaObj* self, VMState* vs, char* data) {
    Arg* arg = obj_getArg(vs->locals, data);
    if (arg == NULL) {
        return arg_setInt(NULL, "", 0);
    }
    if (ARG_TYPE_NULL == arg_getType(arg)) {
        return arg_setInt(NULL, "", 0);
    }
    return arg_setInt(NULL, "", 1);
}

static Arg* VM_instruction_handler_BRK(PikaObj* self, VMState* vs, char* data) {
    /* break jmp signal */
    vs->jmp = -998;
    return NULL;
}

static Arg* VM_instruction_handler_CTN(PikaObj* self, VMState* vs, char* data) {
    /* continue jmp signal */
    vs->jmp = -997;
    return NULL;
}

static Arg* VM_instruction_handler_GLB(PikaObj* self, VMState* vs, char* data) {
    Arg* global_list_buff = NULL;
    char* global_list = args_getStr(vs->locals->list, "__gl");
    /* create new global_list */
    if (NULL == global_list) {
        args_setStr(vs->locals->list, "__gl", data);
        goto exit;
    }
    /* append to exist global_list */
    global_list_buff = arg_setStr(NULL, "", global_list);
    global_list_buff = arg_strAppend(global_list_buff, ",");
    global_list_buff = arg_strAppend(global_list_buff, data);
    args_setStr(vs->locals->list, "__gl", arg_getStr(global_list_buff));
    goto exit;
exit:
    if (NULL != global_list_buff) {
        arg_deinit(global_list_buff);
    }
    return NULL;
}

const VM_instruct_handler VM_instruct_handler_table[__INSTRCUTION_CNT] = {
#define __INS_TABLE
#include "__instruction_table.cfg"
};

enum Instruct pikaVM_getInstructFromAsm(char* line) {
#define __INS_COMPIRE
#include "__instruction_table.cfg"
    return NON;
}

static int pikaVM_runInstructUnit(PikaObj* self,
                                  VMState* vs,
                                  InstructUnit* ins_unit) {
    enum Instruct instruct = instructUnit_getInstruct(ins_unit);
    Arg* resArg;
    char invode_deepth0_str[2] = {0};
    char invode_deepth1_str[2] = {0};
    int32_t pc_next = vs->pc + instructUnit_getSize();
    /* Found new script Line, clear the queues*/
    if (instructUnit_getIsNewLine(ins_unit)) {
        args_setErrorCode(vs->locals->list, 0);
        // args_setSysOut(vs->locals->list, (char*)"");
        __clearInvokeQueues(vs->locals);
    }

    invode_deepth0_str[0] = instructUnit_getInvokeDeepth(ins_unit) + '0';
    invode_deepth1_str[0] = invode_deepth0_str[0] + 1;

    char* data = VMState_getConstWithInstructUnit(vs, ins_unit);

    vs->q0 = args_getPtr(vs->locals->list, invode_deepth0_str);
    vs->q1 = args_getPtr(vs->locals->list, invode_deepth1_str);

    if (NULL == vs->q0) {
        vs->q0 = New_queue();
        args_setPtr(vs->locals->list, invode_deepth0_str, vs->q0);
    }
    if (NULL == vs->q1) {
        vs->q1 = New_queue();
        args_setPtr(vs->locals->list, invode_deepth1_str, vs->q1);
    }
    /* run instruct */
    resArg = VM_instruct_handler_table[instruct](self, vs, data);
    if (NULL != resArg) {
        queue_pushArg(vs->q0, resArg);
    }
    goto nextLine;
nextLine:
    /* exit */
    if (-999 == vs->jmp) {
        pc_next = -99999;
        goto exit;
    }
    /* break */
    if (-998 == vs->jmp) {
        pc_next = vs->pc + VMState_getAddrOffsetOfBreak(vs);
        goto exit;
    }
    /* continue */
    if (-997 == vs->jmp) {
        pc_next = vs->pc + VMState_getAddrOffsetOfContinue(vs);
        goto exit;
    }
    /* static jmp */
    if (vs->jmp != 0) {
        pc_next = vs->pc + VMState_getAddrOffsetFromJmp(vs);
        goto exit;
    }
    /* not jmp */
    pc_next = vs->pc + instructUnit_getSize();
    goto exit;
exit:
    vs->jmp = 0;
    /* reach the end */
    if (pc_next >= (int)VMState_getInstructArraySize(vs)) {
        return -99999;
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

VMParameters* pikaVM_runPyOrByteCode(PikaObj* self,
                                     char* multiLine,
                                     uint8_t* bytecode) {
    uint8_t is_run_py;
    if (NULL != multiLine) {
        is_run_py = 1;
    } else if (NULL != bytecode) {
        is_run_py = 0;
    } else {
        return NULL;
    }
    Args buffs = {0};
    VMParameters* globals = NULL;
    ByteCodeFrame bytecode_frame_stack = {0};
    ByteCodeFrame* bytecode_frame_p = NULL;
    uint8_t is_use_heap_bytecode = 0;
    if (!args_isArgExist(self->list, "__bytecode")) {
        /* the first obj_run, save bytecode to heap */
        is_use_heap_bytecode = 1;
        /* load bytecode to heap */
        args_setHeapStruct(self->list, "__bytecode", bytecode_frame_stack,
                           byteCodeFrame_deinit);
        /* get bytecode_ptr from heap */
        bytecode_frame_p = args_getHeapStruct(self->list, "__bytecode");
    } else {
        /* not the first obj_run, bytecode only in stack */
        /* get bytecode_ptr from stack */
        bytecode_frame_p = &bytecode_frame_stack;
    }
    /* load or generate byte code frame */
    if (is_run_py) {
        /* generate byte code */
        byteCodeFrame_init(bytecode_frame_p);
        if (1 == bytecodeFrame_fromMultiLine(bytecode_frame_p, multiLine)) {
            __platform_printf("[error]: Syntax error.\r\n");
            globals = NULL;
            goto exit;
        }
    } else {
        /* load bytecode */
        byteCodeFrame_loadBytes(bytecode_frame_p, bytecode);
    }
    /* run byteCode */
    globals = pikaVM_runByteCodeFrame(self, bytecode_frame_p);
    goto exit;
exit:
    if (!is_use_heap_bytecode) {
        byteCodeFrame_deinit(&bytecode_frame_stack);
    }
    strsDeinit(&buffs);
    return globals;
}

VMParameters* pikaVM_run(PikaObj* self, char* multiLine) {
    return pikaVM_runPyOrByteCode(self, multiLine, NULL);
}

VMParameters* pikaVM_runByteCode(PikaObj* self, uint8_t* bytecode) {
    return pikaVM_runPyOrByteCode(self, NULL, bytecode);
}

static void* constPool_getStart(ConstPool* self) {
    return self->content_start;
}

void constPool_update(ConstPool* self) {
    self->content_start = (void*)arg_getContent(self->arg_buff);
}

void constPool_init(ConstPool* self) {
    self->arg_buff = arg_setStr(NULL, "", "");
    constPool_update(self);
    self->content_offset_now = 0;
    self->size = strGetSize(constPool_getStart(self)) + 1;
    self->output_redirect_fun = NULL;
    self->output_f = NULL;
}

void constPool_deinit(ConstPool* self) {
    if (NULL != self->arg_buff) {
        arg_deinit(self->arg_buff);
    }
}

void constPool_append(ConstPool* self, char* content) {
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
    return constPool_getStart(self) + (uintptr_t)(self->content_offset_now);
}

uint16_t constPool_getLastOffset(ConstPool* self) {
    return self->size;
}

uint16_t constPool_getOffsetByData(ConstPool* self, char* data) {
    uint16_t ptr_befor = self->content_offset_now;
    /* set ptr_now to begin */
    self->content_offset_now = 0;
    uint16_t offset_out = 65535;
    while (1) {
        if (NULL == constPool_getNext(self)) {
            goto exit;
        }
        if (strEqu(data, constPool_getNow(self))) {
            offset_out = self->content_offset_now;
            goto exit;
        }
    }
exit:
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

void byteCodeFrame_loadBytes(ByteCodeFrame* self, uint8_t* bytes) {
    uint16_t* ins_size_p = (uint16_t*)bytes;
    void* ins_start_p = (uint16_t*)(bytes + 2);
    uint16_t* const_size_p = ins_start_p + *ins_size_p;
    self->instruct_array.size = *ins_size_p;
    self->instruct_array.content_start = ins_start_p;
    self->const_pool.size = *const_size_p;
    self->const_pool.content_start = (void*)const_size_p + 2;
}

void byteCodeFrame_deinit(ByteCodeFrame* self) {
    constPool_deinit(&(self->const_pool));
    instructArray_deinit(&(self->instruct_array));
}

void instructArray_init(InstructArray* self) {
    self->arg_buff = arg_setNull(NULL);
    instructArray_update(self);
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

static void* instructArray_getStart(InstructArray* self) {
    return self->content_start;
}

void instructArray_update(InstructArray* self) {
    self->content_start = (void*)arg_getContent(self->arg_buff);
}

static InstructUnit* instructArray_getNow(InstructArray* self) {
    if (self->content_offset_now >= self->size) {
        /* is the end */
        return NULL;
    }
    return (InstructUnit*)(instructArray_getStart(self) +
                           (uintptr_t)(self->content_offset_now));
}

static InstructUnit* instructArray_getNext(InstructArray* self) {
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
    if (instructUnit_getIsNewLine(self)) {
        __platform_printf("B%d\r\n", instructUnit_getBlockDeepth(self));
    }
    __platform_printf("%d %s %s \t\t(#%d)\r\n",
                      instructUnit_getInvokeDeepth(self),
                      instructUnit_getInstructStr(self),
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
    __platform_printf("/* instruct array size */\n");
    while (1) {
        InstructUnit* ins_unit = instructArray_getNow(self);
        if (NULL == ins_unit) {
            goto exit;
        }
        for (int i = 0; i < (int)instructUnit_getSize(ins_unit); i++) {
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

VMParameters* pikaVM_runByteCodeWithState(PikaObj* self,
                                          VMParameters* locals,
                                          VMParameters* globals,
                                          ByteCodeFrame* bytecode_frame,
                                          uint16_t pc) {
    int size = bytecode_frame->instruct_array.size;
    args_setErrorCode(locals->list, 0);
    // args_setSysOut(locals->list, (char*)"");
    VMState vs = {
        .bytecode_frame = bytecode_frame,
        .locals = locals,
        .globals = globals,
        .jmp = 0,
        .q0 = NULL,
        .q1 = NULL,
        .pc = pc,
    };
    while (vs.pc < size) {
        if (vs.pc == -99999) {
            break;
        }
        InstructUnit* this_ins_unit = VMState_getInstructNow(&vs);
        vs.pc = pikaVM_runInstructUnit(self, &vs, this_ins_unit);
        uint8_t errcode = args_getErrorCode(locals->list);
        if (0 != errcode) {
            __platform_printf("[info] input commond: \r\n");
            instructUnit_printWithConst(this_ins_unit,
                                        &(bytecode_frame->const_pool));
        }
    }
    __clearInvokeQueues(locals);

    return locals;
}

VMParameters* pikaVM_runByteCodeFrame(PikaObj* self,
                                      ByteCodeFrame* byteCode_frame) {
    return pikaVM_runByteCodeWithState(self, self, self, byteCode_frame, 0);
}

char* constPool_getByOffset(ConstPool* self, uint16_t offset) {
    return (char*)(constPool_getStart(self) + (uintptr_t)offset);
}

InstructUnit* instructArray_getByOffset(InstructArray* self, int32_t offset) {
    return (InstructUnit*)(instructArray_getStart(self) + (uintptr_t)offset);
}

void constPool_printAsArray(ConstPool* self) {
    uint8_t* const_size_str = (uint8_t*)&(self->size);
    __platform_printf("0x%02x, ", *(const_size_str));
    __platform_printf("0x%02x, ", *(const_size_str + (uintptr_t)1));
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
        for (uint32_t i = 0; i < strGetSize(data_each) + 1; i++) {
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
