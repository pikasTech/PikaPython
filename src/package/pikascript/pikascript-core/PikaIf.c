/*
  Author: lyon
  Tencent QQ: 645275593
*/

#include "PikaIf.h"
#include <stdarg.h>
#include "PikaBlock.h"
#include "TinyObj.h"
#include "dataStrs.h"

void if_setAssert(PikaObj* self, char* line) {
    Args* buffs = New_strBuff();
    char* assert = strsRemovePrefix(buffs, line, "if ");
    assert = strsGetFirstToken(buffs, assert, ':');
    block_setAssert(self, assert);
    block_setMode(self, "if");
    args_deinit(buffs);
}

void if_pushLine(PikaObj* self, char* line) {
    Args* buffs = New_strBuff();
    char* bodyLine = strsRemovePrefix(buffs, line, "    ");
    block_pushLine(self, bodyLine);
    goto exit;
exit:
    args_deinit(buffs);
    return;
}

void if_run(PikaObj* self) {
    if (block_checkAssert(self)) {
        PikaObj* host = obj_getContext(self);
        while (0 != block_getLineSize(self)) {
            char* line = block_popLine(self);
            obj_run(host, line);
        }
    }
}
