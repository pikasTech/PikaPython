/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "STM32_ADC.h"
#include "PikaStdDevice_ADC.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void STM32_ADC_platformEnableMethod(PikaObj *self, Args *args){
    char * pin = args_getStr(args, "pin");
    STM32_ADC_platformEnable(self, pin);
}

void STM32_ADC_platformReadMethod(PikaObj *self, Args *args){
    char * pin = args_getStr(args, "pin");
    float res = STM32_ADC_platformRead(self, pin);
    method_returnFloat(args, res);
}

PikaObj *New_STM32_ADC(Args *args){
    PikaObj *self = New_PikaStdDevice_ADC(args);
    class_defineMethod(self, "platformEnable(pin:str)", STM32_ADC_platformEnableMethod);
    class_defineMethod(self, "platformRead(pin:str)->float", STM32_ADC_platformReadMethod);
    return self;
}
