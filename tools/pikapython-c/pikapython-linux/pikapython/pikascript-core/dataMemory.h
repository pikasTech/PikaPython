/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifdef __cplusplus
extern "C" {
#endif
#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "PikaPlatform.h"
#include "PikaVersion.h"

/*! \NOTE: Make sure #include "__pika_ooc.h" is close to the class definition
 */
#if defined(__DATA_MEMORY_CLASS_IMPLEMENT__)
#define __PLOOC_CLASS_IMPLEMENT__
#endif
#include "__pika_ooc.h"

typedef struct {
    uint32_t heapUsed;
    uint32_t heapUsedMax;
#if PIKA_ARG_CACHE_ENABLE
    uint8_t* cache_pool[PIKA_ARG_CACHE_POOL_SIZE];
    uint32_t cache_pool_top;
#endif
    uint32_t alloc_times;
    uint32_t alloc_times_cache;
} PikaMemInfo;

typedef uint8_t* BitMap;

/* clang-format off */
typedef struct Pool Pool;
struct Pool{
    private_member(
        BitMap bitmap;
        uint8_t* mem;
        uint8_t aline;
        uint32_t size;
        uint32_t first_free_block;
        uint32_t purl_free_block_start;
        pika_bool inited;
    )
};
/* clang-format on */

#define align_by(size, align) \
    (((size) == 0) ? 0 : (((size)-1) / (align) + 1) * (align))

void pikaFree(void* mem, uint32_t size);
void* pikaMalloc(uint32_t size);
uint32_t pikaMemNow(void);
uint32_t pikaMemMax(void);
void pikaMemMaxReset(void);

BitMap bitmap_init(uint32_t size);
void bitmap_set(BitMap bitmap, uint32_t index, uint8_t bit);
uint8_t bitmap_get(BitMap bitmap, uint32_t index);
uint8_t bitmap_getByte(BitMap bitmap, uint32_t index);
void bitmap_deinit(BitMap bitmap);

void mem_pool_deinit(void);
void mem_pool_init(void);

#define mem_align(_size) ((((_size) + 4 - 1) & ~(4 - 1)))

#undef __DATA_MEMORY_CLASS_IMPLEMENT__
#endif
#ifdef __cplusplus
}
#endif
