/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdDevice_Time.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdDevice_Time_sleep_msMethod(PikaObj *self, Args *args){
    int ms = args_getInt(args, "ms");
    PikaStdDevice_Time_sleep_ms(self, ms);
}

void PikaStdDevice_Time_sleep_sMethod(PikaObj *self, Args *args){
    int s = args_getInt(args, "s");
    PikaStdDevice_Time_sleep_s(self, s);
}

PikaObj *New_PikaStdDevice_Time(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "sleep_ms(ms:int)", PikaStdDevice_Time_sleep_msMethod);
    class_defineMethod(self, "sleep_s(s:int)", PikaStdDevice_Time_sleep_sMethod);
    return self;
}
