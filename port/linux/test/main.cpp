#include "gtest/gtest.h"

extern "C" {
#include "dataMemory.h"
#include "pika_config_gtest.h"
}

int main(int argc, char** argv) {
    mem_pool_init();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
