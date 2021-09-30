/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "STM32_UART.h"
#include "PikaStdDevice_UART.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void STM32_UART_platformEnableMethod(PikaObj *self, Args *args){
    int baudRate = args_getInt(args, "baudRate");
    int id = args_getInt(args, "id");
    STM32_UART_platformEnable(self, baudRate, id);
}

void STM32_UART_platformReadMethod(PikaObj *self, Args *args){
    int id = args_getInt(args, "id");
    int length = args_getInt(args, "length");
    char * res = STM32_UART_platformRead(self, id, length);
    method_returnStr(args, res);
}

void STM32_UART_platformWriteMethod(PikaObj *self, Args *args){
    char * data = args_getStr(args, "data");
    int id = args_getInt(args, "id");
    STM32_UART_platformWrite(self, data, id);
}

PikaObj *New_STM32_UART(Args *args){
    PikaObj *self = New_PikaStdDevice_UART(args);
    class_defineMethod(self, "platformEnable(id:int,baudRate:int)", STM32_UART_platformEnableMethod);
    class_defineMethod(self, "platformRead(id:int,length:int)->str", STM32_UART_platformReadMethod);
    class_defineMethod(self, "platformWrite(id:int,data:str)", STM32_UART_platformWriteMethod);
    return self;
}
