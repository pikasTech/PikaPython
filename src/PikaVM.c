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

Arg* pikaVM_getArg(PikaObj* self, enum Instruct instruct, char* data) {
    Arg* res = New_arg(NULL);
    if (instruct == NUM) {
        if (strIsContain(data, '.')) {
            return arg_setFloat(res, "", atof(data));
        }
        return arg_setInt(res, "", atoi(data));
    }
    return NULL;
}

int32_t pikaVM_run(PikaObj* self, char* pikaAsm, int32_t lineAddr) {
    Args* buffs = New_strBuff();
    char* code = pikaAsm + lineAddr;
    char* line = strs_getLine(buffs, code);
    int32_t nextAddr = lineAddr + strGetSize(line) + 1;

    char deepth[2] = {0};
    deepth[0] = line[0];
    enum Instruct instruct = getInstruct(line);
    char* data = line + 6;

    Arg* res = pikaVM_getArg(self, instruct, data);
    Queue* queue = obj_getPtr(self, deepth);
    if (NULL == queue) {
        queue = New_queue();
        obj_setPtr(self, deepth, queue);
    }
    queue_pushArg(queue, res);
    goto nextLine;
nextLine:
    args_deinit(buffs);
    return nextAddr;
}

int32_t pikaVM_runAsm(PikaObj* self, char* pikaAsm) {
    int lineAddr = 0;
    int size = strGetSize(pikaAsm);
    while (lineAddr < size) {
        lineAddr = pikaVM_run(self, pikaAsm, lineAddr);
    }

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