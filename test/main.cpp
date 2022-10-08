#include "gtest/gtest.h"
#include "test_common.h"

extern "C" {
#include "dataMemory.h"
#include "pika_config_gtest.h"
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int res = RUN_ALL_TESTS();
    mem_pool_deinit();
    extern PikaMemInfo pikaMemInfo;
#if PIKA_ARG_CACHE_ENABLE
    printf("[ Info]: alloc times: %d, cached times: %d (%0.2f%%)\r\n",
           pikaMemInfo.alloc_times, pikaMemInfo.alloc_times_cache,
           ((float)pikaMemInfo.alloc_times_cache /
            (float)pikaMemInfo.alloc_times) *
               100.0);
#endif
    return res;
}
