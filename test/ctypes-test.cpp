#include "test_common.h"

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(ctypes, test1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/ctypes.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[18], "16\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(ctypes, test2) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "len(b'test\\x0')\n"
            "len(str(b'test\\x0'))\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "4\r\n");
    EXPECT_STREQ(log_buff[1], "5\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}