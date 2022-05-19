#include "gtest/gtest.h"
#include "test_common.h"

extern "C" {
#include "dataMemory.h"
#include "pika_config_gtest.h"
}

int main(int argc, char** argv) {
    mem_pool_init();
    ::testing::InitGoogleTest(&argc, argv);
    int res = RUN_ALL_TESTS();
    mem_pool_deinit();
    return res;
}
