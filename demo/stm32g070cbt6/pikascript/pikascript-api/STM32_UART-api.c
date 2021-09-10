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
    int length = args_getInt(args, "length");
    STM32_UART_platformRead(self, length);
}

void STM32_UART_platformWriteMethod(PikaObj *self, Args *args){
    char * data = args_getStr(args, "data");
    STM32_UART_platformWrite(self, data);
}

PikaObj *New_STM32_UART(Args *args){
    PikaObj *self = New_PikaStdDevice_UART(args);
    class_defineMethod(self, "platformEnable(id:int,baudRate:int)", STM32_UART_platformEnableMethod);
    class_defineMethod(self, "platformRead(length:int)", STM32_UART_platformReadMethod);
    class_defineMethod(self, "platformWrite(data:str)", STM32_UART_platformWriteMethod);
    return self;
}
