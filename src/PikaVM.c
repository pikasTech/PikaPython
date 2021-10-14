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

static char* strs_getLine(Args* buffs, char* code) {
    int32_t lineSize = getLineSize(code);
    char* line = args_getBuff(buffs, lineSize + 1);
    memcpy(line, code, lineSize);
    line[lineSize + 1] = 0;
    return line;
}

int32_t pikaVM_run(PikaObj* self, char* pikaAsm, int32_t lineAddr) {
    Args* buffs = New_strBuff();
    int32_t nextAddr = lineAddr;
    char* code = pikaAsm + lineAddr;
    char* line = strs_getLine(buffs, code);

    char deepth = line[0];
    enum { NON, REF, RUN, STR, OUT, NUM } instruct = NON;

    goto nextLine;
nextLine:
    args_deinit(buffs);
    return lineAddr + strGetSize(line) + 1;
}