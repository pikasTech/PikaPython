#include "PikaStdDevice_SPI.h"
#include "PikaStdDevice_common.h"

void PikaStdDevice_SPI___init__(PikaObj* self) {
    obj_setInt(self, "baudRate", 1000);
    obj_setInt(self, "id", 0);
    obj_setStr(self, "name", "SOFT");
    obj_setInt(self, "phase", 0);
    obj_setStr(self, "MISOpin", "none");
    obj_setStr(self, "MOSIpin", "none");
    obj_setStr(self, "SCKpin", "none");
    obj_setInt(self, "polarity", 0);
    obj_setPtr(self, "pika_dev", NULL);
}

static pika_dev* _get_dev(PikaObj* self) {
    pika_dev* dev = PIKA_HAL_OBJ2DEV(self);
    if (NULL != dev) {
        return dev;
    }
    char* name = obj_getStr(self, "name");
    if (0 == strcmp(name, "SOFT")) {
        dev = pika_hal_open(PIKA_HAL_SOFT_SPI, name);
        pika_debug("Open soft spi %s\r\n", name);
    } else {
        dev = pika_hal_open(PIKA_HAL_SPI, name);
        pika_debug("Open spi %s\r\n", name);
    }
    if (NULL == dev) {
        __platform_printf("Error: open IIC '%s' failed.\r\n",
                          obj_getStr(self, "name"));
        return NULL;
    }
    obj_setPtr(self, "pika_dev", dev);
    return dev;
}

void PikaStdDevice_SPI_disable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        pika_platform_printf("Error: SPI '%s' not found.\r\n",
                             obj_getStr(self, "name"));
        return;
    }
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_DISABLE);
}

void PikaStdDevice_SPI_enable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        pika_platform_printf("Error: SPI '%s' not found.\r\n",
                             obj_getStr(self, "name"));
        return;
    }
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_ENABLE);
}

void PikaStdDevice_SPI_write(PikaObj* self, char* data) {
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        pika_platform_printf("Error: SPI '%s' not found.\r\n",
                             obj_getStr(self, "name"));
        return;
    }
    pika_hal_write(dev, data, strlen(data));
}

void PikaStdDevice_SPI_writeBytes(PikaObj* self, uint8_t* data, int length) {
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        pika_platform_printf("Error: SPI '%s' not found.\r\n",
                             obj_getStr(self, "name"));
        return;
    }
    pika_hal_write(dev, data, length);
}

char* PikaStdDevice_SPI_read(PikaObj* self, int length) {
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        pika_platform_printf("Error: SPI '%s' not found.\r\n",
                             obj_getStr(self, "name"));
        return NULL;
    }
    char* data = (char*)pikaMalloc(length + 1);
    if (NULL == data) {
        pika_platform_printf("Error: SPI read failed, no memory.\r\n");
        return NULL;
    }
    int ret = pika_hal_read(dev, data, length);
    if (ret < 0) {
        pikaFree(data, length + 1);
        return NULL;
    }
    data[length] = '\0';
    char* res = obj_cacheStr(self, data);
    pikaFree(data, length + 1);
    return res;
}

Arg* PikaStdDevice_SPI_readBytes(PikaObj* self, int length) {
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        pika_platform_printf("Error: SPI '%s' not found.\r\n",
                             obj_getStr(self, "name"));
        return NULL;
    }
    uint8_t* data = (uint8_t*)pikaMalloc(length);
    if (NULL == data) {
        pika_platform_printf("Error: SPI read failed, no memory.\r\n");
        return NULL;
    }
    int ret = pika_hal_read(dev, data, length);
    if (ret < 0) {
        pikaFree(data, length);
        return NULL;
    }
    Arg* res = arg_newBytes(data, length);
    pikaFree(data, length);
    return res;
}

void PikaStdDevice_SPI_setBaudRate(PikaObj* self, int baudRate) {
    obj_setInt(self, "baudRate", baudRate);
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        pika_platform_printf("Error: SPI '%s' not found.\r\n",
                             obj_getStr(self, "name"));
    }
    pika_hal_SPI_config cfg = {0};
    cfg.speed = baudRate;
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
}

void PikaStdDevice_SPI_setId(PikaObj* self, int id) {
    obj_setInt(self, "id", id);
    Args buffs = {0};
    char* name = strsFormat(&buffs, 64, "SPI%d", id);
    obj_setName(self, name);
    args_deinit(&buffs);
}

void PikaStdDevice_SPI_setName(PikaObj* self, char* name) {
    obj_setStr(self, "name", name);
}

void PikaStdDevice_SPI_setPhase(PikaObj* self, int phase) {
    obj_setInt(self, "phase", phase);
}

void PikaStdDevice_SPI_setPinMISO(PikaObj* self, char* pin) {
    obj_setStr(self, "MISOpin", pin);
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        pika_platform_printf("Error: SPI '%s' not found.\r\n",
                             obj_getStr(self, "name"));
        return;
    }
    pika_hal_SPI_config cfg = {0};
    cfg.MISO = pika_hal_open(PIKA_HAL_GPIO, pin);
    if (NULL == cfg.MISO) {
        pika_platform_printf("Error: open MISO '%s' failed.\r\n", pin);
        return;
    }
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
}

void PikaStdDevice_SPI_setPinCS(PikaObj* self, char* pin) {
    obj_setStr(self, "CSpin", pin);
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        pika_platform_printf("Error: SPI '%s' not found.\r\n",
                             obj_getStr(self, "name"));
        return;
    }
    pika_hal_SPI_config cfg = {0};
    cfg.CS = pika_hal_open(PIKA_HAL_GPIO, pin);
    if (NULL == cfg.CS) {
        pika_platform_printf("Error: open CS '%s' failed.\r\n", pin);
        return;
    }
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
}

void PikaStdDevice_SPI_setPinMOSI(PikaObj* self, char* pin) {
    obj_setStr(self, "MOSIpin", pin);
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        pika_platform_printf("Error: SPI '%s' not found.\r\n",
                             obj_getStr(self, "name"));
        return;
    }
    pika_hal_SPI_config cfg = {0};
    cfg.MOSI = pika_hal_open(PIKA_HAL_GPIO, pin);
    if (NULL == cfg.MOSI) {
        pika_platform_printf("Error: open MOSI '%s' failed.\r\n", pin);
        return;
    }
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
}

void PikaStdDevice_SPI_setPinSCK(PikaObj* self, char* pin) {
    obj_setStr(self, "SCKpin", pin);
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        pika_platform_printf("Error: SPI '%s' not found.\r\n",
                             obj_getStr(self, "name"));
        return;
    }
    pika_hal_SPI_config cfg = {0};
    cfg.SCK = pika_hal_open(PIKA_HAL_GPIO, pin);
    if (NULL == cfg.SCK) {
        pika_platform_printf("Error: open SCK '%s' failed.\r\n", pin);
        return;
    }
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
}

void PikaStdDevice_SPI_setPolarity(PikaObj* self, int polarity) {
    obj_setInt(self, "polarity", polarity);
}
