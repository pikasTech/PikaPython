/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include <stdio.h>
#include <stdlib.h>

PikaObj * pikaScriptInit(){
    PikaObj * pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain, "print('hello pikascript')");
    obj_run(pikaMain, "io1.init()");
    obj_run(pikaMain, "io1.setPin('PB4')");
    obj_run(pikaMain, "io1.setMode('out')");
    obj_run(pikaMain, "io1.enable()");
    obj_run(pikaMain, "io1.low()");
    obj_run(pikaMain, "print('mem.max :')");
    obj_run(pikaMain, "mem.max()");
    obj_run(pikaMain, "print('mem.now :')");
    obj_run(pikaMain, "mem.now()");
    obj_run(pikaMain, "while True:");
    obj_run(pikaMain, "    io1.low()");
    obj_run(pikaMain, "    io1.high()");
    obj_run(pikaMain, "");
    return pikaMain;
}

