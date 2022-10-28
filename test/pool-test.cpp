#include "test_common.h"
TEST_START

extern "C" {
Pool pool_init(uint32_t size, uint8_t aline);
void* pool_malloc(Pool* pool, uint32_t size);
void pool_free(Pool* pool, void* mem, uint32_t size);
void pool_deinit(Pool* pool);
void pool_printBlocks(Pool* pool, uint32_t block_min, uint32_t block_max);
}

TEST(pool, init) {
    Pool p = pool_init(125, 4);
    EXPECT_EQ(p.aline, 4);
    EXPECT_EQ(p.size, 128);
    pool_printBlocks(&p, 0, 128);
    pool_deinit(&p);
    EXPECT_EQ((uint64_t)p.mem, NULL);
}

TEST(pool, m1) {
    Pool p = pool_init(125, 4);
    EXPECT_EQ(p.aline, 4);
    EXPECT_EQ(p.size, 128);
    uint64_t mem1_offset = (uint64_t)pool_malloc(&p, 4) - (uint64_t)p.mem;
    pool_printBlocks(&p, 0, 128);
    EXPECT_EQ(1, bitmap_get(p.bitmap, 0));
    EXPECT_EQ(0, bitmap_get(p.bitmap, 1));
    EXPECT_EQ(0, mem1_offset);
    pool_deinit(&p);
}

TEST(pool, m2) {
    Pool p = pool_init(125, 4);
    EXPECT_EQ(p.aline, 4);
    EXPECT_EQ(p.size, 128);
    uint64_t mem1_offset = (uint64_t)pool_malloc(&p, 4) - (uint64_t)p.mem;
    uint64_t mem2_offset = (uint64_t)pool_malloc(&p, 4) - (uint64_t)p.mem;
    pool_printBlocks(&p, 0, 128);
    EXPECT_EQ(1, bitmap_get(p.bitmap, 0));
    EXPECT_EQ(1, bitmap_get(p.bitmap, 1));
    EXPECT_EQ(0, mem1_offset);
    EXPECT_EQ(4, mem2_offset);
    pool_deinit(&p);
}

TEST(pool, mf1) {
    Pool p = pool_init(125, 4);
    EXPECT_EQ(p.aline, 4);
    EXPECT_EQ(p.size, 128);
    void* mem1 = pool_malloc(&p, 4);
    uint64_t mem1_offset = (uint64_t)mem1 - (uint64_t)p.mem;
    EXPECT_EQ(1, bitmap_get(p.bitmap, 0));
    pool_printBlocks(&p, 0, 128);
    pool_free(&p, mem1, 4);
    pool_printBlocks(&p, 0, 128);
    EXPECT_EQ(0, bitmap_get(p.bitmap, 0));
    EXPECT_EQ(0, mem1_offset);
    pool_deinit(&p);
}

TEST(pool, mf2) {
    Pool p = pool_init(125, 16);
    EXPECT_EQ(p.aline, 16);
    EXPECT_EQ(p.size, 128);
    void* mem1 = pool_malloc(&p, 4);
    uint64_t mem1_offset = (uint64_t)mem1 - (uint64_t)p.mem;
    EXPECT_EQ(1, bitmap_get(p.bitmap, 0));
    pool_printBlocks(&p, 0, 128);
    pool_free(&p, mem1, 4);
    pool_printBlocks(&p, 0, 128);
    EXPECT_EQ(0, bitmap_get(p.bitmap, 0));
    EXPECT_EQ(0, mem1_offset);
    pool_deinit(&p);
}

TEST(pool, mfm1) {
    Pool p = pool_init(125, 4);
    EXPECT_EQ(p.size, 128);
    void* mem1 = pool_malloc(&p, 4);
    pool_malloc(&p, 16);
    pool_free(&p, mem1, 4);
    pool_malloc(&p, 16);
    pool_printBlocks(&p, 0, 128);

    pool_deinit(&p);
}
TEST_END