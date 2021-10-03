/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "STM32_PWM.h"
#include "PikaStdDevice_PWM.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void STM32_PWM_platformEnableMethod(PikaObj *self, Args *args){
    float duty = args_getFloat(args, "duty");
    int freq = args_getInt(args, "freq");
    char * pin = args_getStr(args, "pin");
    STM32_PWM_platformEnable(self, duty, freq, pin);
}

void STM32_PWM_platformSetDutyMethod(PikaObj *self, Args *args){
    float duty = args_getFloat(args, "duty");
    char * pin = args_getStr(args, "pin");
    STM32_PWM_platformSetDuty(self, duty, pin);
}

void STM32_PWM_platformSetFrequencyMethod(PikaObj *self, Args *args){
    int freq = args_getInt(args, "freq");
    char * pin = args_getStr(args, "pin");
    STM32_PWM_platformSetFrequency(self, freq, pin);
}

PikaObj *New_STM32_PWM(Args *args){
    PikaObj *self = New_PikaStdDevice_PWM(args);
    class_defineMethod(self, "platformEnable(pin:str,freq:int,duty:float)", STM32_PWM_platformEnableMethod);
    class_defineMethod(self, "platformSetDuty(pin:str,duty:float)", STM32_PWM_platformSetDutyMethod);
    class_defineMethod(self, "platformSetFrequency(pin:str,freq:int)", STM32_PWM_platformSetFrequencyMethod);
    return self;
}
