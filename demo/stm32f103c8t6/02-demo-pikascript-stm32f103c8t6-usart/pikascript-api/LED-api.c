/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "LED.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void LED_offMethod(PikaObj *self, Args *args){
    LED_off(self);
}

void LED_onMethod(PikaObj *self, Args *args){
    LED_on(self);
}

PikaObj *New_LED(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "off()", LED_offMethod);
    class_defineMethod(self, "on()", LED_onMethod);
    return self;
}
