/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "STM32_GPIO.h"
#include "PikaStdDevice_GPIO.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void STM32_GPIO_platformDisableMethod(PikaObj *self, Args *args){
    STM32_GPIO_platformDisable(self);
}

void STM32_GPIO_platformEnableMethod(PikaObj *self, Args *args){
    STM32_GPIO_platformEnable(self);
}

void STM32_GPIO_platformHighMethod(PikaObj *self, Args *args){
    STM32_GPIO_platformHigh(self);
}

void STM32_GPIO_platformLowMethod(PikaObj *self, Args *args){
    STM32_GPIO_platformLow(self);
}

void STM32_GPIO_platformSetModeMethod(PikaObj *self, Args *args){
    char * mode = args_getStr(args, "mode");
    STM32_GPIO_platformSetMode(self, mode);
}

PikaObj *New_STM32_GPIO(Args *args){
    PikaObj *self = New_PikaStdDevice_GPIO(args);
    class_defineMethod(self, "platformDisable()", STM32_GPIO_platformDisableMethod);
    class_defineMethod(self, "platformEnable()", STM32_GPIO_platformEnableMethod);
    class_defineMethod(self, "platformHigh()", STM32_GPIO_platformHighMethod);
    class_defineMethod(self, "platformLow()", STM32_GPIO_platformLowMethod);
    class_defineMethod(self, "platformSetMode(mode:str)", STM32_GPIO_platformSetModeMethod);
    return self;
}
