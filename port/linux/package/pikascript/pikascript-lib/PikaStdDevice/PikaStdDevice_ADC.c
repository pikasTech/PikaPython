#include "PikaStdDevice_ADC.h"
#include "PikaStdDevice_common.h"

void PikaStdDevice_ADC_enable(PikaObj* self) {
    obj_runNativeMethod(self, "platformEnable", NULL);
}

void PikaStdDevice_ADC_disable(PikaObj* self) {
    obj_runNativeMethod(self, "platformDisable", NULL);
}

void PikaStdDevice_ADC_init(PikaObj* self) {
    obj_setStr(self, "pin", "PA0");
}

void PikaStdDevice_ADC___init__(PikaObj* self) {
    PikaStdDevice_ADC_init(self);
}

pika_float PikaStdDevice_ADC_read(PikaObj* self) {
    obj_runNativeMethod(self, "platformRead", NULL);
    return obj_getFloat(self, "val");
}

void PikaStdDevice_ADC_setPin(PikaObj* self, char* pin) {
    obj_setStr(self, "pin", pin);
}

static pika_dev* _get_dev(PikaObj* self) {
    pika_dev* dev = obj_getPtr(self, "pika_dev");
    if (NULL != dev) {
        return dev;
    }
    dev = pika_hal_open(PIKA_HAL_ADC, obj_getStr(self, "pin"));
    if (NULL == dev) {
        __platform_printf("Error: open ADC '%s' failed.\r\n",
                          obj_getStr(self, "pin"));
    }
    obj_setPtr(self, "pika_dev", dev);
    return dev;
}

void PikaStdDevice_ADC_platformEnable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    if (NULL == dev) {
        __platform_printf("Error: open ADC '%s' failed.\r\n",
                          obj_getStr(self, "pin"));
        return;
    }
    pika_hal_ADC_config cfg = {0};
    cfg.continue_or_single = PIKA_HAL_ADC_SINGLE;
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_ENABLE);
}

void PikaStdDevice_ADC_platformDisable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_DISABLE);
}

void PikaStdDevice_ADC_platformRead(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    pika_hal_ADC_config* cfg = (pika_hal_ADC_config*)dev->ioctl_config;
    uint32_t val_i = 0;
    pika_hal_read(dev, &val_i, sizeof(val_i));
    pika_float val = (pika_float)val_i / (pika_float)cfg->max * cfg->vref;
    val = val_i * obj_setFloat(self, "val", val);
}

void PikaStdDevice_ADC_close(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    pika_hal_close(dev);
}
