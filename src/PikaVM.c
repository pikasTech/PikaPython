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
#include "dataQueue.h"
#include "dataQueueObj.h"
#include "dataStrs.h"

struct VM_State_t {
    VM_Parameters* locals;
    VM_Parameters* globals;
    Queue* q0;
    Queue* q1;
    int32_t jmp;
    char* pc;
    char* ASM_start;
};
typedef struct VM_State_t VM_State;

static int32_t getLineSize(char* str) {
    int i = 0;
    while (1) {
        if (str[i] == '\n') {
            return i;
        }
        i++;
    }
}


enum Instruct {
    
    #define __INS_ENUM
    #include "__instruction_table.cfg"
    
    __INSTRCUTION_CNT,
};





typedef Arg* (*VM_instruct_handler)(PikaObj* self,
                                    struct VM_State_t* vmState,
                                    char* data);

                                    
static Arg* VM_instruction_handler_NON(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    return NULL;
}

static Arg* VM_instruction_handler_REF(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    if (strEqu(data, (char*)"True")) {
        return arg_setInt(NULL, "", 1);
    }
    if (strEqu(data, (char*)"False")) {
        return arg_setInt(NULL, "", 0);
    }
    /* find in local list first */
    Arg* arg = arg_copy(obj_getArg(vmState->locals, data));
    if (NULL == arg) {
        /* find in global list second */
        arg = arg_copy(obj_getArg(vmState->globals, data));
    }
    ArgType arg_type = arg_getType(arg);
    if (TYPE_OBJECT == arg_type) {
        arg = arg_setType(arg, TYPE_POINTER);
    }
    return arg;
}

static Arg* VM_instruction_handler_RUN(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    Args* buffs = New_strBuff();
    Arg* returnArg = NULL;
    VM_Parameters* subLocals = NULL;
    char* methodPath = data;
    PikaObj* methodHostObj;
    Arg* method_arg;
    Method methodPtr;
    char* methodDec;
    char* typeList;
    char* methodCode;
    char* sysOut;
    /* return arg directly */
    if (strEqu(data, "")) {
        returnArg = arg_copy(queue_popArg(vmState->q1));
        goto RUN_exit;
    }
    /* get method host obj */
    methodHostObj = obj_getObj(self, methodPath, 1);
    if (NULL == methodHostObj) {
        methodHostObj = obj_getObj(vmState->locals, methodPath, 1);
    }
    if (NULL == methodHostObj) {
        /* error, not found object */
        args_setErrorCode(vmState->locals->list, 1);
        args_setSysOut(vmState->locals->list,
                       "[error] runner: object no found.");
        goto RUN_exit;
    }
    /* get method */
    method_arg = obj_getMethod(methodHostObj, methodPath);
    /* assert method*/
    if (NULL == method_arg) {
        /* error, method no found */
        args_setErrorCode(vmState->locals->list, 2);
        args_setSysOut(vmState->locals->list,
                       "[error] runner: method no found.");
        goto RUN_exit;
    }
    /* get method Ptr */
    methodPtr = (Method)methodArg_getPtr(method_arg);
    /* get method Decleartion */
    methodDec = strsCopy(buffs, methodArg_getDec(method_arg));
    arg_deinit(method_arg);

    /* get type list */
    typeList = strsCut(buffs, methodDec, '(', ')');

    if (typeList == NULL) {
        /* typeList no found */
        args_setErrorCode(vmState->locals->list, 3);
        args_setSysOut(vmState->locals->list,
                       "[error] runner: type list no found.");
        goto RUN_exit;
    }

    subLocals = New_PikaObj();
    while (1) {
        Arg* methodArg = arg_copy(queue_popArg(vmState->q1));
        if (NULL == methodArg) {
            break;
        }
        char* argDef = strsPopToken(buffs, typeList, ',');
        char* argName = strsGetFirstToken(buffs, argDef, ':');
        methodArg = arg_setName(methodArg, argName);
        args_setArg(subLocals->list, methodArg);
    }

    obj_setErrorCode(methodHostObj, 0);
    obj_setSysOut(methodHostObj, "");

    /* run method */
    methodCode = (char*)methodPtr;
    if (methodCode[0] == 'B' && methodCode[2] == '\n') {
        /* VM method */
        subLocals = pikaVM_runAsmWithPars(methodHostObj, subLocals,
                                          vmState->globals, methodCode);
        /* get method return */
        returnArg = arg_copy(args_getArg(subLocals->list, (char*)"return"));
    } else {
        /* native method */
        methodPtr(methodHostObj, subLocals->list);
        /* get method return */
        returnArg = arg_copy(args_getArg(subLocals->list, (char*)"return"));
    }

    /* transfer sysOut */
    sysOut = obj_getSysOut(methodHostObj);
    if (NULL != sysOut) {
        args_setSysOut(vmState->locals->list, sysOut);
    }
    /* transfer errCode */
    if (0 != obj_getErrorCode(methodHostObj)) {
        /* method error */
        args_setErrorCode(vmState->locals->list, 6);
    }

    goto RUN_exit;
RUN_exit:
    if (NULL != subLocals) {
        obj_deinit(subLocals);
    }
    args_deinit(buffs);
    return returnArg;
}

