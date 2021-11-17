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
#include <string.h>
#include "BaseObj.h"
#include "PikaObj.h"
#include "PikaParser.h"
#include "dataQueue.h"
#include "dataQueueObj.h"
#include "dataStrs.h"

static int32_t getLineSize(char* str) {
    int i = 0;
    while (1) {
        if (str[i] == '\n') {
            return i;
        }
        i++;
    }
}
enum Instruct { NON, REF, RUN, STR, OUT, NUM, JMP, JEZ, OPT, DEF, RET };

static char* strs_getLine(Args* buffs, char* code) {
    int32_t lineSize = getLineSize(code);
    char* line = args_getBuff(buffs, lineSize + 1);
    memcpy(line, code, lineSize);
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
    return NON;
}

int fast_atoi(char* src) {
    const char* p = src;
    static const uint64_t a[][10] = {
        {0, 1e0, 2e0, 3e0, 4e0, 5e0, 6e0, 7e0, 8e0, 9e0},
        {0, 1e1, 2e1, 3e1, 4e1, 5e1, 6e1, 7e1, 8e1, 9e1},
        {0, 1e2, 2e2, 3e2, 4e2, 5e2, 6e2, 7e2, 8e2, 9e2},
        {0, 1e3, 2e3, 3e3, 4e3, 5e3, 6e3, 7e3, 8e3, 9e3},
        {0, 1e4, 2e4, 3e4, 4e4, 5e4, 6e4, 7e4, 8e4, 9e4},
        {0, 1e5, 2e5, 3e5, 4e5, 5e5, 6e5, 7e5, 8e5, 9e5},
        {0, 1e6, 2e6, 3e6, 4e6, 5e6, 6e6, 7e6, 8e6, 9e6},
        {0, 1e7, 2e7, 3e7, 4e7, 5e7, 6e7, 7e7, 8e7, 9e7},
        {0, 1e8, 2e8, 3e8, 4e8, 5e8, 6e8, 7e8, 8e8, 9e8},
        {0, 1e9, 2e9, 3e9, 4e9, 5e9, 6e9, 7e9, 8e9, 9e9},
    };
    uint16_t size = strGetSize(src);
    p = p + size - 1;
    if (*p) {
        int s = 0;
        const uint64_t* n = a[0];
        while (p != src) {
            s += n[(*p - '0')];
            n += 10;
            p--;
        }
        if (*p == '-') {
            return -s;
        }
        return s + n[(*p - '0')];
    }
    return 0;
}

