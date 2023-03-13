#include "test_common.h"
TEST_START
#if !PIKA_NANO_ENABLE

TEST(os, test1) {
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/os/os_test1.py");
    /* assert */
    EXPECT_STREQ(log_buff[0], "PASS\r\n");
    EXPECT_STREQ(log_buff[1], "Hello World!\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif
TEST_END
