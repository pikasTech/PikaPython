/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include "PikaStdLib_SysObj.h"
#include "Device_LED.h"
#include "PikaStdLib_MemChecker.h"
#include "Device_Uart.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

PikaObj *New_PikaMain(Args *args){
    PikaObj *self = New_PikaStdLib_SysObj(args);
    obj_import(self, "Device_LED", New_Device_LED);
    obj_newObj(self, "led", "Device_LED");
    obj_import(self, "PikaStdLib_MemChecker", New_PikaStdLib_MemChecker);
    obj_newObj(self, "mem", "PikaStdLib_MemChecker");
    obj_import(self, "Device_Uart", New_Device_Uart);
    obj_newObj(self, "uart", "Device_Uart");
    return self;
}
