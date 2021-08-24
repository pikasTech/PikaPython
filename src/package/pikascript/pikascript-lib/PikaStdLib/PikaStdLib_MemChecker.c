#include "dataStrs.h"
#include "BaseObj.h"

void PikaStdLib_MemChecker_max(PikaObj *self)
{
    printf("%0.2f kB\r\n", pikaMemMax() / 1024.0);
}

void PikaStdLib_MemChecker_now(PikaObj *self)
{
    printf("%0.2f kB\r\n", pikaMemNow() / 1024.0);
}

void PikaStdLib_MemChecker_resetMax(PikaObj *self)
{
    pikaMemMaxReset();
}
