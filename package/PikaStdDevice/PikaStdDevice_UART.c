#include "PikaStdDevice_UART.h"
#include "PikaStdDevice_common.h"

static pika_dev* _get_dev(PikaObj* self) {
    pika_dev* dev = PIKA_HAL_OBJ2DEV(self);
    if (NULL != dev) {
        return dev;
    }
    int id = obj_getInt(self, "id");
    char id_str[32] = {0};
    sprintf(id_str, "UART%d", id);
    dev = pika_hal_open(PIKA_HAL_UART, id_str);
    if (NULL == dev) {
        __platform_printf("Error: open UART '%s' failed.\r\n", id_str);
    }
    obj_setPtr(self, "pika_dev", dev);
    return dev;
}

void PikaStdDevice_UART_enable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        __platform_printf("Error: open UART '%d' failed.\r\n",
                          (int)obj_getInt(self, "id"));
        return;
    }
    pika_hal_UART_config cfg = {0};
    cfg.baudrate = obj_getInt(self, "baudRate");
    cfg.flow_control = obj_getInt(self, "flowControl");
    cfg.stop_bits = obj_getInt(self, "stopBits");
    cfg.parity = (PIKA_HAL_UART_PARITY)obj_getInt(self, "parity");
    cfg.data_bits = obj_getInt(self, "dataBits");
    if (!strEqu(obj_getStr(self, "TXpin"), "none")) {
        cfg.TX = pika_hal_open(PIKA_HAL_GPIO, obj_getStr(self, "TXpin"));
        if (NULL == cfg.TX) {
            __platform_printf("Error: open GPIO '%s' failed.\r\n",
                              obj_getStr(self, "TXpin"));
        }
    }
    if (!strEqu(obj_getStr(self, "RXpin"), "none")) {
        cfg.RX = pika_hal_open(PIKA_HAL_GPIO, obj_getStr(self, "RXpin"));
        if (NULL == cfg.RX) {
            __platform_printf("Error: open GPIO '%s' failed.\r\n",
                              obj_getStr(self, "RXpin"));
        }
    }
    if (!strEqu(obj_getStr(self, "RTSpin"), "none")) {
        cfg.RTS = pika_hal_open(PIKA_HAL_GPIO, obj_getStr(self, "RTSpin"));
        if (NULL == cfg.RTS) {
            __platform_printf("Error: open GPIO '%s' failed.\r\n",
                              obj_getStr(self, "RTSpin"));
        }
    }
    if (!strEqu(obj_getStr(self, "CTSpin"), "none")) {
        cfg.CTS = pika_hal_open(PIKA_HAL_GPIO, obj_getStr(self, "CTSpin"));
        if (NULL == cfg.CTS) {
            __platform_printf("Error: open GPIO '%s' failed.\r\n",
                              obj_getStr(self, "CTSpin"));
        }
    }
    int err = -1;
    err = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
    if (err != 0) {
        __platform_printf("Error: config UART '%d' failed.\r\n",
                          (int)obj_getInt(self, "id"));
        return;
    }
    err = pika_hal_ioctl(dev, PIKA_HAL_IOCTL_ENABLE);
    if (err != 0) {
        __platform_printf("Error: enable UART '%d' failed.\r\n",
                          (int)obj_getInt(self, "id"));
        return;
    }
}

void PikaStdDevice_UART_disable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        __platform_printf("Error: open UART '%d' failed.\r\n",
                          (int)obj_getInt(self, "id"));
        return;
    }
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_DISABLE);
}

void PikaStdDevice_UART_init(PikaObj* self) {
    /* const */
    obj_setInt(self, "FLOW_CONTROL_NONE", PIKA_HAL_UART_FLOW_CONTROL_NONE);
    obj_setInt(self, "FLOW_CONTROL_RTS", PIKA_HAL_UART_FLOW_CONTROL_RTS);
    obj_setInt(self, "FLOW_CONTROL_CTS", PIKA_HAL_UART_FLOW_CONTROL_CTS);
    obj_setInt(self, "FLOW_CONTROL_RTS_CTS",
               PIKA_HAL_UART_FLOW_CONTROL_RTS_CTS);

    obj_setInt(self, "SIGNAL_RX", PIKA_HAL_UART_EVENT_SIGNAL_RX);
    obj_setInt(self, "SIGNAL_TX", PIKA_HAL_UART_EVENT_SIGNAL_TX);

    obj_setInt(self, "STOP_BITS_1", PIKA_HAL_UART_STOP_BITS_1);
    obj_setInt(self, "STOP_BITS_2", PIKA_HAL_UART_STOP_BITS_2);
    obj_setInt(self, "STOP_BITS_1_5", PIKA_HAL_UART_STOP_BITS_1_5);
    obj_setInt(self, "PARITY_NONE", PIKA_HAL_UART_PARITY_NONE);
    obj_setInt(self, "PARITY_ODD", PIKA_HAL_UART_PARITY_ODD);
    obj_setInt(self, "PARITY_EVEN", PIKA_HAL_UART_PARITY_EVEN);

    /* default */
    obj_setInt(self, "baudRate", 115200);
    obj_setInt(self, "id", 1);
    obj_setStr(self, "readBuff", "");
    obj_setInt(self, "flowControl", PIKA_HAL_UART_FLOW_CONTROL_NONE);
    obj_setStr(self, "TXpin", "none");
    obj_setStr(self, "RXpin", "none");
    obj_setStr(self, "RTSpin", "none");
    obj_setStr(self, "CTSpin", "none");
    obj_setInt(self, "dataBits", 8);
    obj_setInt(self, "parity", PIKA_HAL_UART_PARITY_NONE);
    obj_setInt(self, "stopBits", PIKA_HAL_UART_STOP_BITS_1);
}

