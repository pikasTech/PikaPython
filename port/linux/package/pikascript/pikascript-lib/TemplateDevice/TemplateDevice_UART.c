#include "TemplateDevice_UART.h"
#include <stdio.h>

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
void TemplateDevice_UART_platformWriteBytes(PikaObj* self) {
    uint8_t* bytes = obj_getBytes(self, "writeData");
    int length = obj_getBytesSize(self, "writeData");
    printf("[debug]: write to uart: [ ");
    for (int i = 0; i < length; i++) {
        printf("0x%02X ", bytes[i]);
    }
    printf("]\n");
}
