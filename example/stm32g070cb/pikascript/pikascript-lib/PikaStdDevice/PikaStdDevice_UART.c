#include "PikaStdDevice_UART.h"
#include "BaseObj.h"

void PikaStdDevice_UART_enable(PikaObj* self) {
    obj_run(self, "platformEnable(id, baudRate)");
}
void PikaStdDevice_UART_init(PikaObj* self) {
    obj_setInt(self, "baudRate", 115200);
    obj_setInt(self, "id", 1);
    obj_setStr(self, "readBuff", "");
}
char* PikaStdDevice_UART_read(PikaObj* self, int length) {
    obj_setInt(self, "length", length);
    obj_run(self, "readData = platformRead(id, length)");
    return obj_getStr(self, "readData");
}
void PikaStdDevice_UART_setBaudRate(PikaObj* self, int baudRate) {
    obj_setInt(self, "baudRate", baudRate);
}
void PikaStdDevice_UART_setId(PikaObj* self, int id) {
    obj_setInt(self, "id", id);
}
void PikaStdDevice_UART_write(PikaObj* self, char* data) {
    obj_setStr(self, "writeData", data);
    obj_run(self, "platformWrite(id, writeData)");
}

void PikaStdDevice_UART_platformEnable(PikaObj* self, int baudRate, int id) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}
char* PikaStdDevice_UART_platformRead(PikaObj* self, int id, int length) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
    return NULL;
}
void PikaStdDevice_UART_platformWrite(PikaObj* self, char* data, int id) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}