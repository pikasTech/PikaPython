#include "test_common.h"

TEST(except, try1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
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

TEST(except, trycmodule1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain,
            "import pika_cjson\n"
            "try:\n"
            "    b = pika_cjson.Parse('')\n"
            "except:\n"
            "    print('parse faild')\n"
            "\n");
    /* collect */
    EXPECT_STREQ("BEGIN\r\n", log_buff[2]);
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(except, trydef1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
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
    pikaMemInfo.heapUsedMax = 0;
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