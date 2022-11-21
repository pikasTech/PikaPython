#include "PikaStdDevice_GPIO.h"
#include "BaseObj.h"
#include "pika_hal.h"

void PikaStdDevice_GPIO_init(PikaObj* self) {
    obj_setInt(self, "isEnable", 0);
    obj_setStr(self, "pin", "none");
    obj_setStr(self, "mode", "out");
    obj_setInt(self, "isOn", 0);
    obj_setStr(self, "pull", "none");
    obj_setInt(self, "id", -999);
    obj_setPtr(self, "pika_dev", NULL);
}

int PikaStdDevice_GPIO_getId(PikaObj* self) {
    return obj_getInt(self, "id");
}

void PikaStdDevice_GPIO_setId(PikaObj* self, int id) {
    obj_setInt(self, "id", id);
}

void PikaStdDevice_GPIO___init__(PikaObj* self) {
    PikaStdDevice_GPIO_init(self);
}

void PikaStdDevice_GPIO_disable(PikaObj* self) {
    obj_setInt(self, "isEnable", 0);
    obj_runNativeMethod(self, "platformDisable", NULL);
}

void PikaStdDevice_GPIO_enable(PikaObj* self) {
    obj_setInt(self, "isEnable", 1);
    obj_runNativeMethod(self, "platformEnable", NULL);
}

char* PikaStdDevice_GPIO_getMode(PikaObj* self) {
    return obj_getStr(self, "mode");
}

char* PikaStdDevice_GPIO_getPin(PikaObj* self) {
    return obj_getStr(self, "pin");
}

void PikaStdDevice_GPIO_low(PikaObj* self) {
    obj_setInt(self, "isOn", 0);
    obj_runNativeMethod(self, "platformLow", NULL);
}

void PikaStdDevice_GPIO_high(PikaObj* self) {
    obj_setInt(self, "isOn", 1);
    obj_runNativeMethod(self, "platformHigh", NULL);
}

int PikaStdDevice_GPIO_read(PikaObj* self) {
    obj_runNativeMethod(self, "platformRead", NULL);
    return obj_getInt(self, "readBuff");
}

void PikaStdDevice_GPIO_setMode(PikaObj* self, char* mode) {
    if (strEqu(mode, "out") || strEqu(mode, "in")) {
        obj_setStr(self, "mode", mode);
        obj_runNativeMethod(self, "platformSetMode", NULL);
    } else {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] GPIO mode should be 'out' or 'in'.");
    }
}

void PikaStdDevice_GPIO_setPull(PikaObj* self, char* pull) {
    if (strEqu(pull, "up") || strEqu(pull, "down") || strEqu(pull, "none")) {
        obj_setStr(self, "pull", pull);
    } else {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self,
                      "[error] GPIO pull should be 'up', 'down' or 'none'.");
    }
}

static pika_dev* _get_dev(PikaObj* self){
   pika_dev* dev = obj_getPtr(self, "pika_dev");
   if(NULL != dev){
       return dev;
   }
   dev = pika_hal_open(PIKA_HAL_GPIO, obj_getStr(self, "pin"));
   if(NULL == dev){
       __platform_printf("Error: open GPIO '%s' failed.\r\n", obj_getStr(self, "pin"));
   }
   obj_setPtr(self, "pika_dev", dev);
   return dev;
}

void PikaStdDevice_GPIO_setPin(PikaObj* self, char* pinName) {
    obj_setStr(self, "pin", pinName);
}

void PikaStdDevice_GPIO_platformDisable(PikaObj* self) {
   pika_dev* dev = _get_dev(self);
   pika_hal_ioctl(dev, PIKA_HAL_IOCTL_DISABLE);
}

void PikaStdDevice_GPIO_platformEnable(PikaObj* self) {
   pika_dev* dev = _get_dev(self);
   pika_hal_ioctl(dev, PIKA_HAL_IOCTL_ENABLE);
}

void PikaStdDevice_GPIO_platformLow(PikaObj* self) {
   pika_dev* dev = _get_dev(self);
   uint32_t val = 0;
   pika_hal_write(dev, &val, sizeof(val));
}

void PikaStdDevice_GPIO_platformHigh(PikaObj* self) {
   pika_dev* dev = _get_dev(self);
   uint32_t val = 1;
   pika_hal_write(dev, &val, sizeof(val));
}

void PikaStdDevice_GPIO_platformSetMode(PikaObj* self) {
    char* mode = obj_getStr(self, "mode");
    pika_hal_GPIO_config cfg = {0};
    if(strEqu(mode, "in")){
        cfg.dir = PIKA_HAL_GPIO_DIR_IN;
    }else{
        cfg.dir = PIKA_HAL_GPIO_DIR_OUT;
    }
    pika_dev* dev = _get_dev(self);
    char* pull = obj_getStr(self, "pull");
    if (strEqu(pull, "up")){
        cfg.pull = PIKA_HAL_GPIO_PULL_UP;
    }
    if (strEqu(pull, "down")){
        cfg.pull = PIKA_HAL_GPIO_PULL_DOWN;
    }
    if (strEqu(pull, "none")) {
        cfg.pull = PIKA_HAL_GPIO_PULL_NONE;
    }
    pika_hal_ioctl(dev, PIKA_HAL_IOCTL_CONFIG, &cfg);
}

void PikaStdDevice_GPIO_platformRead(PikaObj* self) {
    pika_dev* dev = _get_dev(self);
    uint32_t val = 0;
    pika_hal_read(dev, &val, sizeof(val));
    obj_setInt(self, "readBuff", val);
}
