#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"
#include "PikaCompiler.h"
#include "PikaObj.h"
#include "PikaParser.h"
#include "dataStrs.h"
#include "libpikabinder.h"

/* fake implement */
PikaObj* __pikaMain;
void New_PikaStdLib_SysObj(void){}
void New_PikaStdData_List(void) {}
void New_PikaStdData_Dict(void) {}
void New_PikaStdData_Tuple(void) {}
void New_PikaStdData_String(void) {}
void New_PikaStdData_ByteArray(void) {}

void main() {
    /* run pika_binder to bind C modules */
    pika_binder();

    PikaMaker* maker = New_PikaMaker();
    pikaMaker_compileModuleWithDepends(maker, "main");
    pikaMaker_linkCompiledModules(maker, "pikaModules.py.a");
    obj_deinit(maker);

    return;
}
