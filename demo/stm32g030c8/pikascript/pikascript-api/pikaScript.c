/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include <stdio.h>
#include <stdlib.h>

PikaObj * pikaScriptInit(){
    PikaObj * pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain, "pwm.init()");
    obj_run(pikaMain, "pwm.setPin('PA8')");
    obj_run(pikaMain, "pwm.setFrequency(2000)");
    obj_run(pikaMain, "pwm.setDuty(0.5)");
    obj_run(pikaMain, "pwm.enable()");
    obj_run(pikaMain, "while True:");
    obj_run(pikaMain, "    time.sleep_ms(500)");
    obj_run(pikaMain, "    pwm.setDuty(0.5)");
    obj_run(pikaMain, "    time.sleep_ms(500)");
    obj_run(pikaMain, "    pwm.setDuty(0.3)");
    obj_run(pikaMain, "");
    return pikaMain;
}

