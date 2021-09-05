#include "PikaObj.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"
#include "PikaBlock.h"
#include <stdarg.h>

void while_setAssert(PikaObj *self, char *line)
{
    Args *buffs = New_strBuff();
    char *assert = strsRemovePrefix(buffs, line, "while ");
    assert = strsGetFirstToken(buffs, assert, ':');
    block_setAssert(self, assert);
    block_setMode(self, "while");
    args_deinit(buffs);
}

void while_pushLine(PikaObj *self, char *line)
{
    Args *buffs = New_strBuff();
    char *bodyLine = strsRemovePrefix(buffs, line, "    ");
    block_pushLine(self, bodyLine);
    goto exit;
exit:
    args_deinit(buffs);
    return;
}

void while_run(PikaObj *self)
{
    Args *buffs = New_strBuff();
    char *body = strsCopy(buffs, block_getBody(self));
    uint16_t lineSize = block_getLineSize(self);
    while (block_checkAssert(self))
    {
        PikaObj *host = obj_getContext(self);
        for (int i = 0; i < lineSize; i++)
        {
            char *line = block_popLine(self);
            obj_run(host, line);
        }
        block_setBody(self, body);
    }
exit:
    args_deinit(buffs);
}