Arg* pikaVM_runInstruct(PikaObj* self,
                        Parameters* locals,
                        Parameters* globals,
                        enum Instruct instruct,
                        char* data,
                        Queue* invokeQuene0,
                        Queue* invokeQuene1,
                        int32_t* jmp,
                        char* programConter) {
    if (instruct == NUM) {
        Arg* numArg = New_arg(NULL);
        if (strIsContain(data, '.')) {
            return arg_setFloat(numArg, "", atof(data));
        }
        return arg_setInt(numArg, "", fast_atoi(data));
    }
    if (instruct == STR) {
        Arg* strArg = New_arg(NULL);
        return arg_setStr(strArg, "", data);
    }
    if (instruct == OUT) {
        Arg* outArg = arg_copy(queue_popArg(invokeQuene0));
        obj_setArg(locals, data, outArg);
        arg_deinit(outArg);
        return NULL;
    }
    if (instruct == REF) {
        if (strEqu(data, (char*)"True")) {
            return arg_setInt(NULL, "", 1);
        }
        if (strEqu(data, (char*)"False")) {
            return arg_setInt(NULL, "", 0);
        }
        /* find in local list first */
        Arg* arg = arg_copy(obj_getArg(locals, data));
        if (NULL == arg) {
            /* find in global list second */
            arg = arg_copy(obj_getArg(globals, data));
        }
        return arg;
    }
    if (instruct == JMP) {
        *jmp = fast_atoi(data);
        return NULL;
    }
    if (instruct == RET) {
        *jmp = -999;
        Arg* returnArg = arg_copy(queue_popArg(invokeQuene0));
        method_returnArg(locals->list, returnArg);
        return NULL;
    }
    if (instruct == DEF) {
        char* methodPtr = programConter;
        int offset = 0;
        int thisBlockDeepth =
            getThisBlockDeepth(programConter - 3, programConter, &offset);
        while (1) {
            if ((methodPtr[0] == 'B') &&
                (methodPtr[1] - '0' == thisBlockDeepth + 1)) {
                class_defineMethod(self, data, (Method)methodPtr);
                break;
            }
            offset += gotoNextLine(methodPtr);
            methodPtr = programConter + offset;
        }
        return NULL;
    }
    if (instruct == JEZ) {
        Arg* assertArg = arg_copy(queue_popArg(invokeQuene0));
        int assert = arg_getInt(assertArg);
        arg_deinit(assertArg);
        if (0 == assert) {
            *jmp = fast_atoi(data);
        }
    }
    if (instruct == OPT) {
        Arg* outArg = NULL;
        Arg* arg1 = arg_copy(queue_popArg(invokeQuene1));
        Arg* arg2 = arg_copy(queue_popArg(invokeQuene1));
        ArgType type_arg1 = arg_getType(arg1);
        ArgType type_arg2 = arg_getType(arg2);
        if ((type_arg1 == TYPE_INT) && (type_arg2 == TYPE_INT)) {
            int num1 = arg_getInt(arg1);
            int num2 = arg_getInt(arg2);
            if (strEqu("+", data)) {
                outArg = arg_setInt(outArg, "", num1 + num2);
                goto OPT_exit;
            }
            if (strEqu("-", data)) {
                outArg = arg_setInt(outArg, "", num1 - num2);
                goto OPT_exit;
            }
            if (strEqu("*", data)) {
                outArg = arg_setInt(outArg, "", num1 * num2);
                goto OPT_exit;
            }
            if (strEqu("/", data)) {
                outArg = arg_setInt(outArg, "", num1 / num2);
                goto OPT_exit;
            }
            if (strEqu("<", data)) {
                outArg = arg_setInt(outArg, "", num1 < num2);
                goto OPT_exit;
            }
            if (strEqu(">", data)) {
                outArg = arg_setInt(outArg, "", num1 > num2);
                goto OPT_exit;
            }
            if (strEqu("==", data)) {
                outArg = arg_setInt(outArg, "", num1 == num2);
                goto OPT_exit;
            }
        }
        if ((type_arg1 == TYPE_FLOAT) && (type_arg2 == TYPE_INT)) {
            float num1 = arg_getFloat(arg1);
            int num2 = arg_getInt(arg2);
            if (strEqu("+", data)) {
                outArg = arg_setFloat(outArg, "", num1 + num2);
                goto OPT_exit;
            }
            if (strEqu("-", data)) {
                outArg = arg_setFloat(outArg, "", num1 - num2);
                goto OPT_exit;
            }
            if (strEqu("*", data)) {
                outArg = arg_setFloat(outArg, "", num1 * num2);
                goto OPT_exit;
            }
            if (strEqu("/", data)) {
                outArg = arg_setFloat(outArg, "", num1 / num2);
                goto OPT_exit;
            }
            if (strEqu("<", data)) {
                outArg = arg_setInt(outArg, "", num1 < num2);
                goto OPT_exit;
            }
            if (strEqu(">", data)) {
                outArg = arg_setInt(outArg, "", num1 > num2);
                goto OPT_exit;
            }
            if (strEqu("==", data)) {
                outArg = arg_setInt(outArg, "", num1 == num2);
                goto OPT_exit;
            }
        }
        if ((type_arg1 == TYPE_INT) && (type_arg2 == TYPE_FLOAT)) {
            int num1 = arg_getInt(arg1);
            float num2 = arg_getFloat(arg2);
            if (strEqu("+", data)) {
                outArg = arg_setFloat(outArg, "", num1 + num2);
                goto OPT_exit;
            }
            if (strEqu("-", data)) {
                outArg = arg_setFloat(outArg, "", num1 - num2);
                goto OPT_exit;
            }
            if (strEqu("*", data)) {
                outArg = arg_setFloat(outArg, "", num1 * num2);
                goto OPT_exit;
            }
            if (strEqu("/", data)) {
                outArg = arg_setFloat(outArg, "", num1 / num2);
                goto OPT_exit;
            }
            if (strEqu("<", data)) {
                outArg = arg_setInt(outArg, "", num1 < num2);
                goto OPT_exit;
            }
            if (strEqu(">", data)) {
                outArg = arg_setInt(outArg, "", num1 > num2);
                goto OPT_exit;
            }
            if (strEqu("==", data)) {
                outArg = arg_setInt(outArg, "", num1 == num2);
                goto OPT_exit;
            }
        }
        if ((type_arg1 == TYPE_FLOAT) && (type_arg2 == TYPE_FLOAT)) {
            float num1 = arg_getFloat(arg1);
            float num2 = arg_getFloat(arg2);
            if (strEqu("+", data)) {
                outArg = arg_setFloat(outArg, "", num1 + num2);
                goto OPT_exit;
            }
            if (strEqu("-", data)) {
                outArg = arg_setFloat(outArg, "", num1 - num2);
                goto OPT_exit;
            }
            if (strEqu("*", data)) {
                outArg = arg_setFloat(outArg, "", num1 * num2);
                goto OPT_exit;
            }
            if (strEqu("/", data)) {
                outArg = arg_setFloat(outArg, "", num1 / num2);
                goto OPT_exit;
            }
            if (strEqu("<", data)) {
                outArg = arg_setInt(outArg, "", num1 < num2);
                goto OPT_exit;
            }
            if (strEqu(">", data)) {
                outArg = arg_setInt(outArg, "", num1 > num2);
                goto OPT_exit;
            }
            if (strEqu("==", data)) {
                outArg = arg_setInt(outArg, "", num1 == num2);
                goto OPT_exit;
            }
        }
    OPT_exit:
        arg_deinit(arg1);
        arg_deinit(arg2);
        if (NULL != outArg) {
            return outArg;
        }
    }
    if (instruct == RUN) {
        Args* buffs = New_strBuff();
        Arg* returnArg = NULL;
        Parameters* subLocals = NULL;
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
            returnArg = arg_copy(queue_popArg(invokeQuene1));
            goto RUN_exit;
        }
        /* get method host obj */
        methodHostObj = obj_getObj(self, methodPath, 1);
        if (NULL == methodHostObj) {
            methodHostObj = obj_getObj(locals, methodPath, 1);
        }
        if (NULL == methodHostObj) {
            /* error, not found object */
            args_setErrorCode(locals->list, 1);
            args_setSysOut(locals->list, "[error] runner: object no found.");
            goto RUN_exit;
        }
        /* get method */
        method_arg = obj_getMethod(methodHostObj, methodPath);
        /* assert method*/
        if (NULL == method_arg) {
            /* error, method no found */
            args_setErrorCode(locals->list, 2);
            args_setSysOut(locals->list, "[error] runner: method no found.");
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
            args_setErrorCode(locals->list, 3);
            args_setSysOut(locals->list, "[error] runner: type list no found.");
            goto RUN_exit;
        }

        subLocals = New_PikaObj();
        while (1) {
            Arg* methodArg = arg_copy(queue_popArg(invokeQuene1));
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
            subLocals = pikaVM_runAsmWithPars(methodHostObj, subLocals, globals,
                                              methodCode);
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
            args_setSysOut(locals->list, sysOut);
        }
        /* transfer errCode */
        if (0 != obj_getErrorCode(methodHostObj)) {
            /* method error */
            args_setErrorCode(locals->list, 6);
        }

        goto RUN_exit;
    RUN_exit:
        if (NULL != subLocals) {
            obj_deinit(subLocals);
        }
        args_deinit(buffs);
        return returnArg;
    }
    return NULL;
}

