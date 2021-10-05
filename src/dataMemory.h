/*
  Author: lyon
  Tencent QQ: 645275593
*/

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(__CC_ARM) || defined(__CLANG_ARM)           /* ARM Compiler */
#define PIKA_WEAK                     __attribute__((weak))
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
#define PIKA_WEAK                     __weak
#elif defined (__GNUC__)                /* GNU GCC Compiler */
#define PIKA_WEAK                     __attribute__((weak))
#endif

#define pika_printf(...)    printf(__VA_ARGS__)

typedef struct {
    uint32_t heapUsed;
    uint32_t heapUsedMax;
} PikaMemInfo;

void pikaFree(void* mem, uint32_t size);
void* pikaMalloc(uint32_t size);
uint16_t pikaMemNow(void);
uint16_t pikaMemMax(void);
void pikaMemMaxReset(void);
#endif
