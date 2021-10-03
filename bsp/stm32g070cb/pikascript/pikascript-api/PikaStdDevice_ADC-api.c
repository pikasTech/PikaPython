/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdDevice_ADC.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdDevice_ADC_enableMethod(PikaObj *self, Args *args){
    PikaStdDevice_ADC_enable(self);
}

void PikaStdDevice_ADC_initMethod(PikaObj *self, Args *args){
    PikaStdDevice_ADC_init(self);
}

void PikaStdDevice_ADC_platformEnableMethod(PikaObj *self, Args *args){
    char * pin = args_getStr(args, "pin");
    PikaStdDevice_ADC_platformEnable(self, pin);
}

void PikaStdDevice_ADC_platformReadMethod(PikaObj *self, Args *args){
    char * pin = args_getStr(args, "pin");
    float res = PikaStdDevice_ADC_platformRead(self, pin);
    method_returnFloat(args, res);
}

void PikaStdDevice_ADC_readMethod(PikaObj *self, Args *args){
    float res = PikaStdDevice_ADC_read(self);
    method_returnFloat(args, res);
}

void PikaStdDevice_ADC_setPinMethod(PikaObj *self, Args *args){
    char * pin = args_getStr(args, "pin");
    PikaStdDevice_ADC_setPin(self, pin);
}

PikaObj *New_PikaStdDevice_ADC(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "enable()", PikaStdDevice_ADC_enableMethod);
    class_defineMethod(self, "init()", PikaStdDevice_ADC_initMethod);
    class_defineMethod(self, "platformEnable(pin:str)", PikaStdDevice_ADC_platformEnableMethod);
    class_defineMethod(self, "platformRead(pin:str)->float", PikaStdDevice_ADC_platformReadMethod);
    class_defineMethod(self, "read()->float", PikaStdDevice_ADC_readMethod);
    class_defineMethod(self, "setPin(pin:str)", PikaStdDevice_ADC_setPinMethod);
    return self;
}
