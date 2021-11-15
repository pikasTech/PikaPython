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
    __platformDisableIrqHandle();
    void* mem = __platformMalloc(size);
    __platformEnableIrqHandle();
    if (NULL == mem) {
        __platformPrintf(
            "[error]: No heap space! Please reset the device.\r\n");
        while (1) {
        }
    }
    return mem;
}

void pikaFree(void* mem, uint32_t size) {
    __platformDisableIrqHandle();
    __platformFree(mem);
    __platformEnableIrqHandle();
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

PikaMemPool pool_init(uint32_t size) {
    PikaMemPool pool;
    pool.mem = __platformMalloc(size);
    pool.bitmap = bitmap_init(size);
    return pool;
}

uint8_t* pool_malloc(PikaMemPool* pool, uint32_t size) {
    return NULL;
}

void pool_free(PikaMemPool* pool, uint8_t* mem, uint32_t size) {
    return;
}

uint8_t* bitmap_init(uint32_t size) {
    uint8_t* mem_bit_map =
        (uint8_t*)__platformMalloc(((size - 1) / 8 + 1) * sizeof(char));
    if (mem_bit_map == NULL)
        NULL;
    uint32_t size_mem_bit_map = (size - 1) / 8 + 1;
    memset(mem_bit_map, 0x0, size_mem_bit_map);
    return mem_bit_map;
}

void bitmap_set(uint8_t* bitmap, uint32_t index, uint8_t bit) {
    uint32_t index_byte = index / 8;
    uint8_t index_bit = index % 8;
    uint8_t x = (0x1 << index_bit);
    /* set 1 */
    if (bit) {
        bitmap[index_byte] |= x;
        return;
    }
    /* set 0 */
    bitmap[index_byte] &= ~x;
    return;
}

uint8_t bitmap_get(uint8_t* bitmap, uint32_t index) {
    uint32_t index_byte = (index) / 8;
    uint8_t index_bit = (index) % 8;
    uint8_t x = (0x1 << index_bit);
    uint8_t bit;
    bit = bitmap[index_byte] & x;
    return bit > 0 ? 1 : 0;
}

void bitmap_deinit(uint8_t* bitmap) {
    __platformFree(bitmap);
}