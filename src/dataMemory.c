/*
  Author: lyon
  Tencent QQ: 645275593
*/

#include "dataMemory.h"
#include <stdint.h>
#include <stdlib.h>

#define pika_aline 8
#define pika_pool_size 8192
uint8_t pika_bitmap[pika_pool_size / pika_aline / 8] = {0};
uint8_t pika_pool_mem[pika_pool_size] = {0};
PikaMemInfo pikaMemInfo = {0};
Pool pikaPool = {.aline = pika_aline,
                 .bitmap = pika_bitmap,
                 .mem = pika_pool_mem,
                 .size = pika_pool_size};

void* __impl_pikaMalloc(size_t size) {
    void* mem = pool_malloc(&pikaPool, size);
    return mem;
}
void __impl_pikaFree(void* ptrm, size_t size) {
    pool_free(&pikaPool, ptrm, size);
}

void* pikaMalloc(uint32_t size) {
    pikaMemInfo.heapUsed += size;
    if (pikaMemInfo.heapUsedMax < pikaMemInfo.heapUsed) {
        pikaMemInfo.heapUsedMax = pikaMemInfo.heapUsed;
    }
    __platformDisableIrqHandle();
    void* mem = __impl_pikaMalloc(size);
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
    __impl_pikaFree(mem, size);
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

uint32_t pool_getBlockIndex_byMemSize(Pool* pool, uint32_t size) {
    if (0 == size) {
        return 0;
    }
    return (size - 1) / pool->aline + 1;
}

uint32_t pool_aline(Pool* pool, uint32_t size) {
    return pool_getBlockIndex_byMemSize(pool, size) * pool->aline;
}

Pool pool_init(uint32_t size, uint8_t aline) {
    Pool pool;
    pool.aline = aline;
    uint32_t block_size = pool_getBlockIndex_byMemSize(&pool, size);
    pool.size = pool_aline(&pool, size);
    pool.bitmap = bitmap_init(block_size);
    pool.mem = __platformMalloc(pool_aline(&pool, pool.size));
    return pool;
}

void pool_deinit(Pool* pool) {
    __platformFree(pool->mem);
    pool->mem = NULL;
    bitmap_deinit(pool->bitmap);
}

void* pool_getMem_byBlockIndex(Pool* pool, uint32_t block_index) {
    return pool->mem + block_index * pool->aline;
}

uint32_t pool_getBlockIndex_byMem(Pool* pool, void* mem) {
    uint32_t mem_size = (uint64_t)mem - (uint64_t)pool->mem;
    return pool_getBlockIndex_byMemSize(pool, mem_size);
}

void pool_printBlocks(Pool* pool, uint32_t size_min, uint32_t size_max) {
    uint32_t block_index_min = pool_getBlockIndex_byMemSize(pool, size_min);
    uint32_t block_index_max = pool_getBlockIndex_byMemSize(pool, size_max);
    __platformPrintf("[bitmap]\r\n");
    uint8_t is_end = 0;
    for (uint32_t i = block_index_min; i < block_index_max; i += 16) {
        if (is_end) {
            break;
        }
        __platformPrintf("0x%x\t: ", i * pool->aline, (i + 15) * pool->aline);
        for (uint32_t j = i; j < i + 16; j += 4) {
            if (is_end) {
                break;
            }
            for (uint32_t k = j; k < j + 4; k++) {
                if (k >= block_index_max) {
                    is_end = 1;
                    break;
                }
                __platformPrintf("%d", bitmap_get(pool->bitmap, k));
            }
            __platformPrintf(" ");
        }
        __platformPrintf("\r\n");
    }
}

void* pool_malloc(Pool* pool, uint32_t size) {
    void* mem = NULL;
    uint32_t block_index_max = pool_getBlockIndex_byMemSize(pool, pool->size);
    uint32_t block_num_need = pool_getBlockIndex_byMemSize(pool, size);
    uint32_t block_num_found = 0;
    for (uint32_t block_index = 0; block_index < block_index_max;
         block_index++) {
        /* found a free block */
        if (0 == bitmap_get(pool->bitmap, block_index)) {
            block_num_found++;
        } else {
            /* a used block appeared, find again */
            block_num_found = 0;
        }
        /* found all request blocks */
        if (block_num_found >= block_num_need) {
            /* set 1 for found blocks */
            for (uint32_t i = 0; i < block_num_need; i++) {
                bitmap_set(pool->bitmap, block_index - i, 1);
            }
            /* return mem by block index */
            return pool_getMem_byBlockIndex(pool,
                                            block_index - block_num_need + 1);
        }
    }
    return mem;
}

void pool_free(Pool* pool, void* mem, uint32_t size) {
    uint32_t block_num = pool_getBlockIndex_byMemSize(pool, size);
    uint32_t block_index = pool_getBlockIndex_byMem(pool, mem);
    for (uint32_t i = 0; i < block_num; i++) {
        bitmap_set(pool->bitmap, block_index + i, 0);
    }
    return;
}

BitMap bitmap_init(uint32_t size) {
    BitMap mem_bit_map =
        (BitMap)__platformMalloc(((size - 1) / 8 + 1) * sizeof(char));
    if (mem_bit_map == NULL)
        NULL;
    uint32_t size_mem_bit_map = (size - 1) / 8 + 1;
    memset(mem_bit_map, 0x0, size_mem_bit_map);
    return mem_bit_map;
}

void bitmap_set(BitMap bitmap, uint32_t index, uint8_t bit) {
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

uint8_t bitmap_get(BitMap bitmap, uint32_t index) {
    uint32_t index_byte = (index) / 8;
    uint8_t index_bit = (index) % 8;
    uint8_t x = (0x1 << index_bit);
    uint8_t bit;
    bit = bitmap[index_byte] & x;
    return bit > 0 ? 1 : 0;
}

void bitmap_deinit(BitMap bitmap) {
    __platformFree(bitmap);
}