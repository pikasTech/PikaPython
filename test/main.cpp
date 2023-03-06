#include "test_common.h"

extern "C" {
#include "dataMemory.h"
#include "pika_config_gtest.h"
}

extern "C" {
void test_purec(void);
}

int main(int argc, char** argv) {
    int res = 0;
#if USE_GOOGLE_TEST
    ::testing::InitGoogleTest(&argc, argv);
    res = RUN_ALL_TESTS();
#else
    test_purec();
#endif
    mem_pool_deinit();
#if PIKA_ARG_CACHE_ENABLE
    extern PikaMemInfo g_PikaMemInfo;
#if PIKA_GC_MARK_SWEEP_ENABLE
    extern PikaObjState g_PikaObjState;
    printf("[   GC]: object num max: %d, last GC: %d\r\n",
           g_PikaObjState.objCntMax, g_PikaObjState.objCntLastGC);
#endif
    printf("[ Info]: alloc times: %d, cached times: %d (%0.2f%%)\r\n",
           g_PikaMemInfo.alloc_times, g_PikaMemInfo.alloc_times_cache,
           ((float)g_PikaMemInfo.alloc_times_cache /
            (float)g_PikaMemInfo.alloc_times) *
               100.0);
#endif
    return res;
}
