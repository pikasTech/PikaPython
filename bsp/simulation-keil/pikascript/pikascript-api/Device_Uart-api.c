/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "Device_Uart.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void Device_Uart_printNameMethod(PikaObj *self, Args *args){
    Device_Uart_printName(self);
}

void Device_Uart_sendMethod(PikaObj *self, Args *args){
    char * data = args_getStr(args, "data");
    Device_Uart_send(self, data);
}

void Device_Uart_setNameMethod(PikaObj *self, Args *args){
    char * name = args_getStr(args, "name");
    Device_Uart_setName(self, name);
}

PikaObj *New_Device_Uart(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "printName()", Device_Uart_printNameMethod);
    class_defineMethod(self, "send(data:str)", Device_Uart_sendMethod);
    class_defineMethod(self, "setName(name:str)", Device_Uart_setNameMethod);
    return self;
}
