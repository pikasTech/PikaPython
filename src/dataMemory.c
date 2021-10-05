/*
  Author: lyon
  Tencent QQ: 645275593
*/

#include "dataMemory.h"
#include <stdint.h>
#include <stdlib.h>

PikaMemInfo pikaMemInfo = {0};

PIKA_WEAK void __pikaDisableIrqHandle(){
    /* disable irq to support thread */
}

PIKA_WEAK void __pikaEnableIrqHandle(){
    /* disable irq to support thread */    
}

PIKA_WEAK void *pika_malloc(size_t size){
    return malloc(size);
}

PIKA_WEAK void pika_free(void *ptr){
    return free(ptr);
}

void* pikaMalloc(uint32_t size) {
    pikaMemInfo.heapUsed += size;
    if (pikaMemInfo.heapUsedMax < pikaMemInfo.heapUsed) {
        pikaMemInfo.heapUsedMax = pikaMemInfo.heapUsed;
    }
    __pikaDisableIrqHandle();
    void* mem = pika_malloc(size);
    __pikaEnableIrqHandle();
    if (NULL == mem) {
        printf("[error]: No heap space! Please reset the device.\r\n");
        while (1) {
        }
    }
    return mem;
}

void pikaFree(void* mem, uint32_t size) {
    __pikaDisableIrqHandle();
    pika_free(mem);
    __pikaEnableIrqHandle();    
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
