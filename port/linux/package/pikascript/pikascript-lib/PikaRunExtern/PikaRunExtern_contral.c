/*
  Author: lyon
  Tencent QQ: 645275593
*/
#include "PikaObj.h"
#include <stdarg.h>
#include "BaseObj.h"
#include "PikaBlock.h"
#include "PikaIf.h"
#include "PikaInvoke.h"
#include "PikaWhile.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"

int __runExtern_contral(PikaObj* self, char* cmd) {
    int isExit = 0;
    /* in block */
    if (NULL != obj_getArg(self, "_isInBlock")) {
        PikaObj* block = obj_getObj(self, "_block", 0);
        if (strIsStartWith(cmd, "    ")) {
            if (strEqu(block_getMode(block), "if")) {
                if_pushLine(block, cmd);
                isExit = 1;
                goto exit;
            }
            if (strEqu(block_getMode(block), "while")) {
                while_pushLine(block, cmd);
                isExit = 1;
                goto exit;
            }
            isExit = 1;
            goto exit;
        }
        /* the block is end */
        else {
            obj_removeArg(self, "_isInBlock");
            if (strEqu(block_getMode(block), "if")) {
                if_run(block);
            }
            if (strEqu(block_getMode(block), "while")) {
                while_run(block);
            }
            obj_removeArg(self, "_block");
            /* not finished */
        }
    }

    /* if block */
    if (strIsStartWith(cmd, "if ")) {
        obj_setInt(self, "_isInBlock", 1);
        obj_setObjWithoutClass(self, "_block", block_init);
        PikaObj* block = obj_getObj(self, "_block", 0);
        if_setAssert(block, cmd);
        /* this line processed ok */
        isExit = 1;
        goto exit;
    }

    /* while block */
    if (strIsStartWith(cmd, "while ")) {
        obj_setInt(self, "_isInBlock", 1);
        obj_setObjWithoutClass(self, "_block", block_init);
        PikaObj* block = obj_getObj(self, "_block", 0);
        while_setAssert(block, cmd);
        /* this line processed ok */
        isExit = 1;
        goto exit;
    }

exit:
    return isExit;
}