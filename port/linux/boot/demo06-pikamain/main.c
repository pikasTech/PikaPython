/* this demo shows the usage of method */
#include "pikaScript.h"
#include <stdio.h>

void obj_runWithInfo(PikaObj *self, char *cmd)
{
    printf(">>> %s\r\n", cmd);
    obj_run(self, cmd);
}

int main()
{
    PikaObj *pikaMain = pikaScriptInit();
    /* user input buff */
    char inputBuff[256] = {0};
    /* run the script with check*/
    printf(">>> ");
    while (1)
    {
        /* get user input */
        fgets(inputBuff, sizeof(inputBuff), stdin);

        /* run PikaScript and get res */
        PikaObj *globals = obj_runDirect(pikaMain, inputBuff);

        /* get system output of PikaScript*/
        char *sysOut = args_getSysOut(globals->list);

        if (!strEqu("", sysOut))
        {
            /* print out the system output */
            printf("%s\r\n", sysOut);
        }
        printf(">>> ");

        /* deinit the res */
        // obj_deinit(globals);
    }
}
