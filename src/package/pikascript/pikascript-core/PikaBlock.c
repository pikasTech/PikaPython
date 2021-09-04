#include "PikaObj.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"
#include <stdarg.h>
#include "PikaBlock.h"
#include "TinyObj.h"

void block_deinit(PikaObj *self)
{
    obj_deinit(self);
}

PikaObj *block_init()
{
    PikaObj *self = New_TinyObj(NULL);
    obj_setStr(self, "mode", "");
    obj_setStr(self, "assert", "");
    obj_setStr(self, "body", "");
    obj_setStr(self, "lineNow", "");
    return self;
}

uint8_t block_checkAssert(PikaObj *self)
{
    Args *buffs = New_strBuff();
    char *assert = block_getAssert(self);
    obj_run(self, strsFormat(buffs, 32, "_res = %s", assert));
    int res = obj_getInt(self, "_res");
    args_deinit(buffs);
    return res;
}

void block_setAssert(PikaObj *self, char *assert)
{
    obj_setStr(self, "assert", assert);
}

char *block_getAssert(PikaObj *self)
{
    return obj_getStr(self, "assert");
}

void block_setMode(PikaObj *self, char *mode)
{
    obj_setStr(self, "mode", mode);
}

char *block_getMode(PikaObj *self)
{
    obj_getStr(self, "mode");
}

void block_pushLine(PikaObj *self, char *line)
{
    Args *buffs = New_strBuff();
    char *body = obj_getStr(self, "body");
    body = strsAppend(buffs, body, line);
    body = strsAppend(buffs, body, "\n");
    obj_setStr(self, "body", body);
    args_deinit(buffs);
}

char *block_popLine(PikaObj *self)
{
    Args *buffs = New_strBuff();
    char *body = obj_getStr(self, "body");
    char *line = strsPopToken(buffs, body, '\n');

    obj_setStr(self, "body", body);
    obj_setStr(self, "lineNow", line);
    args_deinit(buffs);
    return obj_getStr(self, "lineNow");
}