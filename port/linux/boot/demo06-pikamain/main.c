/* this demo shows the usage of method */
#include <stdio.h>
#include "PikaMain.h"
#include "PikaVM.h"
#include "pikaScript.h"

int main(int argc, char* argv[]) {
    if (1 == argc) {
        PikaObj* pikaMain = pikaScriptInit();
        pikaScriptShell(pikaMain);
        obj_deinit(pikaMain);
        return 0;
    }
    if (2 == argc) {
        PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
        __platform_printf("======[pikascript packages installed]======\r\n");
        pika_printVersion();
        __platform_printf("===========================================\r\n");
        pikaVM_runFile(pikaMain, argv[1]);
        obj_deinit(pikaMain);
        return 0;
    }
}
