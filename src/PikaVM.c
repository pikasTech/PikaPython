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
enum Instruct { NON, REF, RUN, STR, OUT, NUM };

static char* strs_getLine(Args* buffs, char* code) {
    int32_t lineSize = getLineSize(code);
    char* line = args_getBuff(buffs, lineSize + 1);
    memcpy(line, code, lineSize);
    line[lineSize + 1] = 0;
    return line;
}

static enum Instruct getInstruct(char* line) {
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
    return NON;
}

static void* getMethodPtr(PikaObj* methodHost, char* methodName) {
    Args* buffs = New_strBuff();
    char* methodPtrPath = strsAppend(buffs, "[mp]", methodName);
    void* res = obj_getPtr(methodHost, methodPtrPath);
    args_deinit(buffs);
    return res;
}

static char* getMethodDeclearation(PikaObj* obj, char* methodName) {
    Args* buffs = New_strBuff();
    char* methodDeclearationPath = strsAppend(buffs, "[md]", methodName);
    char* res = obj_getStr(obj, methodDeclearationPath);
    args_deinit(buffs);
    return res;
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
                           enum Instruct instruct,
                           char* data,
                           Queue* q0,
                           Queue* q1,
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
        Arg* outArg = arg_copy(queue_popArg(q0));
        int outInt = arg_getInt(outArg);
        outArg = arg_setName(outArg, data);
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
        return arg_copy(obj_getArg(self, data));
    }
    if (instruct == RUN) {
        Args* buffs = New_strBuff();
        Arg* returnArg = NULL;
        Args* methodArgs = NULL;
        char* methodPath = data;
        PikaObj* methodHostObj = obj_getObj(self, methodPath, 1);
        if (NULL == methodHostObj) {
            /* error, not found object */
            args_setErrorCode(sysRes, 1);
            args_setSysOut(sysRes, "[error] runner: object no found.");
            goto RUN_exit;
        }
        char* methodName = strsGetLastToken(buffs, methodPath, '.');
        void* classPtr = obj_getPtr(methodHostObj, "_clsptr");
        char* methodHostClassName =
            strsAppend(buffs, "classObj-", obj_getName(methodHostObj));
        PikaObj* methodHostClass = obj_getClassObjByNewFun(
            methodHostObj, methodHostClassName, classPtr);
        /* get method Ptr */
        void (*methodPtr)(PikaObj * self, Args * args) =
            getMethodPtr(methodHostClass, methodName);
        char* methodDecInClass =
            getMethodDeclearation(methodHostClass, methodName);

        /* assert method*/
        if ((NULL == methodDecInClass) || (NULL == methodPtr)) {
            /* error, method no found */
            args_setErrorCode(sysRes, 2);
            args_setSysOut(sysRes, "[error] runner: method no found.");
            goto RUN_exit;
        }

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
            Arg* methodArg = arg_copy(queue_popArg(q1));
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

static int32_t clearQueues(PikaObj* self) {
    for (char deepthChar = '0'; deepthChar < '9'; deepthChar++) {
        char deepth[2] = {0};
        deepth[0] = deepthChar;
        Queue* queue = (Queue*)obj_getPtr(self, deepth);
        if (NULL != queue) {
            args_deinit(queue);
            obj_removeArg(self, deepth);
        }
    }
}

int32_t pikaVM_runAsmLine(PikaObj* self,
                          char* pikaAsm,
                          int32_t lineAddr,
                          Args* sysRes) {
    Args* buffs = New_strBuff();
    char* code = pikaAsm + lineAddr;
    char* line = strs_getLine(buffs, code);
    int32_t nextAddr = lineAddr + strGetSize(line) + 1;
    /* Found new script Line, clear the queues*/
    if ('B' == line[0]) {
        clearQueues(self);
        goto nextLine;
    }

    char d0[2] = {0}, d1[2] = {0};
    d0[0] = line[0];
    d1[0] = line[0] + 1;
    enum Instruct instruct = getInstruct(line);
    char* data = line + 6;

    Queue* q0 = obj_getPtr(self, d0);
    Queue* q1 = obj_getPtr(self, d1);
    if (NULL == q0) {
        q0 = New_queue();
        obj_setPtr(self, d0, q0);
    }
    if (NULL == q1) {
        q1 = New_queue();
        obj_setPtr(self, d1, q1);
    }

    Arg* resArg = pikaVM_runAsmInstruct(self, instruct, data, q0, q1, sysRes);
    if (NULL != resArg) {
        queue_pushArg(q0, resArg);
    }

    goto nextLine;
nextLine:
    args_deinit(buffs);
    return nextAddr;
}

Args* pikaVM_run(PikaObj* self, char* pyLine) {
    Args* buffs = New_strBuff();
    Args* sysRes = pikaVM_runAsm(self, pikaParseToAsm(buffs, pyLine));
    args_deinit(buffs);
    return sysRes;
}

Args* pikaVM_runAsm(PikaObj* self, char* pikaAsm) {
    int lineAddr = 0;
    int size = strGetSize(pikaAsm);
    Args* sysRes = New_args(NULL);
    args_setErrorCode(sysRes, 0);
    args_setSysOut(sysRes, (char*)"");
    while (lineAddr < size) {
        lineAddr = pikaVM_runAsmLine(self, pikaAsm, lineAddr, sysRes);
    }
    clearQueues(self);

    return sysRes;
}