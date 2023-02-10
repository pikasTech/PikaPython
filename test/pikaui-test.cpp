#include "test_common.h"
TEST_START
#if !PIKA_NANO_ENABLE

TEST(pikaui, page) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runFile(pikaMain, "test/python/PikaUI/test_page.py");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}


#endif
TEST_END