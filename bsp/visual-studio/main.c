#include <stdio.h>
#include "pikaScript.h"
#include "pikaVM.h"
int main(int argc, char* argv[])
{
    PikaObj* pikaMain = NULL;
    // pika.exe
    if (argc == 1) {
        pikaMain = pikaPythonInit();
    }

    // pika.exe xxx.py
    if (argc == 2) {
        Args buffs = { 0 };
        pikaMain = newRootObj("pikaMain", New_PikaMain);
        char* path = argv[1];
        char* filename = strsPathGetFileName(&buffs, path);
        size_t filename_size = strGetSize(filename);
        if (0 == strncmp(filename + filename_size - 3, ".py", 3)) {
            mkdir("pikascript-api");
            pikaVM_runFile(pikaMain, path);
        }
        else if(0 == strncmp(filename + filename_size - 5, ".py.a", 5)){
            obj_linkLibraryFile(pikaMain, path);
            obj_runModule(pikaMain, "main");
        }
        strsDeinit(&buffs);
    }

	pikaPythonShell(pikaMain);
}

