#ifndef __PIKA_PALTFORM__H
#define __PIKA_PALTFORM__H
#include <stdlib.h>
#include <stdint.h>

#if defined(__CC_ARM) || defined(__CLANG_ARM) /* ARM Compiler */
#define PIKA_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__) /* for IAR Compiler */
#define PIKA_WEAK __weak
#elif defined(__GNUC__) /* GNU GCC Compiler */
#define PIKA_WEAK __attribute__((weak))
#endif

void __platformPrintf(char* fmt, ...);
void* __platformMalloc(size_t size);
void __platformFree(void* ptr);
void __platformEnableIrqHandle();
void __platformDisableIrqHandle();

char* __platformLoadPikaAsm();
int32_t __platformSavePikaAsm(char *PikaAsm);
uint8_t __platformAsmIsToFlash(char *pyMultiLine);
int32_t __platformSavePikaAsmEOF();

#endif