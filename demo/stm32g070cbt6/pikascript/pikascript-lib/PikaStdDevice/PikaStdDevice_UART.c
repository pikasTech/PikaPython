#include "PikaStdDevice_UART.h"
#include "BaseObj.h"

void PikaStdDevice_UART_enable(PikaObj* self) {}
void PikaStdDevice_UART_init(PikaObj* self) {}
void PikaStdDevice_UART_read(PikaObj* self, int length) {}
void PikaStdDevice_UART_setBaudRate(PikaObj* self, int baudRate) {}
void PikaStdDevice_UART_setId(PikaObj* self, int id) {}
void PikaStdDevice_UART_write(PikaObj* self, char* data) {}

void PikaStdDevice_UART_platformEnable(PikaObj* self, int baudRate, int id) {
  obj_setErrorCode(self, 1);
  obj_setSysOut(self, "[error] platform method need to be override.");
}
void PikaStdDevice_UART_platformRead(PikaObj* self, int length) {
  obj_setErrorCode(self, 1);
  obj_setSysOut(self, "[error] platform method need to be override.");
}
void PikaStdDevice_UART_platformWrite(PikaObj* self, char* data) {
  obj_setErrorCode(self, 1);
  obj_setSysOut(self, "[error] platform method need to be override.");
}