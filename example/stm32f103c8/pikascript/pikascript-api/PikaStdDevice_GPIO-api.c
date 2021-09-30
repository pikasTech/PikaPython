/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdDevice_GPIO.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdDevice_GPIO_disableMethod(PikaObj *self, Args *args){
    PikaStdDevice_GPIO_disable(self);
}

void PikaStdDevice_GPIO_enableMethod(PikaObj *self, Args *args){
    PikaStdDevice_GPIO_enable(self);
}

void PikaStdDevice_GPIO_getModeMethod(PikaObj *self, Args *args){
    char * res = PikaStdDevice_GPIO_getMode(self);
    method_returnStr(args, res);
}

void PikaStdDevice_GPIO_getPinMethod(PikaObj *self, Args *args){
    char * res = PikaStdDevice_GPIO_getPin(self);
    method_returnStr(args, res);
}

void PikaStdDevice_GPIO_highMethod(PikaObj *self, Args *args){
    PikaStdDevice_GPIO_high(self);
}

void PikaStdDevice_GPIO_initMethod(PikaObj *self, Args *args){
    PikaStdDevice_GPIO_init(self);
}

void PikaStdDevice_GPIO_lowMethod(PikaObj *self, Args *args){
    PikaStdDevice_GPIO_low(self);
}

void PikaStdDevice_GPIO_platformDisableMethod(PikaObj *self, Args *args){
    PikaStdDevice_GPIO_platformDisable(self);
}

void PikaStdDevice_GPIO_platformEnableMethod(PikaObj *self, Args *args){
    PikaStdDevice_GPIO_platformEnable(self);
}

void PikaStdDevice_GPIO_platformHighMethod(PikaObj *self, Args *args){
    PikaStdDevice_GPIO_platformHigh(self);
}

void PikaStdDevice_GPIO_platformLowMethod(PikaObj *self, Args *args){
    PikaStdDevice_GPIO_platformLow(self);
}

void PikaStdDevice_GPIO_platformSetModeMethod(PikaObj *self, Args *args){
    char * mode = args_getStr(args, "mode");
    PikaStdDevice_GPIO_platformSetMode(self, mode);
}

void PikaStdDevice_GPIO_setModeMethod(PikaObj *self, Args *args){
    char * mode = args_getStr(args, "mode");
    PikaStdDevice_GPIO_setMode(self, mode);
}

void PikaStdDevice_GPIO_setPinMethod(PikaObj *self, Args *args){
    char * pinName = args_getStr(args, "pinName");
    PikaStdDevice_GPIO_setPin(self, pinName);
}

PikaObj *New_PikaStdDevice_GPIO(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "disable()", PikaStdDevice_GPIO_disableMethod);
    class_defineMethod(self, "enable()", PikaStdDevice_GPIO_enableMethod);
    class_defineMethod(self, "getMode()->str", PikaStdDevice_GPIO_getModeMethod);
    class_defineMethod(self, "getPin()->str", PikaStdDevice_GPIO_getPinMethod);
    class_defineMethod(self, "high()", PikaStdDevice_GPIO_highMethod);
    class_defineMethod(self, "init()", PikaStdDevice_GPIO_initMethod);
    class_defineMethod(self, "low()", PikaStdDevice_GPIO_lowMethod);
    class_defineMethod(self, "platformDisable()", PikaStdDevice_GPIO_platformDisableMethod);
    class_defineMethod(self, "platformEnable()", PikaStdDevice_GPIO_platformEnableMethod);
    class_defineMethod(self, "platformHigh()", PikaStdDevice_GPIO_platformHighMethod);
    class_defineMethod(self, "platformLow()", PikaStdDevice_GPIO_platformLowMethod);
    class_defineMethod(self, "platformSetMode(mode:str)", PikaStdDevice_GPIO_platformSetModeMethod);
    class_defineMethod(self, "setMode(mode:str)", PikaStdDevice_GPIO_setModeMethod);
    class_defineMethod(self, "setPin(pinName:str)", PikaStdDevice_GPIO_setPinMethod);
    return self;
}
