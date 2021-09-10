/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdDevice_UART.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdDevice_UART_enableMethod(PikaObj *self, Args *args){
    PikaStdDevice_UART_enable(self);
}

void PikaStdDevice_UART_initMethod(PikaObj *self, Args *args){
    PikaStdDevice_UART_init(self);
}

void PikaStdDevice_UART_platformEnableMethod(PikaObj *self, Args *args){
    int baudRate = args_getInt(args, "baudRate");
    int id = args_getInt(args, "id");
    PikaStdDevice_UART_platformEnable(self, baudRate, id);
}

void PikaStdDevice_UART_platformReadMethod(PikaObj *self, Args *args){
    int length = args_getInt(args, "length");
    PikaStdDevice_UART_platformRead(self, length);
}

void PikaStdDevice_UART_platformWriteMethod(PikaObj *self, Args *args){
    char * data = args_getStr(args, "data");
    PikaStdDevice_UART_platformWrite(self, data);
}

void PikaStdDevice_UART_readMethod(PikaObj *self, Args *args){
    int length = args_getInt(args, "length");
    PikaStdDevice_UART_read(self, length);
}

void PikaStdDevice_UART_setBaudRateMethod(PikaObj *self, Args *args){
    int baudRate = args_getInt(args, "baudRate");
    PikaStdDevice_UART_setBaudRate(self, baudRate);
}

void PikaStdDevice_UART_setIdMethod(PikaObj *self, Args *args){
    int id = args_getInt(args, "id");
    PikaStdDevice_UART_setId(self, id);
}

void PikaStdDevice_UART_writeMethod(PikaObj *self, Args *args){
    char * data = args_getStr(args, "data");
    PikaStdDevice_UART_write(self, data);
}

PikaObj *New_PikaStdDevice_UART(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "enable()", PikaStdDevice_UART_enableMethod);
    class_defineMethod(self, "init()", PikaStdDevice_UART_initMethod);
    class_defineMethod(self, "platformEnable(id:int,baudRate:int)", PikaStdDevice_UART_platformEnableMethod);
    class_defineMethod(self, "platformRead(length:int)", PikaStdDevice_UART_platformReadMethod);
    class_defineMethod(self, "platformWrite(data:str)", PikaStdDevice_UART_platformWriteMethod);
    class_defineMethod(self, "read(length:int)", PikaStdDevice_UART_readMethod);
    class_defineMethod(self, "setBaudRate(baudRate:int)", PikaStdDevice_UART_setBaudRateMethod);
    class_defineMethod(self, "setId(id:int)", PikaStdDevice_UART_setIdMethod);
    class_defineMethod(self, "write(data:str)", PikaStdDevice_UART_writeMethod);
    return self;
}
