#include "PikaStdDevice_PWM.h"
#include "BaseObj.h"

void PikaStdDevice_PWM_init(PikaObj* self) {
    obj_setStr(self, "pin", "PA8");
    obj_setInt(self, "freq", 1000);
    obj_setFloat(self, "duty", 0.5f);
}

void PikaStdDevice_PWM_setPin(PikaObj* self, char* pin) {
    obj_setStr(self, "pin", pin);
}

void PikaStdDevice_PWM_setFrequency(PikaObj* self, int freq) {
    obj_setInt(self, "freq", freq);
    obj_run(self, "platformSetFrequency(pin, freq)");
}

void PikaStdDevice_PWM_setDuty(PikaObj* self, float duty) {
    obj_setFloat(self, "duty", duty);
    obj_run(self, "platformSetDuty(pin, duty)");
}

void PikaStdDevice_PWM_enable(PikaObj* self) {
    obj_run(self, "platformEnable(pin, freq, duty)");
}

float PikaStdDevice_PWM_getDuty(PikaObj* self) {
    return obj_getFloat(self, "duty");
}

int PikaStdDevice_PWM_getFrequency(PikaObj* self) {
    return obj_getInt(self, "freq");
}

void PikaStdDevice_PWM_platformEnable(PikaObj* self,
                                      float dute,
                                      int freq,
                                      char* pin) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}
void PikaStdDevice_PWM_platformSetDuty(PikaObj* self, float duty, char* pin) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}
void PikaStdDevice_PWM_platformSetFrequency(PikaObj* self,
                                            int freq,
                                            char* pin) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}
