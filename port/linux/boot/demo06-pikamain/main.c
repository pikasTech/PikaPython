/* this demo shows the usage of method */
#include <stdio.h>
#include "PikaMain.h"
#include "PikaVM.h"
#include "pikaScript.h"

int main(int argc, char* argv[]) {
    if (1 == argc) {
        pikaScriptShell(pikaScriptInit());
        return 0;
    }
    if (2 == argc) {
        PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
        pikaVM_runFile(pikaMain, argv[1]);
        obj_deinit(pikaMain);
        return 0;
    }
}
