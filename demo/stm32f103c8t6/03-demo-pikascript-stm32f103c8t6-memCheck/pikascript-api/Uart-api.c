/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "Uart.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void Uart_printNameMethod(PikaObj *self, Args *args){
    Uart_printName(self);
}

void Uart_sendMethod(PikaObj *self, Args *args){
    char * data = args_getStr(args, "data");
    Uart_send(self, data);
}

void Uart_setNameMethod(PikaObj *self, Args *args){
    char * name = args_getStr(args, "name");
    Uart_setName(self, name);
}

PikaObj *New_Uart(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "printName()", Uart_printNameMethod);
    class_defineMethod(self, "send(data:str)", Uart_sendMethod);
    class_defineMethod(self, "setName(name:str)", Uart_setNameMethod);
    return self;
}
