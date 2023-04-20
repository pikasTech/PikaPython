#include "test_common.h"
TEST_START
#if !PIKA_NANO_ENABLE && 1

TEST(thread, test1) {
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/_thread/test1.py");
    /* clang-format on */
    /* deinit */
    obj_deinit(pikaMain);

    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(thread, self) {
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/_thread/thread_self.py");
    /* clang-format on */
    /* deinit */
    obj_deinit(pikaMain);

    EXPECT_STREQ(log_buff[0], "self._val: 2\r\n");
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST_SINGLE_FILE(thread, test2, "test/python/_thread/test2.py")

TEST_SINGLE_FILE(eventloop, test1, "test/python/eventloop/test1.py")
TEST_SINGLE_FILE(eventloop, test2, "test/python/eventloop/test2.py")
TEST_SINGLE_FILE(eventloop, test3, "test/python/eventloop/test3.py")
TEST_SINGLE_FILE(eventloop, delay1, "test/python/eventloop/delay1.py")

#endif
TEST_END
