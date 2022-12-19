#include "PikaStdDevice_UART.h"
#include "BaseObj.h"
#include "pika_hal.h"

void PikaStdDevice_UART_enable(PikaObj* self) {
    obj_runNativeMethod(self, "platformEnable", NULL);
}

void PikaStdDevice_UART_disable(PikaObj* self) {
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

Arg* PikaStdDevice_UART_readBytes(PikaObj* self, int length) {
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

void PikaStdDevice_UART_writeBytes(PikaObj* self, uint8_t* data, int length) {
    obj_setBytes(self, "writeData", data, length);
    obj_runNativeMethod(self, "platformWriteBytes", NULL);
}

static pika_dev* _get_dev(PikaObj* self) {
    pika_dev* dev = obj_getPtr(self, "pika_dev");
    if (NULL != dev) {
        return dev;
    }
    int id = obj_getInt(self, "id");
    char id_str[32] = {0};
    sprintf(id_str, "%d", id);
    dev = pika_hal_open(PIKA_HAL_UART, id_str);
    if (NULL == dev) {
        __platform_printf("Error: open UART '%s' failed.\r\n", id_str);
    }
    obj_setPtr(self, "pika_dev", dev);
    return dev;
}

void PikaStdDevice_UART_platformEnable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        __platform_printf("Error: open UART '%d' failed.\r\n",
                          obj_getInt(self, "id"));
        return;
    }
    pika_hal_UART_config cfg = {0};
    cfg.baudrate = obj_getInt(self, "baudRate");
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_ENABLE);
}

void PikaStdDevice_UART_platformRead(PikaObj* self) {
    int len = obj_getInt(self, "length");
    obj_setBytes(self, "_readData", NULL, len + 1);
    char* buff = obj_getBytes(self, "_readData");
    pika_dev* dev = _get_dev(self);
    pika_hal_read(dev, buff, len);
    obj_setStr(self, "readData", buff);
}

void PikaStdDevice_UART_platformWrite(PikaObj* self) {
    char* data = obj_getStr(self, "writeData");
    pika_dev* dev = _get_dev(self);
    pika_hal_write(dev, data, strlen(data));
}

void PikaStdDevice_UART_platformDisable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        __platform_printf("Error: open UART '%d' failed.\r\n",
                          obj_getInt(self, "id"));
        return;
    }
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_DISABLE);
}

void PikaStdDevice_UART_platformReadBytes(PikaObj* self) {
    int len = obj_getInt(self, "length");
    obj_setBytes(self, "_readData", NULL, len + 1);
    uint8_t* buff = obj_getBytes(self, "_readData");
    pika_dev* dev = _get_dev(self);
    pika_hal_read(dev, buff, len);
    obj_setBytes(self, "readData", buff, len);
}

void PikaStdDevice_UART_platformWriteBytes(PikaObj* self) {
    uint8_t* data = obj_getBytes(self, "writeData");
    int len = obj_getBytesLen(self, "writeData");
    pika_dev* dev = _get_dev(self);
    pika_hal_write(dev, data, len);
}
