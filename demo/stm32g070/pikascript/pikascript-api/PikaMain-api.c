/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include "PikaStdLib_SysObj.h"
#include "STM32_GPIO.h"
#include "PikaStdLib_MemChecker.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

PikaObj *New_PikaMain(Args *args){
    PikaObj *self = New_PikaStdLib_SysObj(args);
    obj_import(self, "STM32_GPIO", New_STM32_GPIO);
    obj_newObj(self, "io1", "STM32_GPIO");
    obj_import(self, "PikaStdLib_MemChecker", New_PikaStdLib_MemChecker);
    obj_newObj(self, "mem", "PikaStdLib_MemChecker");
    return self;
}
