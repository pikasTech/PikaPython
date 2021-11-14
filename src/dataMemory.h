/*
  Author: lyon
  Tencent QQ: 645275593
*/

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "PikaPlatform.h"

typedef struct {
    uint32_t heapUsed;
    uint32_t heapUsedMax;
} PikaMemInfo;

void pikaFree(void* mem, uint32_t size);
void* pikaMalloc(uint32_t size);
uint16_t pikaMemNow(void);
uint16_t pikaMemMax(void);
void pikaMemMaxReset(void);

uint8_t* bitmap_init(uint32_t size);
void bitmap_set(uint8_t* bitmap, uint32_t index, uint8_t bit);
uint8_t bitmap_get(uint8_t* bitmap, uint32_t index);
void bitmap_deinit(uint8_t* bitmap);

#endif
