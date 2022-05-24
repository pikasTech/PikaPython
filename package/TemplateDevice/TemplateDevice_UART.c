#include "TemplateDevice_UART.h"

void TemplateDevice_UART_platformDisable(PikaObj* self) {}
void TemplateDevice_UART_platformEnable(PikaObj* self) {}
void TemplateDevice_UART_platformRead(PikaObj* self) {}
void TemplateDevice_UART_platformReadBytes(PikaObj* self) {
    int length = obj_getInt(self, "length");
    uint8_t bytes_buff[] = {0x00, 0x01, 0x02, 0x03, 0x04,
                            0x05, 0x06, 0x07, 0x08};
    obj_setBytes(self, "readData", bytes_buff, length);
}
void TemplateDevice_UART_platformWrite(PikaObj* self) {}
void TemplateDevice_UART_platformWriteBytes(PikaObj* self) {}