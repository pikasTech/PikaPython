#include "PikaStdDevice_UART.h"
#include "BaseObj.h"

void PikaStdDevice_UART_enable(PikaObj* self) {
    obj_runNativeMethod(self, "platformEnable", NULL);
}

void PikaStdDevice_UART_disable(PikaObj *self){
    obj_runNativeMethod(self, "platformDisable", NULL);
}

void PikaStdDevice_UART_init(PikaObj* self) {
    obj_setInt(self, "baudRate", 115200);
    obj_setInt(self, "id", 1);
    obj_setStr(self, "readBuff", "");
}

void PikaStdDevice_UART___init__(PikaObj* self) {
    PikaStdDevice_UART_init(self);
}

char* PikaStdDevice_UART_read(PikaObj* self, int length) {
    obj_setInt(self, "length", length);
    obj_runNativeMethod(self, "platformRead", NULL);
    return obj_getStr(self, "readData");
}

Arg* PikaStdDevice_UART_readBytes(PikaObj *self, int length){
    obj_setInt(self, "length", length);
    obj_runNativeMethod(self, "platformReadBytes", NULL);
    return arg_copy(obj_getArg(self, "readData"));
}

void PikaStdDevice_UART_setBaudRate(PikaObj* self, int baudRate) {
    obj_setInt(self, "baudRate", baudRate);
}
void PikaStdDevice_UART_setId(PikaObj* self, int id) {
    obj_setInt(self, "id", id);
}
void PikaStdDevice_UART_write(PikaObj* self, char* data) {
    obj_setStr(self, "writeData", data);
    obj_runNativeMethod(self, "platformWrite", NULL);
}

void PikaStdDevice_UART_writeBytes(PikaObj *self, uint8_t* data, int length){
    obj_setBytes(self, "writeData", data, length);
    obj_runNativeMethod(self, "platformWriteBytes", NULL);
}

void PikaStdDevice_UART_platformEnable(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}
void PikaStdDevice_UART_platformRead(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}
void PikaStdDevice_UART_platformWrite(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_UART_platformDisable(PikaObj *self){
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_UART_platformReadBytes(PikaObj *self){
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_UART_platformWriteBytes(PikaObj *self){
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}
