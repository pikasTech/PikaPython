/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include "BaseObj.h"
#include "Box.h"
#include "MemoryChecker.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

MimiObj *New_PikaMain(Args *args){
    MimiObj *self = New_BaseObj(args);
    obj_import(self, "Box", New_Box);
    obj_newObj(self, "box", "Box");
    obj_import(self, "MemoryChecker", New_MemoryChecker);
    obj_newObj(self, "mem", "MemoryChecker");
    return self;
}
