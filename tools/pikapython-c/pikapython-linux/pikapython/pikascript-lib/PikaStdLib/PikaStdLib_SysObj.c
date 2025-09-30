#include "PikaStdLib_SysObj.h"
#include "PikaStdData_FILEIO.h"
#include "PikaStdLib_REPL.h"
#include "PikaVM.h"
#include "dataStrs.h"

void PikaStdLib_REPL_setEcho(PikaObj* self, pika_bool echo) {
    pikaShellSetEcho(echo);
}
