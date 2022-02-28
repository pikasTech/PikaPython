#include "pikaRTDevice_PWM.h"
#include <rtdevice.h>
#include <rtthread.h>

struct rt_device_pwm* PWM_getDevice(PikaObj* self) {
    char* name = obj_getStr(self, "name");
    rt_device_t pwm_device = rt_device_find(name);
    if(NULL == pwm_device){
        printf("[error] PWM: device \"%s\" no found.\r\n", name);
    }
    return (struct rt_device_pwm*)pwm_device;
}

void pikaRTDevice_PWM_platformEnable(PikaObj* self) {
    struct rt_device_pwm* pwm_dev = PWM_getDevice(self);
    int ch = obj_getInt(self, "ch");
    rt_pwm_enable(pwm_dev, ch);
}

void pikaRTDevice_PWM_platformSetDuty(PikaObj* self) {
    struct rt_device_pwm* pwm_dev = PWM_getDevice(self);
    int ch = obj_getInt(self, "ch");
    int freq = obj_getInt(self, "freq");
    float duty = obj_getFloat(self, "duty");
    int period = (int)((1.0 / (float)freq) * 1000 * 1000);
    int pulse = (int)((float)period * duty);
    rt_pwm_set(pwm_dev, ch, period, pulse);
}

void pikaRTDevice_PWM_platformSetFrequency(PikaObj* self) {
    pikaRTDevice_PWM_platformSetDuty(self);
}
