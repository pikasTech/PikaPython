#include "pikaRTDevice_GPIO.h"
#include <rtdevice.h>
#include <rtthread.h>
rt_base_t pika_get_rt_mode_num(char* mode, char* pull);

rt_base_t GPIO_get_pin_num(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    int id = obj_getInt(self, "id");
    if (!strEqu("none", pin)) {
        /* pin name exist */
        return rt_pin_get(pin);
    } else {
        return id;
    }
}

void pikaRTDevice_GPIO_platformEnable(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    char* mode = obj_getStr(self, "mode");
    char* pull = obj_getStr(self, "pull");
    int id = obj_getInt(self, "id");
    rt_base_t pin_num = GPIO_get_pin_num(self);
    if (pin_num < 0) {
        obj_setSysOut(self,
                      "[error]: gpio hardware fault, can not get pin number.");
        obj_setErrorCode(self, 1);
    }
    rt_base_t mode_num = pika_get_rt_mode_num(mode, pull);
    rt_pin_mode(pin_num, mode_num);
}

rt_base_t pika_get_rt_mode_num(char* mode, char* pull) {
    if (strEqu(mode, "out")) {
        return PIN_MODE_OUTPUT;
    }
    if (strEqu(mode, "in")) {
        if (strEqu(pull, "none")) {
            return PIN_MODE_INPUT;
        }
        if (strEqu(pull, "up")) {
            return PIN_MODE_INPUT_PULLUP;
        }
        if (strEqu(pull, "down")) {
            return PIN_MODE_INPUT_PULLDOWN;
        }
    }
    /* default */
    return PIN_MODE_OUTPUT;
}

void pikaRTDevice_GPIO_platformDisable(PikaObj* self) {}
void pikaRTDevice_GPIO_platformHigh(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    rt_base_t pin_num = GPIO_get_pin_num(self);
    rt_pin_write(pin_num, PIN_HIGH);
}
void pikaRTDevice_GPIO_platformLow(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    rt_base_t pin_num = GPIO_get_pin_num(self);
    rt_pin_write(pin_num, PIN_LOW);
}
void pikaRTDevice_GPIO_platformRead(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    rt_base_t pin_num = GPIO_get_pin_num(self);
    obj_setInt(self, "readBuff", rt_pin_read(pin_num));
}
void pikaRTDevice_GPIO_platformSetMode(PikaObj* self) {
    pikaRTDevice_GPIO_platformEnable(self);
}
