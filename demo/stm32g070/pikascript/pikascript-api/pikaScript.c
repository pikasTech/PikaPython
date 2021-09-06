/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include <stdio.h>
#include <stdlib.h>

PikaObj * pikaScriptInit(){
    PikaObj * pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain, "print('hello world')");
    obj_run(pikaMain, "print('mem.max :')");
    obj_run(pikaMain, "mem.max()");
    obj_run(pikaMain, "print('mem.now :')");
    obj_run(pikaMain, "mem.now()");
    return pikaMain;
}

