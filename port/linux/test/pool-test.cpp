#include "gtest/gtest.h"
extern "C" {
#include "dataArg.h"
#include "dataMemory.h"
#include "dataString.h"
}

TEST(pool, init) {
    Pool p = pool_init(125, 4);
    EXPECT_EQ(p.aline, 4);
    EXPECT_EQ(p.size, 128);
    pool_printBlocks(&p, 0, 128);
    pool_deinit(&p);
    EXPECT_EQ((uint64_t)p.mem, NULL);
}