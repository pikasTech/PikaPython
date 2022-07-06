#include "test_common.h"

#if PIKA_SYNTEX_FORMAT_ENABLE
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
#endif

#if PIKA_SYNTEX_FORMAT_ENABLE
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

#if PIKA_SYNTEX_FORMAT_ENABLE
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

#if PIKA_SYNTEX_FORMAT_ENABLE
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

#if PIKA_SYNTEX_FORMAT_ENABLE
TEST(string, print_file) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/print.py");
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

#if PIKA_SYNTEX_FORMAT_ENABLE
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

TEST(string, split) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "s = PikaStdData.String('a,b,c,d')\n"
            "tokens = s.split(',')\n"
            "for item in tokens:\n"
            "    print(item)\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "d\r\n");
    EXPECT_STREQ(log_buff[1], "c\r\n");
    EXPECT_STREQ(log_buff[2], "b\r\n");
    EXPECT_STREQ(log_buff[3], "a\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(string, strip) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "s = PikaStdData.String('  a,b,c, d  ')\n"
            "res = s.strip()\n"
            "\n");
    /* collect */
    char* res = obj_getStr(pikaMain, "res");
    /* assert */
    EXPECT_STREQ(res, "a,b,c, d");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(string, replace) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "s = PikaStdData.String('  a,b,c, d  ')\n"
            "res = s.replace('a', 'A')\n"
            "\n");
    /* collect */
    char* res = obj_getStr(pikaMain, "res");
    /* assert */
    EXPECT_STREQ(res, "  A,b,c, d  ");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTEX_IMPORT_EX_ENABLE
TEST(string, replace_chain) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "from PikaStdData import String as S\n"
            "res = PikaStdData.String('  a,b,c, d  ').replace('a', 'A')\n"
            "res2 = S(S('[test]').replace('[','')).replace(']','')\n"
            "\n");
    /* collect */
    char* res = obj_getStr(pikaMain, "res");
    char* res2 = obj_getStr(pikaMain, "res2");
    /* assert */
    EXPECT_STREQ(res, "  A,b,c, d  ");
    EXPECT_STREQ(res2, "test");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(string, split_chain) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "PikaStdData.String('  a,b,c, d  ').split(',')\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "[  a, b, c,  d  ]\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(string, str_chain) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "PikaStdData.String('test').str\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "test\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTEX_FORMAT_ENABLE
TEST(string, str_issue1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/str_issue1.py");
    /* collect */
    char* ret_str = obj_getStr(pikaMain, "ret_str");
    /* assert */
    EXPECT_STREQ(ret_str, "None");
    /* deinit */
    obj_deinit(pikaMain);

    EXPECT_EQ(pikaMemNow(), 0);
}
#endif