static Arg* VM_instruction_handler_STR(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    Arg* strArg = New_arg(NULL);
    return arg_setStr(strArg, "", data);
}

static Arg* VM_instruction_handler_OUT(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    Arg* outArg = arg_copy(queue_popArg(vmState->q0));
    obj_setArg(vmState->locals, data, outArg);
    arg_deinit(outArg);
    return NULL;
}

static Arg* VM_instruction_handler_NUM(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    Arg* numArg = New_arg(NULL);
    if (strIsContain(data, '.')) {
        return arg_setFloat(numArg, "", atof(data));
    }
    return arg_setInt(numArg, "", fast_atoi(data));
}

static Arg* VM_instruction_handler_JMP(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    vmState->jmp = fast_atoi(data);
    return NULL;
}

static Arg* VM_instruction_handler_JEZ(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    int offset = 0;
    int thisBlockDeepth =
        getThisBlockDeepth(vmState->ASM_start, vmState->pc, &offset);
    Arg* assertArg = arg_copy(queue_popArg(vmState->q0));
    int assert = arg_getInt(assertArg);
    arg_deinit(assertArg);
    char __else[] = "__else0";
    __else[6] = '0' + thisBlockDeepth;
    args_setInt(self->list, __else, !assert);
    if (0 == assert) {
        /* set __else flag */
        vmState->jmp = fast_atoi(data);
    }
    return NULL;
}

