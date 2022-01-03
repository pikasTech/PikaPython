#include <stdio.h>
#include <stdlib.h>
#include "dataMemory.h"
#include "mem_pool_config.h"

#define use_const_pool 0
#define use_dynamic_pool 1

#define pika_aline 8
#define pika_pool_size 0x4000

char log_buff[LOG_BUFF_MAX][LOG_SIZE] = {0};
uint32_t log_index = 0;

/* save printf content to log_buff */
void __platform_printf(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    for (int i = LOG_BUFF_MAX - 2; i >= 0; i--) {
        memcpy(log_buff[i + 1], log_buff[i], LOG_SIZE);
    }
    memcpy(log_buff[0], fmt, LOG_SIZE);
}

/* quick_malloc is always open */
uint8_t __is_quick_malloc(void) {
    // return 1;
    return 0;
}
#if use_const_pool
uint8_t pika_bitmap[pika_pool_size / pika_aline / 8] = {0};
uint8_t pika_pool_mem[pika_pool_size] = {0};
Pool pikaPool = {.aline = pika_aline,
                 .bitmap = pika_bitmap,
                 .mem = pika_pool_mem,
                 .size = pika_pool_size};
void* __impl_pikaMalloc(size_t size) {
    return pool_malloc(&pikaPool, size);
}
void __impl_pikaFree(void* ptrm, size_t size) {
    pool_free(&pikaPool, ptrm, size);
}
#endif

#if use_dynamic_pool
Pool pikaPool;
void* __impl_pikaMalloc(size_t size) {
    return pool_malloc(&pikaPool, size);
}
void __impl_pikaFree(void* ptrm, size_t size) {
    pool_free(&pikaPool, ptrm, size);
}
void __platform_pool_init(void) {
    pikaPool = pool_init(pika_pool_size, pika_aline);
}
#endif
