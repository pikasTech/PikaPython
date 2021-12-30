#include "PikaStdLib_MemChecker.h"
#include "BaseObj.h"
#include "dataStrs.h"

void PikaStdLib_MemChecker_max(PikaObj* self) {
    obj_sysPrintf(self, "%0.2f kB", pikaMemMax() / 1024.0);
}

void PikaStdLib_MemChecker_now(PikaObj* self) {
    obj_sysPrintf(self, "%0.2f kB", pikaMemNow() / 1024.0);
}

void PikaStdLib_MemChecker_resetMax(PikaObj* self) {
    pikaMemMaxReset();
}

float PikaStdLib_MemChecker_getMax(PikaObj* self) {
    return pikaMemMax() / 1024.0;
}

float PikaStdLib_MemChecker_getNow(PikaObj* self) {
    return pikaMemNow() / 1024.0;
}