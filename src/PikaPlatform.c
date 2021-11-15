#include "PikaPlatform.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

PIKA_WEAK void __platformDisableIrqHandle(void) {
    /* disable irq to support thread */
}

PIKA_WEAK void __platformEnableIrqHandle(void) {
    /* disable irq to support thread */
}

PIKA_WEAK void* __platformMalloc(size_t size) {
    return malloc(size);
}

PIKA_WEAK void __platformFree(void* ptr) {
    free(ptr);
}

PIKA_WEAK void* __impl_pikaMalloc(size_t size) {
    return __platformMalloc(size);
}
PIKA_WEAK void __impl_pikaFree(void* ptrm, size_t size) {
    __platformFree(ptrm);
}

PIKA_WEAK void __platformPrintf(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

PIKA_WEAK char* __platformLoadPikaAsm(void) {
    /* faild */
    return NULL;
}

PIKA_WEAK int32_t __platformSavePikaAsm(char* PikaAsm) {
    /* faild */
    return 1;
}

PIKA_WEAK uint8_t __platformAsmIsToFlash(char* pyMultiLine) {
    /* not to flash */
    return 0;
}

PIKA_WEAK int32_t __platformSavePikaAsmEOF(void) {
    return 1;
}
