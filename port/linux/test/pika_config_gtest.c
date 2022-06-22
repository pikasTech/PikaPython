#include "pika_config_gtest.h"
#include <stdio.h>
#include <stdlib.h>
#include "dataMemory.h"

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
    va_start(args, fmt);
    vsnprintf(log_buff[0], LOG_SIZE - 1, fmt, args);
    va_end(args);
}

/* quick_malloc is always open */
uint8_t __is_quick_malloc(void) {
    // return 1;
    return 0;
}
#if use_const_pool
uint8_t pika_bitmap[PIKA_POOL_SIZE / PIKA_POOL_ALIGN / 8] = {0};
uint8_t pika_pool_mem[PIKA_POOL_SIZE] = {0};
Pool pikaPool = {.aline = PIKA_POOL_ALIGN,
                 .bitmap = pika_bitmap,
                 .mem = pika_pool_mem,
                 .size = PIKA_POOL_SIZE};
void* __user_malloc(size_t size) {
    return pool_malloc(&pikaPool, size);
}
void __user_free(void* ptrm, size_t size) {
    pool_free(&pikaPool, ptrm, size);
}
#endif
