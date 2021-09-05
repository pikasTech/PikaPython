/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include <stdio.h>
#include <stdlib.h>

PikaObj * pikaScriptInit(){
    PikaObj * pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain, "print('hello wrold')");
    obj_run(pikaMain, "uart.setName('com1')");
    obj_run(pikaMain, "uart.send('My name is:')");
    obj_run(pikaMain, "uart.printName()");
    obj_run(pikaMain, "print('mem used max:')");
    obj_run(pikaMain, "mem.max()");
    obj_run(pikaMain, "print('mem used now:')");
    obj_run(pikaMain, "mem.now()");
    return pikaMain;
}

