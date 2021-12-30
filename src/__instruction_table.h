/*
 * This file is part of the PikaScript project.
 * http://github.com/pikastech/pikascript
 *
 * MIT License
 *
 * Copyright (c) 2021 GorgonMeducer ?? embedded_zhuoran@hotmail.com
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

#include "__instruction_def.h"
 
def_ins(NON, {
    return NULL;
})

def_ins(REF, {
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
})

def_ins(RUN, {
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
})

def_ins(STR, {
    Arg* strArg = New_arg(NULL);
    return arg_setStr(strArg, "", data);
})

def_ins(OUT, {
    Arg* outArg = arg_copy(queue_popArg(vmState->q0));
    obj_setArg(vmState->locals, data, outArg);
    arg_deinit(outArg);
    return NULL;
})

def_ins(NUM, {
    Arg* numArg = New_arg(NULL);
    if (strIsContain(data, '.')) {
        return arg_setFloat(numArg, "", atof(data));
    }
    return arg_setInt(numArg, "", fast_atoi(data));
})

def_ins(JMP, {
    vmState->jmp = fast_atoi(data);
    return NULL;
})

def_ins(JEZ, {
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
})

def_ins(OPT, {
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
})

def_ins(DEF, {
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
})

def_ins(RET, {
    /* exit jmp signal */
    vmState->jmp = -999;
    Arg* returnArg = arg_copy(queue_popArg(vmState->q0));
    method_returnArg(vmState->locals->list, returnArg);
    return NULL;
})

def_ins(NEL, {
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
})

def_ins(DEL, {
    obj_removeArg(vmState->locals, data);
    return NULL;
})

def_ins(EST, {
    Arg* arg = obj_getArg(vmState->locals, data);
    if (arg == NULL) {
        return arg_setInt(NULL, "", 0);
    }
    if (TYPE_NULL == arg_getType(arg)) {
        return arg_setInt(NULL, "", 0);
    }
    return arg_setInt(NULL, "", 1);
})


def_ins(BRK, {
    /* break jmp signal */
    vmState->jmp = -998;
    return NULL;
})


def_ins(CTN, {
    /* continue jmp signal */
    vmState->jmp = -997;
    return NULL;
})