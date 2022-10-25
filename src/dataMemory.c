/*
 * This file is part of the PikaScript project.
 * http://github.com/pikastech/pikascript
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon 李昂 liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define __DATA_MEMORY_CLASS_IMPLEMENT__
#include "dataMemory.h"
#include "PikaPlatform.h"

volatile PikaMemInfo pikaMemInfo = {0};

void* pikaMalloc(uint32_t size) {
    /* pika memory lock */
    if (0 != __is_locked_pikaMemory()) {
        __platform_wait();
    }

//! if you unsure about the __impl_pikaMalloc, uncomment this to force alignment
#if PIKA_ARG_ALIGN_ENABLE
    /* force alignment to avoid unaligned access */
    size = mem_align(size);
#endif

    pikaMemInfo.heapUsed += size;
    if (pikaMemInfo.heapUsedMax < pikaMemInfo.heapUsed) {
        pikaMemInfo.heapUsedMax = pikaMemInfo.heapUsed;
    }
    __platform_disable_irq_handle();
    void* mem = __user_malloc(size);
    __platform_enable_irq_handle();
    if (NULL == mem) {
        __platform_printf("Error: No heap space! Please reset the device.\r\n");
        while (1) {
        }
    }
    return mem;
}

void pikaFree(void* mem, uint32_t size) {
    if (0 != __is_locked_pikaMemory()) {
        __platform_wait();
    }

//! if you unsure about the __impl_pikaMalloc, uncomment this to force alignment
#if PIKA_ARG_ALIGN_ENABLE
    /* force alignment to avoid unaligned access */
    size = mem_align(size);
#endif

    __platform_disable_irq_handle();
    __user_free(mem, size);
    __platform_enable_irq_handle();
    pikaMemInfo.heapUsed -= size;
}

uint32_t pikaMemNow(void) {
    return pikaMemInfo.heapUsed;
    // return 0;
}

uint32_t pikaMemMax(void) {
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
    pool.mem = __platform_malloc(pool_aline(&pool, pool.size));
    pool.first_free_block = 0;
    pool.purl_free_block_start = 0;
    pool.inited = PIKA_TRUE;
    return pool;
}

void pool_deinit(Pool* pool) {
    __platform_free(pool->mem);
    pool->mem = NULL;
    bitmap_deinit(pool->bitmap);
}

void* pool_getBytes_byBlockIndex(Pool* pool, uint32_t block_index) {
    return pool->mem + block_index * pool->aline;
}

uint32_t pool_getBlockIndex_byMem(Pool* pool, void* mem) {
    uint32_t mem_size = (uintptr_t)mem - (uintptr_t)pool->mem;
    return pool_getBlockIndex_byMemSize(pool, mem_size);
}

void pool_printBlocks(Pool* pool, uint32_t size_min, uint32_t size_max) {
    uint32_t block_index_min = pool_getBlockIndex_byMemSize(pool, size_min);
    uint32_t block_index_max = pool_getBlockIndex_byMemSize(pool, size_max);
    __platform_printf("[bitmap]\r\n");
    uint8_t is_end = 0;
    for (uint32_t i = block_index_min; i < block_index_max; i += 16) {
        if (is_end) {
            break;
        }
        __platform_printf("0x%x\t: 0x%d", i * pool->aline,
                          (i + 15) * pool->aline);
        for (uint32_t j = i; j < i + 16; j += 4) {
            if (is_end) {
                break;
            }
            for (uint32_t k = j; k < j + 4; k++) {
                if (k >= block_index_max) {
                    is_end = 1;
                    break;
                }
                __platform_printf("%d", bitmap_get(pool->bitmap, k));
            }
            __platform_printf(" ");
        }
        __platform_printf("\r\n");
    }
}

