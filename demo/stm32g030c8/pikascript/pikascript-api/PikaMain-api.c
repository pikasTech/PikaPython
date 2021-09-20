/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include "PikaStdLib_SysObj.h"
<<<<<<< HEAD
#include "PikaStdLib_MemChecker.h"
#include "PikaPiZero_RGB.h"
=======
#include "STM32_PWM.h"
>>>>>>> 75642075742a22be2a2f4f20354f9ee028543b7c
#include "STM32_Time.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

PikaObj *New_PikaMain(Args *args){
    PikaObj *self = New_PikaStdLib_SysObj(args);
<<<<<<< HEAD
    obj_import(self, "PikaStdLib_MemChecker", New_PikaStdLib_MemChecker);
    obj_newObj(self, "mem", "PikaStdLib_MemChecker");
    obj_import(self, "PikaPiZero_RGB", New_PikaPiZero_RGB);
    obj_newObj(self, "rgb", "PikaPiZero_RGB");
=======
    obj_import(self, "STM32_PWM", New_STM32_PWM);
    obj_newObj(self, "pwm", "STM32_PWM");
>>>>>>> 75642075742a22be2a2f4f20354f9ee028543b7c
    obj_import(self, "STM32_Time", New_STM32_Time);
    obj_newObj(self, "time", "STM32_Time");
    return self;
}
