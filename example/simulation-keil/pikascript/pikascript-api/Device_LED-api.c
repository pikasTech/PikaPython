/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "Device_LED.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void Device_LED_offMethod(PikaObj *self, Args *args){
    Device_LED_off(self);
}

void Device_LED_onMethod(PikaObj *self, Args *args){
    Device_LED_on(self);
}

PikaObj *New_Device_LED(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "off()", Device_LED_offMethod);
    class_defineMethod(self, "on()", Device_LED_onMethod);
    return self;
}
