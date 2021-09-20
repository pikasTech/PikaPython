/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include <stdio.h>
#include <stdlib.h>

PikaObj * pikaScriptInit(){
    PikaObj * pikaMain = newRootObj("pikaMain", New_PikaMain);
<<<<<<< HEAD
    obj_run(pikaMain, "uart.init()");
    obj_run(pikaMain, "uart.setId()");
    obj_run(pikaMain, "uart.enable()");
    obj_run(pikaMain, "print('initing rgb...')");
    obj_run(pikaMain, "rgb.init()");
    obj_run(pikaMain, "rgb.enable()");
    obj_run(pikaMain, "print('init rgb ok!')");
    obj_run(pikaMain, "print('mem max:')");
    obj_run(pikaMain, "mem.max()");
    obj_run(pikaMain, "while True:");
    obj_run(pikaMain, "    time.sleep_ms(500)");
    obj_run(pikaMain, "    rgb.red()");
    obj_run(pikaMain, "    time.sleep_ms(500)");
    obj_run(pikaMain, "    rgb.red()");
=======
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
>>>>>>> 75642075742a22be2a2f4f20354f9ee028543b7c
    obj_run(pikaMain, "");
    return pikaMain;
}

