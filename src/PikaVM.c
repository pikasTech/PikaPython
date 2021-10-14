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

int32_t pikaVM_run(PikaObj* self, char* pikaAsm, int32_t lineAddr) {
    Args* buffs = New_strBuff();
    int32_t size = getLineSize(pikaAsm + lineAddr);
    char* line = pikaAsm + lineAddr;
    return lineAddr;
}