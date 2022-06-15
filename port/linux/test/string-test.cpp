#include "test_common.h"

TEST(string, cformat) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "s = cformat('test:%d:%f', 33, 1.5)\n");
    /* collect */
    char* s = obj_getStr(pikaMain, "s");
    /* assert */
    EXPECT_STREQ("test:33:1.500000", s);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTEX_ITEM_FORMAT_ENABLE
TEST(string, cformat1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "s = 'res:%d' % 23\n");
    /* collect */
    char* s = obj_getStr(pikaMain, "s");
    /* assert */
    EXPECT_STREQ("res:23", s);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTEX_ITEM_FORMAT_ENABLE
TEST(string, format1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "print('tes:%d,%f'%(123,1.5))\n");
    /* collect */
    /* assert */
    EXPECT_STREQ("tes:123,1.500000\r\n", log_buff[0]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTEX_ITEM_FORMAT_ENABLE
TEST(string, format2) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "print('tes:%d,%f'%(123,1.5), 23)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ("tes:123,1.500000 23\r\n", log_buff[0]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTEX_ITEM_FORMAT_ENABLE
TEST(string, print_file) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runFile(pikaMain, "../../examples/BuiltIn/print.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "format: test,0123,15.50\r\n");
    EXPECT_STREQ(log_buff[1], "my name is old wang my age is 43\r\n");
    EXPECT_STREQ(log_buff[2], "test\r\n");
    EXPECT_STREQ(log_buff[3], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);

    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTEX_ITEM_FORMAT_ENABLE
TEST(string, format_parse1) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "print('tes:%0.2f'% mem.getMax())";
    printf("%s\n", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 STR tes:%0.2f\n"
                 "2 RUN mem.getMax\n"
                 "1 RUN cformat\n"
                 "0 RUN print\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif
