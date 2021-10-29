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
enum Instruct { NON, REF, RUN, STR, OUT, NUM, JMP, JEZ, OPT, ARG, DEF, RET };

static char* strs_getLine(Args* buffs, char* code) {
    int32_t lineSize = getLineSize(code);
    char* line = args_getBuff(buffs, lineSize + 1);
    memcpy(line, code, lineSize);
    line[lineSize + 1] = 0;
    return line;
}

static enum Instruct getInstruct(char* line) {
    if (0 == strncmp(line + 2, "OPT", 3)) {
        return OPT;
    }
    if (0 == strncmp(line + 2, "REF", 3)) {
        return REF;
    }
    if (0 == strncmp(line + 2, "NUM", 3)) {
        return NUM;
    }
    if (0 == strncmp(line + 2, "RUN", 3)) {
        return RUN;
    }
    if (0 == strncmp(line + 2, "STR", 3)) {
        return STR;
    }
    if (0 == strncmp(line + 2, "OUT", 3)) {
        return OUT;
    }
    if (0 == strncmp(line + 2, "JMP", 3)) {
        return JMP;
    }
    if (0 == strncmp(line + 2, "JEZ", 3)) {
        return JEZ;
    }
    if (0 == strncmp(line + 2, "ARG", 3)) {
        return ARG;
    }
    if (0 == strncmp(line + 2, "DEF", 3)) {
        return DEF;
    }
    if (0 == strncmp(line + 2, "RET", 3)) {
        return RET;
    }
    return NON;
}

int fast_atoi(char* src) {
    const char* p = src;
    static const uint64_t a[][10] = {
        0,   1e0, 2e0, 3e0, 4e0, 5e0, 6e0, 7e0, 8e0, 9e0, 0,   1e1, 2e1,
        3e1, 4e1, 5e1, 6e1, 7e1, 8e1, 9e1, 0,   1e2, 2e2, 3e2, 4e2, 5e2,
        6e2, 7e2, 8e2, 9e2, 0,   1e3, 2e3, 3e3, 4e3, 5e3, 6e3, 7e3, 8e3,
        9e3, 0,   1e4, 2e4, 3e4, 4e4, 5e4, 6e4, 7e4, 8e4, 9e4, 0,   1e5,
        2e5, 3e5, 4e5, 5e5, 6e5, 7e5, 8e5, 9e5, 0,   1e6, 2e6, 3e6, 4e6,
        5e6, 6e6, 7e6, 8e6, 9e6, 0,   1e7, 2e7, 3e7, 4e7, 5e7, 6e7, 7e7,
        8e7, 9e7, 0,   1e8, 2e8, 3e8, 4e8, 5e8, 6e8, 7e8, 8e8, 9e8, 0,
        1e9, 2e9, 3e9, 4e9, 5e9, 6e9, 7e9, 8e9, 9e9,
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

Arg* pikaVM_runAsmInstruct(PikaObj* self,
                           Args* localArgs,
                           enum Instruct instruct,
                           char* data,
                           Queue* invokeQuene0,
                           Queue* invokeQuene1,
                           int32_t* jmp,
                           Args* sysRes) {
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
        Args* buffs = New_strBuff();
        char* argName = strsGetLastToken(buffs, data, '.');
        outArg = arg_setName(outArg, argName);
        args_deinit(buffs);
        obj_setArg(self, data, outArg);
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
        Arg* arg = arg_copy(obj_getArg(self, data));
        return arg;
    }
    if (instruct == ARG) {
        Arg* arg = arg_copy(args_getArg(localArgs, data));
        return arg;
    }
    if (instruct == JMP) {
        *jmp = fast_atoi(data);
        return NULL;
    }
    if (instruct == RET) {
        *jmp = -999;
        Arg* returnArg = arg_copy(queue_popArg(invokeQuene0));
        return returnArg;
    }
    if (instruct == DEF) {
        class_defineMethod(self, data, NULL);
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
        char* operator= data;
        Arg* outArg = NULL;
        Arg* arg1 = arg_copy(queue_popArg(invokeQuene1));
        Arg* arg2 = arg_copy(queue_popArg(invokeQuene1));
        if (strEqu(arg_getType(arg1), "int") &&
            strEqu(arg_getType(arg2), "int")) {
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
        if (strEqu(arg_getType(arg1), "float") &&
            strEqu(arg_getType(arg2), "int")) {
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
        if (strEqu(arg_getType(arg1), "int") &&
            strEqu(arg_getType(arg2), "float")) {
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
        if (strEqu(arg_getType(arg1), "float") &&
            strEqu(arg_getType(arg2), "float")) {
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
        Args* methodArgs = NULL;
        char* methodPath = data;
        PikaObj* methodHostClass = NULL;
        /* return arg directly */
        if (strEqu(data, "")) {
            returnArg = arg_copy(queue_popArg(invokeQuene1));
            goto RUN_exit;
        }
        /* get method host obj */
        PikaObj* methodHostObj = obj_getObj(self, methodPath, 1);
        if (NULL == methodHostObj) {
            /* error, not found object */
            args_setErrorCode(sysRes, 1);
            args_setSysOut(sysRes, "[error] runner: object no found.");
            goto RUN_exit;
        }
        methodHostClass = obj_getClassObj(methodHostObj);
        char* methodName = strsGetLastToken(buffs, methodPath, '.');
        Arg* method = obj_getArg(methodHostClass, methodName);
        /* assert method*/
        if (NULL == method) {
            /* error, method no found */
            args_setErrorCode(sysRes, 2);
            args_setSysOut(sysRes, "[error] runner: method no found.");
            goto RUN_exit;
        }
        /* get method Ptr */
        void (*methodPtr)(PikaObj * self, Args * args) = arg_getPtr(method);
        char* methodDecInClass = arg_getType(method);

        char* methodDec = strsCopy(buffs, methodDecInClass);
        /* free method host class to save memory */
        obj_deinit(methodHostClass);
        methodHostClass = NULL;
        /* get type list */
        char* typeList = strsCut(buffs, methodDec, '(', ')');

        if (typeList == NULL) {
            /* typeList no found */
            args_setErrorCode(sysRes, 3);
            args_setSysOut(sysRes, "[error] runner: type list no found.");
            goto RUN_exit;
        }

        methodArgs = New_args(NULL);
        while (1) {
            Arg* methodArg = arg_copy(queue_popArg(invokeQuene1));
            if (NULL == methodArg) {
                break;
            }
            char* argDef = strsPopToken(buffs, typeList, ',');
            char* argName = strsGetFirstToken(buffs, argDef, ':');
            methodArg = arg_setName(methodArg, argName);
            args_setArg(methodArgs, methodArg);
        }

        obj_setErrorCode(methodHostObj, 0);
        obj_setSysOut(methodHostObj, "");

        /* run method */
        methodPtr(methodHostObj, methodArgs);

        /* transfer sysOut */
        char* sysOut = obj_getSysOut(methodHostObj);
        if (NULL != sysOut) {
            args_setSysOut(sysRes, sysOut);
        }
        /* transfer errCode */
        if (0 != obj_getErrorCode(methodHostObj)) {
            /* method error */
            args_setErrorCode(sysRes, 6);
        }

        returnArg = arg_copy(args_getArg(methodArgs, (char*)"return"));
        goto RUN_exit;
    RUN_exit:
        if (NULL != methodArgs) {
            args_deinit(methodArgs);
        }
        if (NULL != methodHostClass) {
            obj_deinit(methodHostClass);
        }
        args_deinit(buffs);
        return returnArg;
    }
    return NULL;
}

int32_t __clearInvokeQueues(PikaObj* self) {
    for (char deepthChar = '0'; deepthChar < '9'; deepthChar++) {
        char deepth[2] = {0};
        deepth[0] = deepthChar;
        Queue* queue = (Queue*)obj_getPtr(self, deepth);
        if (NULL != queue) {
            args_deinit(queue);
            obj_removeArg(self, deepth);
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

int32_t getAddrOffsetFromJmp(char* start, char* code, int32_t jmp) {
    int offset = 0;
    int thisBlockDeepth = -1;
    char* codeNow = code + offset;
    while (1) {
        offset += gotoLastLine(start, codeNow);
        codeNow = code + offset;
        if (codeNow[0] == 'B') {
            thisBlockDeepth = codeNow[1] - '0';
            break;
        }
    }
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

int32_t pikaVM_runAsmLineWithArgs(PikaObj* self,
                                  Args* localArgs,
                                  char* pikaAsm,
                                  int32_t lineAddr,
                                  Args* sysRes) {
    Args* buffs = New_strBuff();
    char* code = pikaAsm + lineAddr;
    char* line = strs_getLine(buffs, code);
    int32_t nextAddr = lineAddr + strGetSize(line) + 1;
    int32_t jmp = 0;
    /* Found new script Line, clear the queues*/
    if ('B' == line[0]) {
        args_setErrorCode(sysRes, 0);
        args_setSysOut(sysRes, (char*)"");
        __clearInvokeQueues(self);
        uint8_t blockDeepth = line[1] - '0';
        goto nextLine;
    }

    char invokeDeepth0[2] = {0}, invokeDeepth1[2] = {0};
    invokeDeepth0[0] = line[0];
    invokeDeepth1[0] = line[0] + 1;
    enum Instruct instruct = getInstruct(line);
    char* data = line + 6;

    Queue* invokeQuene0 = obj_getPtr(self, invokeDeepth0);
    Queue* invokeQuene1 = obj_getPtr(self, invokeDeepth1);
    if (NULL == invokeQuene0) {
        invokeQuene0 = New_queue();
        obj_setPtr(self, invokeDeepth0, invokeQuene0);
    }
    if (NULL == invokeQuene1) {
        invokeQuene1 = New_queue();
        obj_setPtr(self, invokeDeepth1, invokeQuene1);
    }

    Arg* resArg =
        pikaVM_runAsmInstruct(self, localArgs, instruct, data, invokeQuene0,
                              invokeQuene1, &jmp, sysRes);
    if (NULL != resArg) {
        queue_pushArg(invokeQuene0, resArg);
    }

    goto nextLine;
nextLine:
    args_deinit(buffs);
    if (jmp != 0) {
        return lineAddr + getAddrOffsetFromJmp(pikaAsm, code, jmp);
    }
    return nextAddr;
}

int32_t pikaVM_runAsmLine(PikaObj* self,
                          char* pikaAsm,
                          int32_t lineAddr,
                          Args* sysRes) {
    return pikaVM_runAsmLineWithArgs(self, NULL, pikaAsm, lineAddr, sysRes);
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

Args* pikaVM_runAsmWithArgs(PikaObj* self, Args* localArgs, char* pikaAsm) {
    int lineAddr = 0;
    int size = strGetSize(pikaAsm);
    Args* sysRes = New_args(NULL);
    args_setErrorCode(sysRes, 0);
    args_setSysOut(sysRes, (char*)"");
    while (lineAddr < size) {
        char* thisLine = pikaAsm + lineAddr;
        lineAddr = pikaVM_runAsmLineWithArgs(self, localArgs, pikaAsm, lineAddr,
                                             sysRes);
        char* sysOut = args_getSysOut(sysRes);
        uint8_t errcode = args_getErrorCode(sysRes);
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
    __clearInvokeQueues(self);

    return sysRes;
}

Args* pikaVM_runAsm(PikaObj* self, char* pikaAsm) {
    return pikaVM_runAsmWithArgs(self, NULL, pikaAsm);
}

Args* pikaVM_run(PikaObj* self, char* multiLine) {
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, multiLine);
    uint32_t asm_size = strGetSize(pikaAsm);
    Args* sysRes = pikaVM_runAsm(self, pikaAsm);
    if (NULL != buffs) {
        args_deinit(buffs);
    }
    return sysRes;
}
