#include "test_common.h"
TEST_START

TEST(cmodule, print_) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "print('test', 'test2')\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "test test2\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST_END