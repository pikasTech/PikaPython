#include "test_common.h"


#if PIKA_SYNTAX_SLICE_ENABLE
TEST(stddata, test1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/dict.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "{'len': 3, 'list': [1, 2, 3]}\r\n");
    EXPECT_STREQ(log_buff[1], "dict_keys([c, b, a])\r\n");
    EXPECT_STREQ(log_buff[2], "a\r\n");
    EXPECT_STREQ(log_buff[3], "b\r\n");
    EXPECT_STREQ(log_buff[4], "c\r\n");
    EXPECT_STREQ(log_buff[5], "1\r\n");
    EXPECT_STREQ(log_buff[6], "2\r\n");
    EXPECT_STREQ(log_buff[7], "test\r\n");
    EXPECT_STREQ(log_buff[8], "{'c': test, 'b': 2, 'a': 1}\r\n");
    EXPECT_STREQ(log_buff[9], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_IMPORT_EX_ENABLE
/* test b2a_hex */
TEST(stddata, test2) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "from PikaStdData import Utils\n"
            "ut = Utils()\n"
            "res = str(ut.b2a_hex(b'德卡科技'))\n");
    /* collect */
    char* res = obj_getStr(pikaMain, "res");
    /* assert */
    EXPECT_STREQ(res, "E5BEB7E58DA1E7A791E68A80");
    /* deinit */
    obj_deinit(pikaMain);
}
#endif
