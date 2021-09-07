/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdDeivce_Time.h"
#include "TinyObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdDeivce_Time_sleep_msMethod(PikaObj *self, Args *args){
    int ms = args_getInt(args, "ms");
    PikaStdDeivce_Time_sleep_ms(self, ms);
}

void PikaStdDeivce_Time_sleep_sMethod(PikaObj *self, Args *args){
    int s = args_getInt(args, "s");
    PikaStdDeivce_Time_sleep_s(self, s);
}

PikaObj *New_PikaStdDeivce_Time(Args *args){
    PikaObj *self = New_TinyObj(args);
    class_defineMethod(self, "sleep_ms(ms:int)", PikaStdDeivce_Time_sleep_msMethod);
    class_defineMethod(self, "sleep_s(s:int)", PikaStdDeivce_Time_sleep_sMethod);
    return self;
}
