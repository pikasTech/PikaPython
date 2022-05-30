#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"
#include "PikaCompiler.h"
#include "PikaObj.h"
#include "PikaParser.h"
#include "dataStrs.h"
#include "libpikabinder.h"

void main() {
    /* run pika_binder to bind C modules */
    pika_binder();

    PikaMaker* maker = New_PikaMaker();
    pikaMaker_compileModuleWithDepends(maker, "main");
    pikaMaker_linkCompiledModules(maker, "pikaModules.py.a");
    obj_deinit(maker);

    return;
}
