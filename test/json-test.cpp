#include "test_common.h"
TEST_START

#if !PIKA_NANO_ENABLE
TEST(json, loads) {
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/json/json_loads.py");
    /* collect */
    /* assert */
    /* clang-format off */
    /*
    {'c': 3.000000, 'b': 2.000000, 'a': 1.000000}
    {'d': {'f': 5.000000, 'e': 4.000000}, 'c': 3.000000, 'b': 2.000000, 'a': 1.000000}
    {'g': [6.000000, 7.000000, 8.000000], 'd': {'f': 5.000000, 'e': 4.000000}, 'c': 3.000000, 'b': 2.000000, 'a': 1.000000}
    {'h': None, 'g': [6.000000, 7.000000, 8.000000], 'd': {'f': 5.000000, 'e': 4.000000}, 'c': 3.000000, 'b': 2.000000, 'a': 1.000000}
    {'i': 0, 'h': None, 'g': [6.000000, 7.000000, 8.000000], 'd': {'f': 5.000000, 'e': 4.000000}, 'c': 3.000000, 'b': 2.000000, 'a': 1.000000}
    {'j': 1, 'i': 0, 'h': None, 'g': [6.000000, 7.000000, 8.000000], 'd': {'f': 5.000000, 'e': 4.000000}, 'c': 3.000000, 'b': 2.000000, 'a': 1.000000}
    {'k': 'string', 'j': 1, 'i': 0, 'h': None, 'g': [6.000000, 7.000000, 8.000000], 'd': {'f': 5.000000, 'e': 4.000000}, 'c': 3.000000, 'b': 2.000000, 'a': 1.000000}
    {'l': 1.234000, 'k': 'string', 'j': 1, 'i': 0, 'h': None, 'g': [6.000000, 7.000000, 8.000000], 'd': {'f': 5.000000, 'e': 4.000000}, 'c': 3.000000, 'b': 2.000000, 'a': 1.000000} 
    */
    /* clang-format on */
    EXPECT_STREQ(log_buff[8], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[7],
                 "{'c': 3.000000, 'b': 2.000000, 'a': 1.000000}\r\n");
    EXPECT_STREQ(log_buff[6],
                 "{'d': {'f': 5.000000, 'e': 4.000000}, 'c': 3.000000, 'b': "
                 "2.000000, 'a': 1.000000}\r\n");
    EXPECT_STREQ(
        log_buff[5],
        "{'g': [6.000000, 7.000000, 8.000000], 'd': {'f': 5.000000, 'e': "
        "4.000000}, 'c': 3.000000, 'b': 2.000000, 'a': 1.000000}\r\n");
    EXPECT_STREQ(
        log_buff[4],
        "{'h': None, 'g': [6.000000, 7.000000, 8.000000], 'd': {'f': 5.000000, "
        "'e': 4.000000}, 'c': 3.000000, 'b': 2.000000, 'a': 1.000000}\r\n");
    EXPECT_STREQ(log_buff[3],
                 "{'i': False, 'h': None, 'g': [6.000000, 7.000000, 8.000000], "
                 "'d': {'f': 5.000000, 'e': 4.000000}, 'c': 3.000000, 'b': "
                 "2.000000, 'a': 1.000000}\r\n");
    EXPECT_STREQ(log_buff[2],
                 "{'j': True, 'i': False, 'h': None, 'g': [6.000000, 7.000000, "
                 "8.000000], 'd': {'f': 5.000000, 'e': 4.000000}, 'c': "
                 "3.000000, 'b': 2.000000, 'a': 1.000000}\r\n");
    EXPECT_STREQ(log_buff[1],
                 "{'k': 'string', 'j': True, 'i': False, 'h': None, 'g': [6.000000, "
                 "7.000000, 8.000000], 'd': {'f': 5.000000, 'e': 4.000000}, "
                 "'c': 3.000000, 'b': 2.000000, 'a': 1.000000}\r\n");
    EXPECT_STREQ(
        log_buff[0],
        "{'l': 1.234000, 'k': 'string', 'j': True, 'i': False, 'h': None, 'g': "
        "[6.000000, 7.000000, 8.000000], 'd': {'f': 5.000000, 'e': 4.000000}, "
        "'c': 3.000000, 'b': 2.000000, 'a': 1.000000}\r\n");
    /* deinit */
    obj_deinit(pikaMain);

    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_FLOAT_TYPE_DOUBLE
TEST(json, dumps) {
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/json/json_dumps.py");
    /* assert */
    EXPECT_STREQ(obj_getStr(pikaMain, "s1"),
                 "{\n\t\"c\":\t3,\n\t\"b\":\t2,\n\t\"a\":\t1\n}");
    EXPECT_STREQ(obj_getStr(pikaMain, "s2"),
                 "{\n\t\"d\":\t{\n\t\t\"f\":\t5,\n\t\t\"e\":\t4\n\t},\n\t\"c\":"
                 "\t3,\n\t\"b\":\t2,\n\t\"a\":\t1\n}");
    EXPECT_STREQ(obj_getStr(pikaMain, "s3"),
                 "{\n\t\"g\":\t[6, 7, "
                 "8],\n\t\"d\":\t{\n\t\t\"f\":\t5,\n\t\t\"e\":\t4\n\t},"
                 "\n\t\"c\":\t3,\n\t\"b\":\t2,\n\t\"a\":\t1\n}");
    EXPECT_STREQ(obj_getStr(pikaMain, "s4"),
                 "{\n\t\"l\":\t1.234,\n\t\"k\":\t\"string\",\n\t\"j\":\ttrue,"
                 "\n\t\"i\":\tfalse,\n\t\"h\":\tnull,\n\t\"g\":\t[6, 7, "
                 "8],\n\t\"d\":\t{\n\t\t\"f\":\t5,\n\t\t\"e\":\t4\n\t},"
                 "\n\t\"c\":\t3,\n\t\"b\":\t2,\n\t\"a\":\t1\n}");

    /* deinit */
    obj_deinit(pikaMain);

    EXPECT_EQ(pikaMemNow(), 0);
}
#endif
#endif

TEST_END