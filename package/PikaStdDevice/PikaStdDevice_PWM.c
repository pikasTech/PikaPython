#include "PikaStdDevice_PWM.h"
#include "BaseObj.h"

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
    obj_runNativeMethod(self, "platformSetFrequency",NULL);
}

void PikaStdDevice_PWM_setDuty(PikaObj* self, double duty) {
    obj_setFloat(self, "duty", duty);
    obj_runNativeMethod(self, "platformSetDuty",NULL);
}

void PikaStdDevice_PWM_enable(PikaObj* self) {
    obj_runNativeMethod(self, "platformEnable",NULL);
}

void PikaStdDevice_PWM_disable(PikaObj *self){
    obj_runNativeMethod(self, "platformDisable",NULL);
}

double PikaStdDevice_PWM_getDuty(PikaObj* self) {
    return obj_getFloat(self, "duty");
}

int PikaStdDevice_PWM_getFrequency(PikaObj* self) {
    return obj_getInt(self, "freq");
}

void PikaStdDevice_PWM_platformEnable(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}
void PikaStdDevice_PWM_platformSetDuty(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}
void PikaStdDevice_PWM_platformSetFrequency(PikaObj* self) {
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
}

void PikaStdDevice_PWM_platformDisable(PikaObj *self){
    ABSTRACT_METHOD_NEED_OVERRIDE_ERROR();
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
