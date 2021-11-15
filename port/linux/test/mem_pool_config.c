#include "dataMemory.h"

#define pika_aline 8
#define pika_pool_size 8192
uint8_t pika_bitmap[pika_pool_size / pika_aline / 8] = {0};
uint8_t pika_pool_mem[pika_pool_size] = {0};
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