#include "PikaStdDevice_PWM.h"
#include "PikaStdDevice_common.h"

void PikaStdDevice_PWM_init(PikaObj* self) {
    obj_setStr(self, "pin", "none");
    obj_setStr(self, "name", "none");
    obj_setInt(self, "freq", 1000);
    obj_setInt(self, "ch", 0);
    obj_setFloat(self, "duty", 0.5f);
}

void PikaStdDevice_PWM___init__(PikaObj* self) {
    PikaStdDevice_PWM_init(self);
}

void PikaStdDevice_PWM_setPin(PikaObj* self, char* pin) {
    obj_setStr(self, "pin", pin);
}

void PikaStdDevice_PWM_setFrequency(PikaObj* self, int freq) {
    obj_setInt(self, "freq", freq);
    obj_runNativeMethod(self, "platformSetFrequency", NULL);
}

void PikaStdDevice_PWM_setDuty(PikaObj* self, pika_float duty) {
    obj_setFloat(self, "duty", duty);
    obj_runNativeMethod(self, "platformSetDuty", NULL);
}

void PikaStdDevice_PWM_enable(PikaObj* self) {
    obj_runNativeMethod(self, "platformEnable", NULL);
}

void PikaStdDevice_PWM_disable(PikaObj* self) {
    obj_runNativeMethod(self, "platformDisable", NULL);
}

pika_float PikaStdDevice_PWM_getDuty(PikaObj* self) {
    return obj_getFloat(self, "duty");
}

int PikaStdDevice_PWM_getFrequency(PikaObj* self) {
    return obj_getInt(self, "freq");
}

char* PikaStdDevice_PWM_getName(PikaObj* self) {
    return obj_getStr(self, "name");
}

void PikaStdDevice_PWM_setName(PikaObj* self, char* name) {
    obj_setStr(self, "name", name);
}

void PikaStdDevice_PWM_setChannel(PikaObj* self, int ch) {
    obj_setInt(self, "ch", ch);
}

int PikaStdDevice_PWM_getChannel(PikaObj* self) {
    return obj_getInt(self, "ch");
}

void PikaStdDevice_PWM_setFreq(PikaObj* self, int freq) {
    PikaStdDevice_PWM_setFrequency(self, freq);
}

static pika_dev* _get_dev(PikaObj* self) {
    pika_dev* dev = obj_getPtr(self, "pika_dev");
    if (NULL != dev) {
        return dev;
    }
    dev = pika_hal_open(PIKA_HAL_PWM, obj_getStr(self, "pin"));
    if (NULL == dev) {
        __platform_printf("Error: open PWM '%s' failed.\r\n",
                          obj_getStr(self, "pin"));
    }
    obj_setPtr(self, "pika_dev", dev);
    return dev;
}

void PikaStdDevice_PWM_platformEnable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_ENABLE);
}

void PikaStdDevice_PWM_platformSetDuty(PikaObj* self) {
    pika_float duty = obj_getFloat(self, "duty");  // 0.0 ~ 1.0
    uint32_t freq = obj_getInt(self, "freq");      // Hz
    pika_hal_PWM_config cfg = {0};
    /* ns */
    cfg.duty = (uint32_t)(1000000000.0f / freq * duty);
    pika_hal_ioctl(_get_dev(self), PIKA_HAL_IOCTL_CONFIG, &cfg);
}

void PikaStdDevice_PWM_platformSetFrequency(PikaObj* self) {
    uint32_t freq = obj_getInt(self, "freq");      // Hz
    pika_float duty = obj_getFloat(self, "duty");  // 0.0 ~ 1.0
    pika_hal_PWM_config cfg = {0};
    /* ns */
    cfg.period = (uint32_t)(1000000000.0f / freq);
    cfg.duty = (uint32_t)(1000000000.0f / freq * duty);
    pika_hal_ioctl(_get_dev(self), PIKA_HAL_IOCTL_CONFIG, &cfg);
}

void PikaStdDevice_PWM_platformDisable(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_DISABLE);
}

void PikaStdDevice_PWM_close(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    pika_hal_close(dev);
}
