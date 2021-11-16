#include "gtest/gtest.h"

extern "C" {
#include "dataMemory.h"
}

#define use_dynamic_pool 1
/* use mem pool */
#if use_dynamic_pool
Pool pikaPool;
void* __impl_pikaMalloc(size_t size) {
    void* mem = pool_malloc(&pikaPool, size);
    return mem;
}
void __impl_pikaFree(void* ptrm, size_t size) {
    pool_free(&pikaPool, ptrm, size);
}
#endif

int main(int argc, char** argv) {
#if use_dynamic_pool
    pikaPool = pool_init(0x1B00, 4);
#endif
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
