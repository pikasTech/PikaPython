/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include <stdio.h>
#include <stdlib.h>

PikaObj * pikaScriptInit(){
    PikaObj * pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain, "uart.init()");
    obj_run(pikaMain, "uart.setId(1)");
    obj_run(pikaMain, "uart.enable()");
    obj_run(pikaMain, "print('initing rgb...')");
    obj_run(pikaMain, "rgb.init()");
    obj_run(pikaMain, "rgb.enable()");
    obj_run(pikaMain, "print('init rgb ok!')");
    obj_run(pikaMain, "print('mem max:')");
    obj_run(pikaMain, "mem.max()");
    obj_run(pikaMain, "while True:");
    obj_run(pikaMain, "    time.sleep_ms(50)");
    obj_run(pikaMain, "    rgb.flow()");
    obj_run(pikaMain, "");
    return pikaMain;
}

