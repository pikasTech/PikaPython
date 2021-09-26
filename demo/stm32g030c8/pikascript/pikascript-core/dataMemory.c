/*
  Author: lyon
  Tencent QQ: 645275593
*/

#include "dataMemory.h"
#include <stdint.h>
#include <stdlib.h>

PikaMemInfo pikaMemInfo = {0};

void* pikaMalloc(uint32_t size) {
    pikaMemInfo.heapUsed += size;
    if (pikaMemInfo.heapUsedMax < pikaMemInfo.heapUsed) {
        pikaMemInfo.heapUsedMax = pikaMemInfo.heapUsed;
    }
    void* mem = malloc(size);
    if (NULL == mem) {
        printf("[error]: No heap space! Please reset the device.\r\n");
        while (1) {
        }
    }
    return mem;
}

void pikaFree(void* mem, uint32_t size) {
    free(mem);
    pikaMemInfo.heapUsed -= size;
}

uint16_t pikaMemNow(void) {
    return pikaMemInfo.heapUsed;
}

uint16_t pikaMemMax(void) {
    return pikaMemInfo.heapUsedMax;
}

void pikaMemMaxReset(void) {
    pikaMemInfo.heapUsedMax = 0;
}
