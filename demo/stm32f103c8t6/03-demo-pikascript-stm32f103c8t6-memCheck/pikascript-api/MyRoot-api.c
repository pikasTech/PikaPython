/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "MyRoot.h"
#include "BaseObj.h"
#include "LED.h"
#include "MemoryChecker.h"
#include "Uart.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

PikaObj *New_MyRoot(Args *args){
    PikaObj *self = New_BaseObj(args);
    obj_import(self, "LED", New_LED);
    obj_newObj(self, "led", "LED");
    obj_import(self, "MemoryChecker", New_MemoryChecker);
    obj_newObj(self, "mem", "MemoryChecker");
    obj_import(self, "Uart", New_Uart);
    obj_newObj(self, "uart", "Uart");
    return self;
}
