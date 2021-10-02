/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaPiZero_RGB.h"
#include "PikaStdLib_SysObj.h"
#include "STM32_GPIO.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaPiZero_RGB_blueMethod(PikaObj *self, Args *args){
    PikaPiZero_RGB_blue(self);
}

void PikaPiZero_RGB_enableMethod(PikaObj *self, Args *args){
    PikaPiZero_RGB_enable(self);
}

void PikaPiZero_RGB_flowMethod(PikaObj *self, Args *args){
    PikaPiZero_RGB_flow(self);
}

void PikaPiZero_RGB_greenMethod(PikaObj *self, Args *args){
    PikaPiZero_RGB_green(self);
}

void PikaPiZero_RGB_initMethod(PikaObj *self, Args *args){
    PikaPiZero_RGB_init(self);
}

void PikaPiZero_RGB_redMethod(PikaObj *self, Args *args){
    PikaPiZero_RGB_red(self);
}

void PikaPiZero_RGB_whiteMethod(PikaObj *self, Args *args){
    PikaPiZero_RGB_white(self);
}

PikaObj *New_PikaPiZero_RGB(Args *args){
    PikaObj *self = New_PikaStdLib_SysObj(args);
    obj_import(self, "STM32_GPIO", New_STM32_GPIO);
    obj_newObj(self, "pin", "STM32_GPIO");
    class_defineMethod(self, "blue()", PikaPiZero_RGB_blueMethod);
    class_defineMethod(self, "enable()", PikaPiZero_RGB_enableMethod);
    class_defineMethod(self, "flow()", PikaPiZero_RGB_flowMethod);
    class_defineMethod(self, "green()", PikaPiZero_RGB_greenMethod);
    class_defineMethod(self, "init()", PikaPiZero_RGB_initMethod);
    class_defineMethod(self, "red()", PikaPiZero_RGB_redMethod);
    class_defineMethod(self, "white()", PikaPiZero_RGB_whiteMethod);
    return self;
}
