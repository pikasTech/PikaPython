#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
    uint32_t heapUsed;
    uint32_t heapUsedMax;
} PikaMemInfo;

void pikaFree(void *mem, uint32_t size);
void *pikaMalloc(uint32_t size);
uint16_t pikaMemNow(void);
uint16_t pikaMemMax(void);
void pikaMemMaxReset(void);
#endif 
