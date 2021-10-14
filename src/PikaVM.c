#include "PikaVM.h"
#include <string.h>
#include "PikaObj.h"
#include "PikaParser.h"
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

int32_t pikaVM_run(PikaObj* self, char* pikaAsm, int32_t lineAddr) {
    Args* buffs = New_strBuff();
    char* code = pikaAsm + lineAddr;
    char* line = strs_getLine(buffs, code);
    int32_t nextAddr = lineAddr + strGetSize(line) + 1;

    char deepth = line[0];
    enum Instruct instruct = getInstruct(line);

    goto nextLine;
nextLine:
    args_deinit(buffs);
    return nextAddr;
}