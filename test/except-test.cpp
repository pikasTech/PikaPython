#include "test_common.h"
TEST_START

#if PIKA_SYNTAX_EXCEPTION_ENABLE
TEST(except, try1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain,
            "try:\n"
            "    print('before raise')\n"
            "    raise \n"
            "    print('after raise')\n"
            "except:\n"
            "    print('in except')\n"
            "\n");
    /* collect */
    EXPECT_STREQ("BEGIN\r\n", log_buff[2]);
    EXPECT_STREQ("before raise\r\n", log_buff[1]);
    EXPECT_STREQ("in except\r\n", log_buff[0]);
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(except, def_none) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain,
            "import pika_cjson\n"
            "b = pika_cjson.Parse('')\n"
            "if None == b:\n"
            "    print('None')\n"
            "\n");
    /* collect */
    EXPECT_STREQ("None\r\n", log_buff[0]);
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(except, trydef1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    /* run */
    pikaVM_runSingleFile(pikaMain, "../../examples/Exception/trydef1.py");
    /* collect */
    /* assert */
    EXPECT_STREQ("BEGIN\r\n", log_buff[2]);
    EXPECT_STREQ("a:1\r\n", log_buff[1]);
    EXPECT_STREQ("in except\r\n", log_buff[0]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(except, try1file) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    /* run */
    pikaVM_runSingleFile(pikaMain, "../../examples/Exception/try1.py");
    /* collect */
    EXPECT_STREQ("BEGIN\r\n", log_buff[2]);
    EXPECT_STREQ("before raise\r\n", log_buff[1]);
    EXPECT_STREQ("in except\r\n", log_buff[0]);
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(except, for_loop) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain,
            "sum = 0\n"
            "for i in range(0, 10):\n"
            "    try:\n"
            "        if i == 3:\n"
            "            raise\n"
            "        print(i)\n"
            "        sum += i\n"
            "    except:\n"
            "        print('in except')\n"
            "\n");
    /* collect */
    int sum = obj_getInt(pikaMain, "sum");
    /* assert */
    EXPECT_EQ(sum, 42);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(except, dict) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    /* run */
    pikaVM_runSingleFile(pikaMain, "../../examples/Exception/trydict.py");
    /* collect */
    int res = obj_getInt(pikaMain, "res");
    EXPECT_EQ(res, 5);
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(except, len) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    /* run */
    pikaVM_runSingleFile(pikaMain, "../../examples/Exception/trylen.py");
    /* collect */
    int res = obj_getInt(pikaMain, "res");
    EXPECT_EQ(res, 5);
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(except, trycmodule1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain,
            "import GTestTask\n"
            "try:\n"
            "    GTestTask.test_raise()\n"
            "    print('after failed')\n"
            "except:\n"
            "    print('parse failed')\n"
            "\n");
    /* collect */
    EXPECT_STREQ("BEGIN\r\n", log_buff[1]);
    EXPECT_STREQ("parse failed\r\n", log_buff[0]);
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(except, except_break) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain,
            "l = [1,2,3]\n"
            "sum = 0\n"
            "for i in range(10):\n"
            "    try:\n"
            "        sum += l[i]\n"
            "    except Exception:\n"
            "        print('in excepton')\n"
            "        break\n"
            "print(sum)\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_EQ(obj_getInt(pikaMain, "sum"), 6);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(except, while_try_while) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    /* run */
    pikaVM_runSingleFile(pikaMain, "test/python/except/while_try_while.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[2], "before try\r\n");
    EXPECT_STREQ(log_buff[1], "after try\r\n");
    EXPECT_STREQ(log_buff[0], "after while\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST_RUN_SINGLE_FILE(except,
                     try_while_return,
                     "test/python/except/try_while_return.py");

TEST_RUN_SINGLE_FILE(except,
                     isinstance,
                     "test/python/except/except_isinstance.py");

#endif

TEST_END