/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include <stdio.h>
#include <stdlib.h>

PikaObj * pikaScriptInit(){
    PikaObj * pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain,
            "\n"
            "mem = PikaStdLib.MemChecker()\n"
            "\n"
            "\n"
            "for i in range(1, 100):\n"
            "    print(i)\n"
            "\n"
            "print('mem used max:')\n"
            "mem.max()\n"
            "print('mem used now:')\n"
            "mem.now()\n"
            "\n");
    return pikaMain;
}

