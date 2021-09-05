/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include "SysObj.h"
#include "Block.h"
#include "MemoryChecker.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

MimiObj *New_PikaMain(Args *args){
    MimiObj *self = New_SysObj(args);
    obj_import(self, "Block", New_Block);
    obj_newObj(self, "block", "Block");
    obj_import(self, "MemoryChecker", New_MemoryChecker);
    obj_newObj(self, "mem", "MemoryChecker");
    return self;
}
