/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "Device.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void Device_LEDMethod(PikaObj *self, Args *args){
    Arg * res = Device_LED(self);
    method_returnArg(args, res);
}

void Device_UartMethod(PikaObj *self, Args *args){
    Arg * res = Device_Uart(self);
    method_returnArg(args, res);
}

PikaObj *New_Device(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "LED()->any", Device_LEDMethod);
    class_defineMethod(self, "Uart()->any", Device_UartMethod);
    return self;
}

