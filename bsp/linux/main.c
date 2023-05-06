#include "pikaScript.h"

int main(int argc, char *argv[])
{
    PikaObj* root = pikaScriptInit();
    pikaScriptShell(root);
    obj_deinit(root);
    return 0;
}
