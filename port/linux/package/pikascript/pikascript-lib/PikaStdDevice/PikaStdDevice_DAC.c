#include "PikaStdDevice_DAC.h"
#include "PikaStdDevice_common.h"

static pika_dev* _get_dev(PikaObj* self) {
    pika_dev* dev = obj_getPtr(self, "pika_dev");
    if (NULL != dev) {
        return dev;
    }
    dev = pika_hal_open(PIKA_HAL_DAC, obj_getStr(self, "pin"));
    if (NULL == dev) {
        __platform_printf("Error: open DAC '%s' failed.\r\n",
                          obj_getStr(self, "pin"));
    }
    obj_setPtr(self, "pika_dev", dev);
    return dev;
}

void PikaStdDevice_DAC_enable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    pika_hal_DAC_config cfg = {0};
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_ENABLE);
}

void PikaStdDevice_DAC_setPin(PikaObj* self, char* pin) {
    obj_setStr(self, "pin", pin);
}

void PikaStdDevice_DAC_disable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_DISABLE);
}

void PikaStdDevice_DAC_init(PikaObj* self) {
    obj_setStr(self, "pin", "PA0");
}

void PikaStdDevice_DAC___init__(PikaObj* self) {
    PikaStdDevice_DAC_init(self);
}

void PikaStdDevice_DAC_write(PikaObj* self, pika_float val) {
    pika_dev* dev = _get_dev(self);
    pika_hal_DAC_config* cfg = (pika_hal_DAC_config*)dev->ioctl_config;
    uint32_t val_i = (val / cfg->vref) * cfg->max;
    pika_hal_write(dev, &val_i, sizeof val_i);
}

void PikaStdDevice_DAC_close(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    pika_hal_close(dev);
}
