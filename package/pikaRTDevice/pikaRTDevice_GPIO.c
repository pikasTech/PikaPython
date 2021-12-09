#include "pikaRTDevice_GPIO.h"
#include <rtthread.h>
#include <rtdevice.h>
rt_base_t pika_get_rt_mode_num(char *mode, char* pull);
void pikaRTDevice_GPIO_platformEnable(PikaObj *self){
    char* pin = obj_getStr(self, "pin");
    char* mode = obj_getStr(self, "mode");
    char* pull = obj_getStr(self, "pull");
    rt_base_t pin_num = rt_pin_get(pin);
    if(pin_num < 0){
        obj_setSysOut(self, "[error]: gpio hardware fault, can not get pin number.");
        obj_setErrorCode(self, 1);
    }
    rt_base_t mode_num = pika_get_rt_mode_num(mode, pull);
    rt_pin_mode(pin_num, mode_num);
}

rt_base_t pika_get_rt_mode_num(char *mode, char* pull){
    if(strEqu(mode, "out")){
        return PIN_MODE_OUTPUT;
    }
    if(strEqu(mode, "in")){
        if(strEqu(pull, "none")){
            return PIN_MODE_INPUT;
        }
        if(strEqu(pull, "up")){
            return PIN_MODE_INPUT_PULLUP;
        }
        if(strEqu(pull, "down")){
            return PIN_MODE_INPUT_PULLDOWN;
        }
    }
    /* default */
    return PIN_MODE_OUTPUT;
}

void pikaRTDevice_GPIO_platformDisable(PikaObj *self){

}
void pikaRTDevice_GPIO_platformHigh(PikaObj *self){
    char* pin = obj_getStr(self, "pin");
    rt_base_t pin_num = rt_pin_get(pin);
    rt_pin_write(pin_num, PIN_HIGH);
}
void pikaRTDevice_GPIO_platformLow(PikaObj *self){
    char* pin = obj_getStr(self, "pin");
    rt_base_t pin_num = rt_pin_get(pin);
    rt_pin_write(pin_num, PIN_LOW);
}
void pikaRTDevice_GPIO_platformRead(PikaObj *self){
    char* pin = obj_getStr(self, "pin");
    rt_base_t pin_num = rt_pin_get(pin);
    obj_setInt(self, "readBuff", rt_pin_read(pin_num));
}
void pikaRTDevice_GPIO_platformSetMode(PikaObj *self){
    pikaRTDevice_GPIO_platformEnable(self);
}