void PikaStdDevice_UART___init__(PikaObj* self) {
    PikaStdDevice_UART_init(self);
}

char* PikaStdDevice_UART_read(PikaObj* self, int length) {
    int len = length;
    obj_setBytes(self, "_readData", NULL, len + 1);
    char* buff = (char*)obj_getBytes(self, "_readData");
    pika_dev* dev = _get_dev(self);
    int len_get = pika_hal_read(dev, buff, len);
    buff[len_get] = 0;
    obj_setStr(self, "readData", buff);
    return obj_getStr(self, "readData");
}

Arg* PikaStdDevice_UART_readBytes(PikaObj* self, int length) {
    int len = length;
    obj_setBytes(self, "_readData", NULL, len + 1);
    uint8_t* buff = obj_getBytes(self, "_readData");
    pika_dev* dev = _get_dev(self);
    int len_get = pika_hal_read(dev, buff, len);
    obj_setBytes(self, "readData", buff, len_get);
    return arg_copy(obj_getArg(self, "readData"));
}

void PikaStdDevice_UART_setBaudRate(PikaObj* self, int baudRate) {
    obj_setInt(self, "baudRate", baudRate);
}

void PikaStdDevice_UART_setFlowControl(PikaObj* self, int flowControl) {
    obj_setInt(self, "flowControl", flowControl);
}

void PikaStdDevice_UART_setDataBits(PikaObj* self, int dataBits) {
    obj_setInt(self, "dataBits", dataBits);
}

void PikaStdDevice_UART_setParity(PikaObj* self, int parity) {
    obj_setInt(self, "parity", parity);
}

void PikaStdDevice_UART_setStopBits(PikaObj* self, int stopBits) {
    obj_setInt(self, "stopBits", stopBits);
}

void PikaStdDevice_UART_setId(PikaObj* self, int id) {
    obj_setInt(self, "id", id);
}

void PikaStdDevice_UART_write(PikaObj* self, char* data) {
    pika_dev* dev = _get_dev(self);
    pika_hal_write(dev, data, strlen(data));
}

void PikaStdDevice_UART_writeBytes(PikaObj* self, uint8_t* data, int length) {
    int len = length;
    pika_dev* dev = _get_dev(self);
    pika_hal_write(dev, data, len);
}

void PikaStdDevice_UART_setCallback(PikaObj* self,
                                    Arg* eventCallBack,
                                    int filter) {
#if PIKA_EVENT_ENABLE
    pika_dev* dev = _get_dev(self);
    _PikaStdDevice_setCallback(self, eventCallBack, (uintptr_t)dev);
    /* regist event to pika_hal */
    pika_hal_UART_config cfg_cb = {0};
    cfg_cb.event_callback = _PikaStdDevice_UART_event_handler;
    cfg_cb.event_callback_filter = filter;
    cfg_cb.event_callback_ena = PIKA_HAL_EVENT_CALLBACK_ENA_ENABLE;
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg_cb);
#else
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] PIKA_EVENT_ENABLE is disabled.");
#endif
}

void PikaStdDevice_UART_setCallBack(PikaObj* self,
                                    Arg* eventCallBack,
                                    int filter) {
    pika_platform_printf(
        "Warning: setCallBack is deprecated, use setCallback instead.\r\n");
    PikaStdDevice_UART_setCallback(self, eventCallBack, filter);
}

void PikaStdDevice_UART_close(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    pika_hal_UART_config* cfg = (pika_hal_UART_config*)dev->ioctl_config;
    if (NULL != cfg->TX) {
        pika_hal_close(cfg->TX);
        cfg->TX = NULL;
    }
    if (NULL != cfg->RX) {
        pika_hal_close(cfg->RX);
        cfg->RX = NULL;
    }
    if (NULL != cfg->RTS) {
        pika_hal_close(cfg->RTS);
        cfg->RTS = NULL;
    }
    if (NULL != cfg->CTS) {
        pika_hal_close(cfg->CTS);
        cfg->CTS = NULL;
    }
    pika_hal_close(dev);
}

void PikaStdDevice_UART_setPinCTS(PikaObj* self, char* pin) {
    obj_setStr(self, "CTSpin", pin);
}

void PikaStdDevice_UART_setPinRTS(PikaObj* self, char* pin) {
    obj_setStr(self, "RTSpin", pin);
}

void PikaStdDevice_UART_setPinRX(PikaObj* self, char* pin) {
    obj_setStr(self, "RXpin", pin);
}

void PikaStdDevice_UART_setPinTX(PikaObj* self, char* pin) {
    obj_setStr(self, "TXpin", pin);
}
