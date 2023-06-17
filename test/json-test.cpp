#include "test_common.h"
TEST_START

#if !PIKA_NANO_ENABLE
TEST_RUN_SINGLE_FILE_PASS(json, loads, "test/python/json/json_loads.py")

#if PIKA_FLOAT_TYPE_DOUBLE
TEST(json, dumps) {
    g_PikaMemInfo.heapUsedMax = 0;
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

TEST_RUN_SINGLE_FILE_PASS(_json, loads, "test/python/json/_json_loads.py")

TEST_RUN_SINGLE_FILE(json, speed, "test/python/json/json_speed.py")

#endif
#endif

TEST_END