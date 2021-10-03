/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include <stdio.h>
#include <stdlib.h>

PikaObj * pikaScriptInit(){
    PikaObj * pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain, "mem = PikaStdLib.MemChecker()");
    obj_run(pikaMain, "time = STM32.Time()");
    obj_run(pikaMain, "uart = STM32.UART()");
    obj_run(pikaMain, "uart.init()");
    obj_run(pikaMain, "uart.setBaudRate(115200)");
    obj_run(pikaMain, "uart.setId(1)");
    obj_run(pikaMain, "uart.enable()");
    obj_run(pikaMain, "while True:");
    obj_run(pikaMain, "    time.sleep_s(1)");
    obj_run(pikaMain, "    readBuff = uart.read(2);");
    obj_run(pikaMain, "    print('mem used max:')");
    obj_run(pikaMain, "    mem.max()");
    obj_run(pikaMain, "    print('read 2 char:')");
    obj_run(pikaMain, "    print(readBuff)");
    obj_run(pikaMain, "    print(' ')");
    obj_run(pikaMain, "");
    return pikaMain;
}

