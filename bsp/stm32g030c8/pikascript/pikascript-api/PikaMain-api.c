/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include "PikaStdLib_SysObj.h"
#include "STM32_ADC.h"
#include "PikaStdLib_MemChecker.h"
#include "STM32_GPIO.h"
#include "STM32_PWM.h"
#include "PikaPiZero_RGB.h"
#include "STM32_Time.h"
#include "STM32_UART.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

PikaObj *New_PikaMain(Args *args){
    PikaObj *self = New_PikaStdLib_SysObj(args);
    obj_import(self, "STM32_ADC", New_STM32_ADC);
    obj_import(self, "PikaStdLib_MemChecker", New_PikaStdLib_MemChecker);
    obj_import(self, "STM32_GPIO", New_STM32_GPIO);
    obj_import(self, "STM32_PWM", New_STM32_PWM);
    obj_import(self, "PikaPiZero_RGB", New_PikaPiZero_RGB);
    obj_import(self, "STM32_Time", New_STM32_Time);
    obj_import(self, "STM32_UART", New_STM32_UART);
    return self;
}
