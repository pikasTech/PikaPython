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

typedef uint8_t* BitMap;

typedef struct {
    BitMap bitmap;
    uint8_t* mem;
    uint8_t aline;
    uint32_t size;
} Pool;

void pikaFree(void* mem, uint32_t size);
void* pikaMalloc(uint32_t size);
uint16_t pikaMemNow(void);
uint16_t pikaMemMax(void);
void pikaMemMaxReset(void);

BitMap bitmap_init(uint32_t size);
void bitmap_set(BitMap bitmap, uint32_t index, uint8_t bit);
uint8_t bitmap_get(BitMap bitmap, uint32_t index);
void bitmap_deinit(BitMap bitmap);

Pool pool_init(uint32_t size, uint8_t aline);
void* pool_malloc(Pool* pool, uint32_t size);
void pool_free(Pool* pool, void* mem, uint32_t size);
void pool_deinit(Pool* pool);
void pool_printBlocks(Pool* pool, uint32_t block_min, uint32_t block_max);
#endif
