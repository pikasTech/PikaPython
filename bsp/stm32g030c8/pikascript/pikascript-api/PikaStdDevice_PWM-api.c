/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdDevice_PWM.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdDevice_PWM_enableMethod(PikaObj *self, Args *args){
    PikaStdDevice_PWM_enable(self);
}

void PikaStdDevice_PWM_getDutyMethod(PikaObj *self, Args *args){
    float res = PikaStdDevice_PWM_getDuty(self);
    method_returnFloat(args, res);
}

void PikaStdDevice_PWM_getFrequencyMethod(PikaObj *self, Args *args){
    int res = PikaStdDevice_PWM_getFrequency(self);
    method_returnInt(args, res);
}

void PikaStdDevice_PWM_initMethod(PikaObj *self, Args *args){
    PikaStdDevice_PWM_init(self);
}

void PikaStdDevice_PWM_platformEnableMethod(PikaObj *self, Args *args){
    float duty = args_getFloat(args, "duty");
    int freq = args_getInt(args, "freq");
    char * pin = args_getStr(args, "pin");
    PikaStdDevice_PWM_platformEnable(self, duty, freq, pin);
}

void PikaStdDevice_PWM_platformSetDutyMethod(PikaObj *self, Args *args){
    float duty = args_getFloat(args, "duty");
    char * pin = args_getStr(args, "pin");
    PikaStdDevice_PWM_platformSetDuty(self, duty, pin);
}

void PikaStdDevice_PWM_platformSetFrequencyMethod(PikaObj *self, Args *args){
    int freq = args_getInt(args, "freq");
    char * pin = args_getStr(args, "pin");
    PikaStdDevice_PWM_platformSetFrequency(self, freq, pin);
}

void PikaStdDevice_PWM_setDutyMethod(PikaObj *self, Args *args){
    float duty = args_getFloat(args, "duty");
    PikaStdDevice_PWM_setDuty(self, duty);
}

void PikaStdDevice_PWM_setFrequencyMethod(PikaObj *self, Args *args){
    int freq = args_getInt(args, "freq");
    PikaStdDevice_PWM_setFrequency(self, freq);
}

void PikaStdDevice_PWM_setPinMethod(PikaObj *self, Args *args){
    char * pin = args_getStr(args, "pin");
    PikaStdDevice_PWM_setPin(self, pin);
}

PikaObj *New_PikaStdDevice_PWM(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "enable()", PikaStdDevice_PWM_enableMethod);
    class_defineMethod(self, "getDuty()->float", PikaStdDevice_PWM_getDutyMethod);
    class_defineMethod(self, "getFrequency()->int", PikaStdDevice_PWM_getFrequencyMethod);
    class_defineMethod(self, "init()", PikaStdDevice_PWM_initMethod);
    class_defineMethod(self, "platformEnable(pin:str,freq:int,duty:float)", PikaStdDevice_PWM_platformEnableMethod);
    class_defineMethod(self, "platformSetDuty(pin:str,duty:float)", PikaStdDevice_PWM_platformSetDutyMethod);
    class_defineMethod(self, "platformSetFrequency(pin:str,freq:int)", PikaStdDevice_PWM_platformSetFrequencyMethod);
    class_defineMethod(self, "setDuty(duty:float)", PikaStdDevice_PWM_setDutyMethod);
    class_defineMethod(self, "setFrequency(freq:int)", PikaStdDevice_PWM_setFrequencyMethod);
    class_defineMethod(self, "setPin(pin:str)", PikaStdDevice_PWM_setPinMethod);
    return self;
}
