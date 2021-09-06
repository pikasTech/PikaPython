/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdDeivce_GPIO.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdDeivce_GPIO_disableMethod(PikaObj *self, Args *args){
    PikaStdDeivce_GPIO_disable(self);
}

void PikaStdDeivce_GPIO_enableMethod(PikaObj *self, Args *args){
    PikaStdDeivce_GPIO_enable(self);
}

void PikaStdDeivce_GPIO_getModeMethod(PikaObj *self, Args *args){
    char * res = PikaStdDeivce_GPIO_getMode(self);
    method_returnStr(args, res);
}

void PikaStdDeivce_GPIO_getPinMethod(PikaObj *self, Args *args){
    char * res = PikaStdDeivce_GPIO_getPin(self);
    method_returnStr(args, res);
}

void PikaStdDeivce_GPIO_initMethod(PikaObj *self, Args *args){
    PikaStdDeivce_GPIO_init(self);
}

void PikaStdDeivce_GPIO_offMethod(PikaObj *self, Args *args){
    PikaStdDeivce_GPIO_off(self);
}

void PikaStdDeivce_GPIO_onMethod(PikaObj *self, Args *args){
    PikaStdDeivce_GPIO_on(self);
}

void PikaStdDeivce_GPIO_platformDisableMethod(PikaObj *self, Args *args){
    PikaStdDeivce_GPIO_platformDisable(self);
}

void PikaStdDeivce_GPIO_platformEnableMethod(PikaObj *self, Args *args){
    PikaStdDeivce_GPIO_platformEnable(self);
}

void PikaStdDeivce_GPIO_platformOffMethod(PikaObj *self, Args *args){
    PikaStdDeivce_GPIO_platformOff(self);
}

void PikaStdDeivce_GPIO_platformOnMethod(PikaObj *self, Args *args){
    PikaStdDeivce_GPIO_platformOn(self);
}

void PikaStdDeivce_GPIO_platformSetModeMethod(PikaObj *self, Args *args){
    char * mode = args_getStr(args, "mode");
    PikaStdDeivce_GPIO_platformSetMode(self, mode);
}

void PikaStdDeivce_GPIO_setModeMethod(PikaObj *self, Args *args){
    char * mode = args_getStr(args, "mode");
    PikaStdDeivce_GPIO_setMode(self, mode);
}

void PikaStdDeivce_GPIO_setPinMethod(PikaObj *self, Args *args){
    char * pinName = args_getStr(args, "pinName");
    PikaStdDeivce_GPIO_setPin(self, pinName);
}

PikaObj *New_PikaStdDeivce_GPIO(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "disable()", PikaStdDeivce_GPIO_disableMethod);
    class_defineMethod(self, "enable()", PikaStdDeivce_GPIO_enableMethod);
    class_defineMethod(self, "getMode()->str", PikaStdDeivce_GPIO_getModeMethod);
    class_defineMethod(self, "getPin()->str", PikaStdDeivce_GPIO_getPinMethod);
    class_defineMethod(self, "init()", PikaStdDeivce_GPIO_initMethod);
    class_defineMethod(self, "off()", PikaStdDeivce_GPIO_offMethod);
    class_defineMethod(self, "on()", PikaStdDeivce_GPIO_onMethod);
    class_defineMethod(self, "platformDisable()", PikaStdDeivce_GPIO_platformDisableMethod);
    class_defineMethod(self, "platformEnable()", PikaStdDeivce_GPIO_platformEnableMethod);
    class_defineMethod(self, "platformOff()", PikaStdDeivce_GPIO_platformOffMethod);
    class_defineMethod(self, "platformOn()", PikaStdDeivce_GPIO_platformOnMethod);
    class_defineMethod(self, "platformSetMode(mode:str)", PikaStdDeivce_GPIO_platformSetModeMethod);
    class_defineMethod(self, "setMode(mode:str)", PikaStdDeivce_GPIO_setModeMethod);
    class_defineMethod(self, "setPin(pinName:str)", PikaStdDeivce_GPIO_setPinMethod);
    return self;
}
