#include "dataMemory.h"

#define use_const_pool 1
#define use_dynamic_pool 0

#define pika_aline 8
#define pika_pool_size 0x1B00

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
void __platformPoolInit(void) {
    pikaPool = pool_init(pika_pool_size, pika_aline);
}
#endif
