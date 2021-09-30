/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include <stdio.h>
#include <stdlib.h>

PikaObj * pikaScriptInit(){
    PikaObj * pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain, "uart.init()");
    obj_run(pikaMain, "uart.setBaudRate(115200)");
    obj_run(pikaMain, "uart.setId(1)");
    obj_run(pikaMain, "uart.enable()");
    obj_run(pikaMain, "adc.init()");
    obj_run(pikaMain, "adc.setPin('PA0')");
    obj_run(pikaMain, "adc.enable()");
    obj_run(pikaMain, "pwm.init()");
    obj_run(pikaMain, "pwm.setPin('PA8')");
    obj_run(pikaMain, "pwm.setFrequency(2000)");
    obj_run(pikaMain, "pwm.setDuty(0.3)");
    obj_run(pikaMain, "pwm.enable()");
    obj_run(pikaMain, "while True:");
    obj_run(pikaMain, "    time.sleep_ms(500)");
    obj_run(pikaMain, "    pwm.setDuty(0.8)");
    obj_run(pikaMain, "    readBuff = uart.read(2)");
    obj_run(pikaMain, "    print('mem used max:')");
    obj_run(pikaMain, "    mem.max()");
    obj_run(pikaMain, "    print('read 2 char:')");
    obj_run(pikaMain, "    print(readBuff)");
    obj_run(pikaMain, "    print(' ')");
    obj_run(pikaMain, "    time.sleep_ms(500)");
    obj_run(pikaMain, "    pwm.setDuty(0.05)");
    obj_run(pikaMain, "    adcVal = adc.read()");
    obj_run(pikaMain, "    print('adc value:')");
    obj_run(pikaMain, "    print(adcVal)");
    obj_run(pikaMain, "");
    return pikaMain;
}

