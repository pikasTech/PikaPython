/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "MyRoot.h"
#include "SysObj.h"
#include "LED.h"
#include "MemoryChecker.h"
#include "Uart.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

PikaObj *New_MyRoot(Args *args){
    PikaObj *self = New_SysObj(args);
    obj_import(self, "LED", New_LED);
    obj_import(self, "MemoryChecker", New_MemoryChecker);
    obj_import(self, "Uart", New_Uart);
    return self;
}
