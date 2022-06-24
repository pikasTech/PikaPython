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
    obj_run(pikaMain,
            "def tryfn(a):\n"
            "    if a == 1:\n"
            "        print('a:1')\n"
            "        raise\n"
            "        print('a:1,after raise')\n"
            "        return\n"
            "    if a == 0:\n"
            "        print('a:0')\n"
            "        return\n"
            "try:\n"
            "    tryfn(1)\n"
            "    tryfn(0)\n"
            "except:\n"
            "    print('in except')\n"
            "\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
