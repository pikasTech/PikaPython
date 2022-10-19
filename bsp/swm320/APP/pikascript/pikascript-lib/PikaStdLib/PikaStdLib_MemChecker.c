#include "PikaStdLib_MemChecker.h"
#include "BaseObj.h"
#include "dataStrs.h"

void PikaStdLib_MemChecker_max(PikaObj* self) {
    __platform_printf("%0.2f kB\r\n", pikaMemMax() / 1024.0);
}

void PikaStdLib_MemChecker_now(PikaObj* self) {
    __platform_printf("%0.2f kB\r\n", pikaMemNow() / 1024.0);
}

void PikaStdLib_MemChecker_resetMax(PikaObj* self) {
    pikaMemMaxReset();
}

pika_float PikaStdLib_MemChecker_getMax(PikaObj* self) {
    return pikaMemMax() / 1024.0;
}

pika_float PikaStdLib_MemChecker_getNow(PikaObj* self) {
    return pikaMemNow() / 1024.0;
}