static Arg* VM_instruction_handler_OPT(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    Arg* outArg = NULL;
    Arg* arg1 = arg_copy(queue_popArg(vmState->q1));
    Arg* arg2 = arg_copy(queue_popArg(vmState->q1));
    ArgType type_arg1 = arg_getType(arg1);
    ArgType type_arg2 = arg_getType(arg2);
    int num1_i = 0;
    int num2_i = 0;
    float num1_f = 0.0;
    float num2_f = 0.0;
    /* get int and float num */
    if (type_arg1 == TYPE_INT) {
        num1_i = arg_getInt(arg1);
        num1_f = (float)num1_i;
    }
    if (type_arg1 == TYPE_FLOAT) {
        num1_f = arg_getFloat(arg1);
        num1_i = (int)num1_f;
    }
    if (type_arg2 == TYPE_INT) {
        num2_i = arg_getInt(arg2);
        num2_f = (float)num2_i;
    }
    if (type_arg2 == TYPE_FLOAT) {
        num2_f = arg_getFloat(arg2);
        num2_i = (int)num2_f;
    }
    if (strEqu("+", data)) {
        if ((type_arg1 == TYPE_FLOAT) || type_arg2 == TYPE_FLOAT) {
            outArg = arg_setFloat(outArg, "", num1_f + num2_f);
            goto OPT_exit;
        }
        outArg = arg_setInt(outArg, "", num1_i + num2_i);
        goto OPT_exit;
    }
    if (strEqu("-", data)) {
        if ((type_arg1 == TYPE_FLOAT) || type_arg2 == TYPE_FLOAT) {
            outArg = arg_setFloat(outArg, "", num1_f - num2_f);
            goto OPT_exit;
        }
        outArg = arg_setInt(outArg, "", num1_i - num2_i);
        goto OPT_exit;
    }
    if (strEqu("*", data)) {
        if ((type_arg1 == TYPE_FLOAT) || type_arg2 == TYPE_FLOAT) {
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
        outArg = arg_setInt(outArg, "",
                            (num1_f - num2_f) * (num1_f - num2_f) < 0.000001);
        goto OPT_exit;
    }
    if (strEqu("!=", data)) {
        outArg = arg_setInt(
            outArg, "", !((num1_f - num2_f) * (num1_f - num2_f) < 0.000001));
        goto OPT_exit;
    }
    if (strEqu(">=", data)) {
        outArg =
            arg_setInt(outArg, "",
                       (num1_f > num2_f) ||
                           ((num1_f - num2_f) * (num1_f - num2_f) < 0.000001));
        goto OPT_exit;
    }
    if (strEqu("<=", data)) {
        outArg =
            arg_setInt(outArg, "",
                       (num1_f < num2_f) ||
                           ((num1_f - num2_f) * (num1_f - num2_f) < 0.000001));
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

static Arg* VM_instruction_handler_DEF(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    char* methodPtr = vmState->pc;
    int offset = 0;
    int thisBlockDeepth =
        getThisBlockDeepth(vmState->ASM_start, vmState->pc, &offset);
    while (1) {
        if ((methodPtr[0] == 'B') &&
            (methodPtr[1] - '0' == thisBlockDeepth + 1)) {
            class_defineMethod(self, data, (Method)methodPtr);
            break;
        }
        offset += gotoNextLine(methodPtr);
        methodPtr = vmState->pc + offset;
    }
    return NULL;
}

static Arg* VM_instruction_handler_RET(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    /* exit jmp signal */
    vmState->jmp = -999;
    Arg* returnArg = arg_copy(queue_popArg(vmState->q0));
    method_returnArg(vmState->locals->list, returnArg);
    return NULL;
}

static Arg* VM_instruction_handler_NEL(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    int offset = 0;
    int thisBlockDeepth =
        getThisBlockDeepth(vmState->ASM_start, vmState->pc, &offset);
    char __else[] = "__else0";
    __else[6] = '0' + thisBlockDeepth;
    if (0 == args_getInt(self->list, __else)) {
        /* set __else flag */
        vmState->jmp = fast_atoi(data);
    }
    return NULL;
}

static Arg* VM_instruction_handler_DEL(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    obj_removeArg(vmState->locals, data);
    return NULL;
}

static Arg* VM_instruction_handler_EST(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    Arg* arg = obj_getArg(vmState->locals, data);
    if (arg == NULL) {
        return arg_setInt(NULL, "", 0);
    }
    if (TYPE_NULL == arg_getType(arg)) {
        return arg_setInt(NULL, "", 0);
    }
    return arg_setInt(NULL, "", 1);
}

static Arg* VM_instruction_handler_BRK(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    /* break jmp signal */
    vmState->jmp = -998;
    return NULL;
}

static Arg* VM_instruction_handler_CTN(PikaObj* self,
                                       struct VM_State_t* vmState,
                                       char* data) {
    /* continue jmp signal */
    vmState->jmp = -997;
    return NULL;
}

const VM_instruct_handler VM_instruct_handler_table[__INSTRCUTION_CNT] = {

    #define __INS_TABLE
    #include "__instruction_table.cfg"   
};

static char* strs_getLine(Args* buffs, char* code) {
    int32_t lineSize = getLineSize(code);
    char* line = args_getBuff(buffs, lineSize + 1);
    __platform_memcpy(line, code, lineSize);
    line[lineSize + 1] = 0;
    return line;
}

static enum Instruct getInstruct(char* line) {
    if (0 == strncmp(line + 2, "OPT", 3)) {
        /* operator */
        return OPT;
    }
    if (0 == strncmp(line + 2, "REF", 3)) {
        /* reference */
        return REF;
    }
    if (0 == strncmp(line + 2, "NUM", 3)) {
        /* number */
        return NUM;
    }
    if (0 == strncmp(line + 2, "RUN", 3)) {
        /* run */
        return RUN;
    }
    if (0 == strncmp(line + 2, "STR", 3)) {
        /* string */
        return STR;
    }
    if (0 == strncmp(line + 2, "OUT", 3)) {
        /* out */
        return OUT;
    }
    if (0 == strncmp(line + 2, "JMP", 3)) {
        /* jump */
        return JMP;
    }
    if (0 == strncmp(line + 2, "JEZ", 3)) {
        /* jump equal zero */
        return JEZ;
    }
    if (0 == strncmp(line + 2, "DEF", 3)) {
        /* define */
        return DEF;
    }
    if (0 == strncmp(line + 2, "RET", 3)) {
        /* return */
        return RET;
    }
    if (0 == strncmp(line + 2, "NEL", 3)) {
        /* not else */
        return NEL;
    }
    if (0 == strncmp(line + 2, "DEL", 3)) {
        /* delete */
        return DEL;
    }
    if (0 == strncmp(line + 2, "EST", 3)) {
        /* is exist */
        return EST;
    }
    if (0 == strncmp(line + 2, "BRK", 3)) {
        /* break */
        return BRK;
    }
    if (0 == strncmp(line + 2, "CTN", 3)) {
        /* continue */
        return CTN;
    }
    return NON;
}

int32_t __clearInvokeQueues(VM_Parameters* locals) {
    for (char deepthChar = '0'; deepthChar < '9'; deepthChar++) {
        char deepth[2] = {0};
        deepth[0] = deepthChar;
        Queue* queue = (Queue*)args_getPtr(locals->list, deepth);
        if (NULL != queue) {
            args_deinit(queue);
            args_removeArg(locals->list, args_getArg(locals->list, deepth));
        }
    }
    return 0;
}

int32_t gotoNextLine(char* code) {
    int offset = 0;
    while (1) {
        if (code[offset] == '\n') {
            break;
        }
        offset++;
    }
    return offset + 1;
}

int32_t gotoLastLine(char* start, char* code) {
    int offset = -2;
    while (1) {
        char* codeNow = code + offset;
        if (codeNow == start) {
            offset--;
            break;
        }
        if (codeNow[0] == '\n') {
            break;
        }
        offset--;
    }
    return offset + 1;
}

int getThisBlockDeepth(char* start, char* code, int* offset) {
    int thisBlockDeepth = -1;
    char* codeNow = code + *offset;
    while (1) {
        *offset += gotoLastLine(start, codeNow);
        codeNow = code + *offset;
        if (codeNow[0] == 'B') {
            thisBlockDeepth = codeNow[1] - '0';
            break;
        }
    }
    return thisBlockDeepth;
}

int32_t asm_getAddrOffsetOfJUM(char* code) {
    int offset = 0;
    char* codeNow = code + offset;
    while (1) {
        offset += gotoNextLine(codeNow);
        codeNow = code + offset;
        if (0 == strncmp(codeNow, "0 JMP -1", 8)) {
            return offset;
        }
    }
    return 0;
}

int32_t getAddrOffsetFromJmp(char* start, char* code, int32_t jmp) {
    int offset = 0;
    int thisBlockDeepth = getThisBlockDeepth(start, code, &offset);
    char* codeNow = code + offset;
    int8_t blockNum = 0;
    if (jmp > 0) {
        offset = 0;
        codeNow = code + offset;
        while (1) {
            offset += gotoNextLine(codeNow);
            codeNow = code + offset;
            if (codeNow[0] == 'B') {
                uint8_t blockDeepth = codeNow[1] - '0';
                if (blockDeepth <= thisBlockDeepth) {
                    blockNum++;
                }
            }
            if (blockNum >= jmp) {
                break;
            }
        }
    }
    if (jmp < 0) {
        while (1) {
            offset += gotoLastLine(start, codeNow);
            codeNow = code + offset;
            if (codeNow[0] == 'B') {
                uint8_t blockDeepth = codeNow[1] - '0';
                if (blockDeepth == thisBlockDeepth) {
                    blockNum--;
                }
            }
            if (blockNum <= jmp) {
                break;
            }
        }
    }
    return offset;
}

int32_t pikaVM_runAsmLine(PikaObj* self,
                          VM_Parameters* locals,
                          VM_Parameters* globals,
                          char* pikaAsm,
                          int32_t lineAddr) {
    Args* buffs = New_strBuff();
    VM_State vmState = {
        .locals = locals,
        .globals = globals,
        .q0 = NULL,
        .q1 = NULL,
        .jmp = 0,
        .pc = pikaAsm + lineAddr,
        .ASM_start = pikaAsm,
    };
    char* line = strs_getLine(buffs, vmState.pc);
    int32_t nextAddr = lineAddr + strGetSize(line) + 1;
    enum Instruct instruct;
    char invokeDeepth0[2] = {0}, invokeDeepth1[2] = {0};
    char* data;
    Arg* resArg;

    /* Found new script Line, clear the queues*/
    if ('B' == line[0]) {
        args_setErrorCode(locals->list, 0);
        args_setSysOut(locals->list, (char*)"");
        __clearInvokeQueues(locals);
        goto nextLine;
    }
    invokeDeepth0[0] = line[0];
    invokeDeepth1[0] = line[0] + 1;
    instruct = getInstruct(line);
    data = line + 6;

    vmState.q0 = args_getPtr(locals->list, invokeDeepth0);
    vmState.q1 = args_getPtr(locals->list, invokeDeepth1);
    if (NULL == vmState.q0) {
        vmState.q0 = New_queue();
        args_setPtr(locals->list, invokeDeepth0, vmState.q0);
    }
    if (NULL == vmState.q1) {
        vmState.q1 = New_queue();
        args_setPtr(locals->list, invokeDeepth1, vmState.q1);
    }
    /* run instruct */
    resArg = VM_instruct_handler_table[instruct](self, &vmState, data);
    if (NULL != resArg) {
        queue_pushArg(vmState.q0, resArg);
    }
    goto nextLine;
nextLine:
    args_deinit(buffs);
    /* exit */
    if (-999 == vmState.jmp) {
        return -99999;
    }
    /* break or continue */
    if ((-998 == vmState.jmp) || (-997 == vmState.jmp)) {
        int32_t loop_end_addr = asm_getAddrOffsetOfJUM(vmState.pc);
        /* break */
        if (-998 == vmState.jmp) {
            loop_end_addr += gotoNextLine(vmState.pc + loop_end_addr);
            return lineAddr + loop_end_addr;
        }
        if (-997 == vmState.jmp) {
            loop_end_addr += gotoLastLine(pikaAsm, vmState.pc + loop_end_addr);
            return lineAddr + loop_end_addr;
        }
    }
    if (vmState.jmp != 0) {
        return lineAddr +
               getAddrOffsetFromJmp(pikaAsm, vmState.pc, vmState.jmp);
    }
    return nextAddr;
}

char* useFlashAsBuff(char* pikaAsm, Args* buffs) {
    /* not write flash when asm is old */
    if (strEqu(pikaAsm, __platform_load_pikaAsm())) {
        args_deinit(buffs);
        buffs = NULL;
        return __platform_load_pikaAsm();
    }
    /* write flash */
    if (0 == __platform_save_pikaAsm(pikaAsm)) {
        args_deinit(buffs);
        buffs = NULL;
        return __platform_load_pikaAsm();
    }
    return pikaAsm;
}

VM_Parameters* pikaVM_runAsmWithPars(PikaObj* self,
                                     VM_Parameters* locals,
                                     VM_Parameters* globals,
                                     char* pikaAsm) {
    int lineAddr = 0;
    int size = strGetSize(pikaAsm);
    args_setErrorCode(locals->list, 0);
    args_setSysOut(locals->list, (char*)"");
    while (lineAddr < size) {
        if (lineAddr == -99999) {
            break;
        }
        char* thisLine = pikaAsm + lineAddr;
        lineAddr = pikaVM_runAsmLine(self, locals, globals, pikaAsm, lineAddr);
        char* sysOut = args_getSysOut(locals->list);
        uint8_t errcode = args_getErrorCode(locals->list);
        if (!strEqu("", sysOut)) {
            __platform_printf("%s\r\n", sysOut);
        }
        if (0 != errcode) {
            Args* buffs = New_strBuff();
            char* onlyThisLine = strsGetFirstToken(buffs, thisLine, '\n');
            __platform_printf("[info] input commond: %s\r\n", onlyThisLine);
            args_deinit(buffs);
        }
    }
    __clearInvokeQueues(locals);

    return locals;
}

VM_Parameters* pikaVM_runAsm(PikaObj* self, char* pikaAsm) {
    return pikaVM_runAsmWithPars(self, self, self, pikaAsm);
}

VM_Parameters* pikaVM_run(PikaObj* self, char* multiLine) {
    Args* buffs = New_strBuff();
    VM_Parameters* globals = NULL;
    char* pikaAsm = Parser_multiLineToAsm(buffs, multiLine);
    if (NULL == pikaAsm) {
        __platform_printf("[error]: Syntax error.\r\n");
        globals = NULL;
        goto exit;
    }
    globals = pikaVM_runAsm(self, pikaAsm);
    goto exit;
exit:
    if (NULL != buffs) {
        args_deinit(buffs);
    }
    return globals;
}
