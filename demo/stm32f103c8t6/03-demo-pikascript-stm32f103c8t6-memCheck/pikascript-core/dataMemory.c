#include "dataMemory.h"
#include <stdint.h>
#include <stdlib.h>

PikaMemInfo pikaMemInfo = {0};

void *pikaMalloc(uint32_t size)
{
    pikaMemInfo.heapUsed += size;
    if (pikaMemInfo.heapUsedMax < pikaMemInfo.heapUsed)
    {
        pikaMemInfo.heapUsedMax = pikaMemInfo.heapUsed;
    }
    return malloc(size);
}

void pikaFree(void *mem, uint32_t size)
{
    free(mem);
    mem = NULL;
    pikaMemInfo.heapUsed -= size;
}

uint16_t pikaMemNow(void)
{
    return pikaMemInfo.heapUsed;
}

uint16_t pikaMemMax(void)
{
    return pikaMemInfo.heapUsedMax;
}

void pikaMemMaxReset(void)
{
    pikaMemInfo.heapUsedMax = 0;
}