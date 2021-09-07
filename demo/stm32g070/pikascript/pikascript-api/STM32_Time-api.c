/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "STM32_Time.h"
#include "PikaStdDeivce_Time.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void STM32_Time_sleep_msMethod(PikaObj *self, Args *args){
    int ms = args_getInt(args, "ms");
    STM32_Time_sleep_ms(self, ms);
}

void STM32_Time_sleep_sMethod(PikaObj *self, Args *args){
    int s = args_getInt(args, "s");
    STM32_Time_sleep_s(self, s);
}

PikaObj *New_STM32_Time(Args *args){
    PikaObj *self = New_PikaStdDeivce_Time(args);
    class_defineMethod(self, "sleep_ms(ms:int)", STM32_Time_sleep_msMethod);
    class_defineMethod(self, "sleep_s(s:int)", STM32_Time_sleep_sMethod);
    return self;
}
