#include "test_common.h"

extern "C" {
#include "dataMemory.h"
#include "pika_config_gtest.h"
}

int main(int argc, char** argv) {
    int res = 0;
#if USE_GOOGLE_TEST
    ::testing::InitGoogleTest(&argc, argv);
    res = RUN_ALL_TESTS();
#endif
    mem_pool_deinit();
#if PIKA_ARG_CACHE_ENABLE
    extern PikaMemInfo pikaMemInfo;
    printf("[ Info]: alloc times: %d, cached times: %d (%0.2f%%)\r\n",
           pikaMemInfo.alloc_times, pikaMemInfo.alloc_times_cache,
           ((float)pikaMemInfo.alloc_times_cache /
            (float)pikaMemInfo.alloc_times) *
               100.0);
#endif
    return res;
}