int32_t __clearInvokeQueues(Parameters* locals) {
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
                          Parameters* locals,
                          Parameters* globals,
                          char* pikaAsm,
                          int32_t lineAddr) {
    Args* buffs = New_strBuff();
    char* programCounter = pikaAsm + lineAddr;
    char* line = strs_getLine(buffs, programCounter);
    int32_t nextAddr = lineAddr + strGetSize(line) + 1;
    int32_t jmp = 0;
    enum Instruct instruct;
    char invokeDeepth0[2] = {0}, invokeDeepth1[2] = {0};
    char* data;
    Queue* invokeQuene0;
    Queue* invokeQuene1;
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

    invokeQuene0 = args_getPtr(locals->list, invokeDeepth0);
    invokeQuene1 = args_getPtr(locals->list, invokeDeepth1);
    if (NULL == invokeQuene0) {
        invokeQuene0 = New_queue();
        args_setPtr(locals->list, invokeDeepth0, invokeQuene0);
    }
    if (NULL == invokeQuene1) {
        invokeQuene1 = New_queue();
        args_setPtr(locals->list, invokeDeepth1, invokeQuene1);
    }
    resArg =
        pikaVM_runInstruct(self, locals, globals, instruct, data, invokeQuene0,
                           invokeQuene1, &jmp, programCounter);
    if (NULL != resArg) {
        queue_pushArg(invokeQuene0, resArg);
    }
    goto nextLine;
nextLine:
    args_deinit(buffs);
    if (-999 == jmp) {
        return -99999;
    }
    if (jmp != 0) {
        return lineAddr + getAddrOffsetFromJmp(pikaAsm, programCounter, jmp);
    }
    return nextAddr;
}

char* useFlashAsBuff(char* pikaAsm, Args* buffs) {
    /* not write flash when asm is old */
    if (strEqu(pikaAsm, __platformLoadPikaAsm())) {
        args_deinit(buffs);
        buffs = NULL;
        return __platformLoadPikaAsm();
    }
    /* write flash */
    if (0 == __platformSavePikaAsm(pikaAsm)) {
        args_deinit(buffs);
        buffs = NULL;
        return __platformLoadPikaAsm();
    }
    return pikaAsm;
}

Parameters* pikaVM_runAsmWithPars(PikaObj* self,
                                  Parameters* locals,
                                  Parameters* globals,
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
            __platformPrintf("%s\r\n", sysOut);
        }
        if (0 != errcode) {
            Args* buffs = New_strBuff();
            char* onlyThisLine = strsGetFirstToken(buffs, thisLine, '\n');
            __platformPrintf("[info] input commond: %s\r\n", onlyThisLine);
            args_deinit(buffs);
        }
    }
    __clearInvokeQueues(locals);

    return locals;
}

Parameters* pikaVM_runAsm(PikaObj* self, char* pikaAsm) {
    Parameters* globals = New_PikaObj();
    globals = pikaVM_runAsmWithPars(self, globals, globals, pikaAsm);
    return globals;
}

Parameters* pikaVM_run(PikaObj* self, char* multiLine) {
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, multiLine);
    Parameters* globals = pikaVM_runAsm(self, pikaAsm);
    if (NULL != buffs) {
        args_deinit(buffs);
    }
    return globals;
}