void* pool_malloc(Pool* pool, uint32_t size) {
    uint32_t block_index_max = pool_getBlockIndex_byMemSize(pool, pool->size);
    uint32_t block_num_need = pool_getBlockIndex_byMemSize(pool, size);
    uint32_t block_num_found = 0;
    uint8_t found_first_free = 0;
    uint32_t block_index;
    /* high speed malloc */
    block_index = pool->purl_free_block_start + block_num_need - 1;
    if (block_index < block_index_max) {
        goto found;
    }
    /* low speed malloc */
    for (block_index = pool->first_free_block;
         block_index < pool->purl_free_block_start; block_index++) {
        /* 8 bit is not free */
        uint8_t bitmap_byte = bitmap_getByte(pool->bitmap, block_index);
        if (0xFF == bitmap_byte) {
            block_index = 8 * (block_index / 8) + 7;
            block_num_found = 0;
            continue;
        }
        /* found a free block */
        if (0 == bitmap_get(pool->bitmap, block_index)) {
            /* save the first free */
            if (!found_first_free) {
                pool->first_free_block = block_index;
                found_first_free = 1;
            }
            block_num_found++;
        } else {
            /* a used block appeared, find again */
            block_num_found = 0;
        }
        /* found all request blocks */
        if (block_num_found >= block_num_need) {
            goto found;
        }
    }
    /* malloc for purl free blocks */
    block_index = pool->purl_free_block_start + block_num_need - 1;
    if (block_index < block_index_max) {
        goto found;
    }

    /* no found */
    return NULL;
found:
    /* set 1 for found blocks */
    for (uint32_t i = 0; i < block_num_need; i++) {
        bitmap_set(pool->bitmap, block_index - i, 1);
    }
    /* save last used block */
    if (block_index >= pool->purl_free_block_start) {
        pool->purl_free_block_start = block_index + 1;
    }
    /* return mem by block index */
    return pool_getBytes_byBlockIndex(pool, block_index - block_num_need + 1);
}

void pool_free(Pool* pool, void* mem, uint32_t size) {
    uint32_t block_num = pool_getBlockIndex_byMemSize(pool, size);
    uint32_t block_index = pool_getBlockIndex_byMem(pool, mem);
    for (uint32_t i = 0; i < block_num; i++) {
        bitmap_set(pool->bitmap, block_index + i, 0);
    }
    /* save min free block index to add speed */
    if (block_index < pool->first_free_block) {
        pool->first_free_block = block_index;
    }
    /* save last free block index to add speed */
    uint32_t block_end = block_index + block_num - 1;
    if (block_end == pool->purl_free_block_start - 1) {
        uint32_t first_pure_free_block = block_index;
        /* back to first used block */
        if (0 != first_pure_free_block) {
            while (0 == bitmap_get(pool->bitmap, first_pure_free_block - 1)) {
                first_pure_free_block--;
                if (0 == first_pure_free_block) {
                    break;
                }
            }
        }
        pool->purl_free_block_start = first_pure_free_block;
    }
    return;
}

BitMap bitmap_init(uint32_t size) {
    BitMap mem_bit_map =
        (BitMap)__platform_malloc(((size - 1) / 8 + 1) * sizeof(char));
    if (mem_bit_map == NULL) {
        return NULL;
    }
    uint32_t size_mem_bit_map = (size - 1) / 8 + 1;
    __platform_memset(mem_bit_map, 0x0, size_mem_bit_map);
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

uint8_t bitmap_getByte(BitMap bitmap, uint32_t index) {
    uint32_t index_byte = (index) / 8;
    uint8_t byte;
    byte = bitmap[index_byte];
    return byte;
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
    __platform_free(bitmap);
}

#if PIKA_POOL_ENABLE
Pool pikaPool = {0};
void* __user_malloc(size_t size) {
    return pool_malloc(&pikaPool, size);
}
void __user_free(void* ptrm, size_t size) {
    pool_free(&pikaPool, ptrm, size);
}
#endif

void mem_pool_init(void) {
#if PIKA_POOL_ENABLE
    if (!pikaPool.inited) {
        pikaPool = pool_init(PIKA_POOL_SIZE, PIKA_POOL_ALIGN);
    }
#endif
}

void _mem_cache_deinit(void) {
#if PIKA_ARG_CACHE_ENABLE
    while (pikaMemInfo.cache_pool_top) {
        __user_free(pikaMemInfo.cache_pool[pikaMemInfo.cache_pool_top - 1], 0);
        pikaMemInfo.cache_pool_top--;
    }
#endif
}

void mem_pool_deinit(void) {
    _mem_cache_deinit();
#if PIKA_POOL_ENABLE
    pool_deinit(&pikaPool);
#endif
}
