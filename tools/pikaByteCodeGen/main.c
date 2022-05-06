#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"
#include "PikaCompiler.h"
#include "PikaObj.h"
#include "PikaParser.h"
#include "dataStrs.h"
#include "libpikabinder.h"

PikaObj* __pikaMain;



void main() {
    /* run pika_binder to bind C modules */
    pika_binder();
    return;
}
