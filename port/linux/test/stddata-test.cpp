#include "test_common.h"


#if PIKA_SYNTEX_ITEM_SLICE_ENABLE
TEST(stddata, test1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/dict.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "dict_keys([c, b, a])\r\n");
    EXPECT_STREQ(log_buff[1], "a\r\n");
    EXPECT_STREQ(log_buff[2], "b\r\n");
    EXPECT_STREQ(log_buff[3], "c\r\n");
    EXPECT_STREQ(log_buff[4], "1\r\n");
    EXPECT_STREQ(log_buff[5], "2\r\n");
    EXPECT_STREQ(log_buff[6], "test\r\n");
    EXPECT_STREQ(log_buff[7], "{'c': test, 'b': 2, 'a': 1}\r\n");
    EXPECT_STREQ(log_buff[8], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif
