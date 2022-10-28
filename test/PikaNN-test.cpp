#include "test_common.h"
TEST_START

#if (PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MAXIMAL) && (!PIKA_POOL_ENABLE)

TEST(PikaNN, test1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/PikaNN/PikaNN_test1.py");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);

    EXPECT_EQ(pikaMemNow(), 0);
}

extern "C" {
void _lm_test(void);
}
TEST(PikaNN, test_) {
    _lm_test();
}

#endif

TEST_END