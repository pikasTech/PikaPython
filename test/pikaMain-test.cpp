#include "test_common.h"
TEST_START

TEST(pikaMain, init) {
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = pikaScriptInit();
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, new_mem) {
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    VMParameters* globals = obj_run(pikaMain,
                                    "mem = PikaStdLib.MemChecker()\n"
                                    "mem.max()\n");
    PikaObj* mem = (PikaObj*)args_getPtr(globals->list, "mem");
    PikaObj* memClassPtr = (PikaObj*)mem->constructor;
    EXPECT_EQ((void*)memClassPtr, (void*)New_PikaStdLib_MemChecker);
    obj_deinit(pikaMain);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, unitTest) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/UnitTest.py");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);

    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, list_new) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    VMParameters* globals = obj_run(pikaMain,
                                    "list = PikaStdData.List()\n"
                                    "list.append(7)\n"
                                    "list.append('eee')\n"
                                    "len = list.len()\n"
                                    "a1 = list.get(0)\n"
                                    "a2 = list.get(1)\n");
    /* collect */
    int len = obj_getInt(globals, "len");
    int a1 = obj_getInt(globals, "a1");
    char* a2 = obj_getStr(globals, "a2");

    /* assert */
    EXPECT_EQ(len, 2);
    EXPECT_EQ(a1, 7);
    EXPECT_STREQ(a2, "eee");

    /* deinit */
    // obj_deinit(globals);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, int_float_convert) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    VMParameters* globals = obj_run(pikaMain,
                                    "a = 1\n"
                                    "a_f = float(a)\n"
                                    "b = 1.3\n"
                                    "b_i = int(b)\n");
    /* collect */
    int a = obj_getInt(globals, "a");
    float a_f = obj_getFloat(globals, "a_f");
    float b = obj_getFloat(globals, "b");
    int b_i = obj_getInt(globals, "b_i");

    /* assert */
    EXPECT_EQ(a, 1);
    EXPECT_FLOAT_EQ(a_f, 1);
    EXPECT_FLOAT_EQ(b, 1.3);
    EXPECT_FLOAT_EQ(b_i, 1);

    /* deinit */
    // obj_deinit(globals);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, type_) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "type(1)\n"
            "b = 1.4\n"
            "type(b)\n");
    /* collect */

    /* assert */

    /* deinit */
    // obj_deinit(globals);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, a_signed) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "a = -1\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");

    /* assert */
    EXPECT_EQ(-1, a);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, a_mm) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "a = -1.1 ** 3\n");
    /* collect */
    float a = obj_getFloat(pikaMain, "a");

    /* assert */
    EXPECT_FLOAT_EQ(-1.331, a);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, a_n1) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "a = -2\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");

    /* assert */
    EXPECT_EQ(-2, a);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, a_l4) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "a = 1<<4\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");

    /* assert */
    EXPECT_EQ(16, a);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, less_equ) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 0\n"
            "while a <= 10:\n"
            "    a += 1\n"
            "\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");

    /* assert */
    EXPECT_EQ(11, a);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, and_or_not) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 0\n"
            "b = 0\n"
            "c = 0\n"
            "d = 0\n"
            "if (1>2) or (2>1):\n"
            "    a = 1\n"
            "\n"
            "if (1>2) and (2>1):\n"
            "    b = 1\n"
            "\n"
            "if not (1>2):\n"
            "    c = 1\n"
            "\n"
            "if not (2>1):\n"
            "    d = 1\n"
            "\n"
            "e = ~1\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    int b = obj_getInt(pikaMain, "b");
    int c = obj_getInt(pikaMain, "c");
    int d = obj_getInt(pikaMain, "d");
    int e = obj_getInt(pikaMain, "e");

    /* assert */
    EXPECT_EQ(1, a);
    EXPECT_EQ(0, b);
    EXPECT_EQ(1, c);
    EXPECT_EQ(0, d);
    EXPECT_EQ(e, -2);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, err_scop) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "print('test'");
    /* collect */

    /* assert */

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, PikaStdData) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "dict = PikaStdData.Dict()\n"
            "dict.set('a', 1)\n"
            "dict.set('b', 2)\n"
            "dict.remove('b')\n"
            "a = dict.get('a')\n"
            "b = dict.get('b')\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    int b = obj_getInt(pikaMain, "b");
    /* assert */
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, _PIKA_INT_ERR);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, list_method) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "list = PikaStdData.List()\n"
            "list.append(7)\n"
            "list.append('eee')\n"
            "len = list.len()\n"
            "a1 = list.get(0)\n"
            "a2 = list.get(1)\n"
            "\n");
    /* collect */
    /* assert */
    /* deinit */
    // obj_deinit(globals);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, list_iter) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "list = PikaStdData.List()\n"
            "res = iter(list)\n"
            "\n");
    /* collect */
    PikaObj* res = (PikaObj*)obj_getPtr(pikaMain, "res");
    PikaObj* list = (PikaObj*)obj_getPtr(pikaMain, "list");
    int ref_cnt = obj_refcntNow(list);
    EXPECT_EQ(ref_cnt, 2);
    /* assert */
    EXPECT_EQ(res, list);
    /* deinit */
    // obj_deinit(globals);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, list_for_loop) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "list = PikaStdData.List()\n"
            "list.append(7)\n"
            "list.append('eee')\n"
            "len = list.len()\n"
            "for item in list:\n"
            "    print(item)\n"
            "    a = item\n"
            "\n");
    /* collect */
    char* a = obj_getStr(pikaMain, "a");
    /* assert */
    EXPECT_STREQ(a, "eee");
    /* deinit */
    // obj_deinit(globals);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, range) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "r = range(10, 0)\n"
            "r\n"
            "\n");
    /* collect */
    /* assert */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_in_range) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 0\n"
            "for i in range(0, 10):\n"
            "    print(i)\n"
            "    a = a + i\n"
            "\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 45);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_for_in_range) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 0\n"
            "for i in range(0, 10):\n"
            "    for k in range(0, 3):\n"
            "        print(k)\n"
            "        a = a + k\n"
            "\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 30);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_if_break) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 0\n"
            "for i in range(0, 10):\n"
            "    if i == 5:\n"
            "        break\n"
            "    a = a + i\n"
            "\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 10);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_if_continue) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 0\n"
            "for i in range(0, 10):\n"
            "    if i == 5:\n"
            "        continue\n"
            "    a = a + i\n"
            "\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 40);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

/* the log_buff of printf */
extern char log_buff[LOG_BUFF_MAX][LOG_SIZE];
TEST(pikaMain, print_in_def) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    /* the test is used to fix too many print in def */
    /* clear the log_buff */
    memset(log_buff, 0, LOG_BUFF_MAX * LOG_SIZE);
    obj_run(pikaMain,
            "def test_print():\n"
            "    print('test')\n"
            "test_print()\n");
    /* collect */
    /* assert */
    /* should only print once, so the second log (log_buff[1]) shuold be '\0' */
    EXPECT_STREQ(log_buff[0], "test\r\n");
    EXPECT_STREQ(log_buff[1], "");
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, prime_100) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "num = 0\n"
            "i = 2\n"
            "while i < 100:\n"
            "    j=2\n"
            "    is_prime = 1\n"
            "    while j < i:\n"
            "        if i%j==0 :\n"
            "            is_prime = 0\n"
            "            break\n"
            "        j+=1\n"
            "    if is_prime:\n"
            "        num = num + i\n"
            "    i+=1\n"
            "\n");
    /* collect */
    int num = obj_getInt(pikaMain, "num");
    /* assert */
    EXPECT_EQ(num, 1060);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

/* the log_buff of printf */
extern char log_buff[LOG_BUFF_MAX][LOG_SIZE];
TEST(pikaMain, for_in_dict) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "print('__start__')\n"
            "dict = PikaStdData.Dict()\n"
            "dict.set('a', 1)\n"
            "dict.set('b', 2)\n"
            "dict.remove('b')\n"
            "for item in dict:\n"
            "    print(item)\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "a\r\n");
    EXPECT_STREQ(log_buff[1], "__start__\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, str_add) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 'a'\n"
            "b = 'b'\n"
            "c = a + b\n"
            "d = 1\n"
            "e = 1.2\n"
            "g = c + str(d)\n"
            "h = c + str(d) + str(e)\n"
            "\n");
    /* collect */
    char* c = obj_getStr(pikaMain, "c");
    char* g = obj_getStr(pikaMain, "g");
    char* h = obj_getStr(pikaMain, "h");
    /* assert */
    EXPECT_STREQ(c, "ab");
    EXPECT_STREQ(g, "ab1");
    EXPECT_STREQ(h, "ab11.200000");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, str_add_print) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "print('test: ' + str(2233))\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "test: 2233\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, int_float_to_str) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = str(1)\n"
            "b = str(1.2)\n"
            "\n");
    /* collect */
    char* a = obj_getStr(pikaMain, "a");
    char* b = obj_getStr(pikaMain, "b");
    /* assert */
    EXPECT_STREQ(a, "1");
    EXPECT_STREQ(b, "1.200000");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, str_eq) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = '1ee'\n"
            "b = '1ee'\n"
            "c = '1e'\n"
            "if a==b:\n"
            "    res1 = 1\n"
            "else:\n"
            "    res1 = 0\n"
            "if a==c:\n"
            "    res2 = 1\n"
            "else:\n"
            "    res2 = 0\n"
            "\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");
    /* assert */
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 0);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, print_with_enter) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain, "print('test\\n')\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "test\n\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, print_with_2enter) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain, "print('test\\r\\n\\n')\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "test\r\n\n\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, print_ddd) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain, "print(\"[Info]: in Python config ...\")\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "[Info]: in Python config ...\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_in_string) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "s = PikaStdData.String('test')\n"
            "for c in s:\n"
            "    print(c)\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[4], "BEGIN\n");
    EXPECT_STREQ(log_buff[3], "t\r\n");
    EXPECT_STREQ(log_buff[2], "e\r\n");
    EXPECT_STREQ(log_buff[1], "s\r\n");
    EXPECT_STREQ(log_buff[0], "t\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_no_init_arg) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "s = PikaStdData.String()\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[3], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[2],
                 "TypeError: __init__() takes 1 positional argument but 0 were "
                 "given\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, obj_no_free) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "s = PikaStdData.String()\n"
            "s = PikaStdData.String()\n"
            "\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if 0
TEST(pikaMain, list__setitem__) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "list = PikaStdData.List()\n"
            "list = __setitem__(list, 0, 2)\n"
            "res = __getitem__(list, 0)\n"
            "\n");
    /* collect */
    int res = obj_getInt(pikaMain, "res");
    /* assert */
    EXPECT_EQ(res, 2);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, string__getitem__) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "s = PikaStdData.String('test')\n"
            "res = __getitem__(s, 2)\n"
            "\n");
    /* collect */
    char* res = obj_getStr(pikaMain, "res");
    /* assert */
    EXPECT_STREQ(res, "s");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, dict__setitem__get) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "list = PikaStdData.Dict()\n"
            "list = __setitem__(list, 'a', 2)\n"
            "res = __getitem__(list, 'a')\n"
            "\n");
    /* collect */
    int res = obj_getInt(pikaMain, "res");
    /* assert */
    EXPECT_EQ(res, 2);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, str___getitem____setitem__) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "s = 'test'\n"
            "res = __getitem__(s, 2)\n"
            "s = __setitem__(s, 2, 'q')\n"
            "\n");
    /* collect */
    char* res = obj_getStr(pikaMain, "res");
    char* s = obj_getStr(pikaMain, "s");
    /* assert */
    EXPECT_STREQ(res, "s");
    EXPECT_STREQ(s, "teqt");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, str__index__) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "s = 'test'\n"
            "res = s[2]\n"
            "res2 = 'eqrt'[2]\n"
            "s[2] = 'q'\n"
            "\n");
    /* collect */
    char* res = obj_getStr(pikaMain, "res");
    char* res2 = obj_getStr(pikaMain, "res2");
    char* s = obj_getStr(pikaMain, "s");
    /* assert */
    EXPECT_STREQ(res, "s");
    EXPECT_STREQ(res2, "r");
    EXPECT_STREQ(s, "teqt");
    // EXPECT_STREQ(s, "teqt");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, bytes__index__) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "s = b'test'\n"
            "res = s[2]\n"
            "res2 = b'eqrt'[2]\n"
            "s[2] = b'q'\n"
            "\n");
    /* collect */
    uint8_t* res = obj_getBytes(pikaMain, "res");
    uint8_t* res2 = obj_getBytes(pikaMain, "res2");
    uint8_t* s = obj_getBytes(pikaMain, "s");
    /* assert */
    EXPECT_STREQ((char*)res, "s");
    EXPECT_STREQ((char*)res2, "r");
    EXPECT_EQ(s[2], 'q');
    // EXPECT_STREQ(s, "teqt");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, list_index) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "list = PikaStdData.List()\n"
            "list.append(2)\n"
            "res = list[0]\n"
            "\n");
    /* collect */
    int res = obj_getInt(pikaMain, "res");
    /* assert */
    EXPECT_EQ(res, 2);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, dict_index) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "dict = PikaStdData.Dict()\n"
            "dict['a'] = 2\n"
            "res = dict['a']\n"
            "\n");
    /* collect */
    int res = obj_getInt(pikaMain, "res");
    /* assert */
    EXPECT_EQ(res, 2);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, task_run_once) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain,
            "def todo1():\n"
            "    print('task 1 running...')\n"
            "def todo2():\n"
            "    print('task 2 running...')\n"
            "task = GTestTask.Task()\n"
            "task.call_always(todo1)\n"
            "task.call_always(todo2)\n"
            "task.run_once()\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "task 2 running...\r\n");
    EXPECT_STREQ(log_buff[1], "task 1 running...\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, task_run_when) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain,
            "def todo1():\n"
            "    print('task 1 running...')\n"
            "def todo2():\n"
            "    print('task 2 running...')\n"
            "def todo3():\n"
            "    print('task 3 running...')\n"
            "def when3():\n"
            "    return True\n"
            "task = GTestTask.Task()\n"
            "task.call_always(todo1)\n"
            "task.call_always(todo2)\n"
            "task.call_when(todo3, when3)\n"
            "task.run_once()\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "task 3 running...\r\n");
    EXPECT_STREQ(log_buff[1], "task 2 running...\r\n");
    EXPECT_STREQ(log_buff[2], "task 1 running...\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(pikaMain, task_run_period_until) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "import PikaStdLib\n"
            "mem = PikaStdLib.MemChecker()\n"
            "mem_now = 0\n"
            "mem_max = 0\n"
            "mem_lack = False\n"
            "def todo1():\n"
            "    global mem_now\n"
            "    global mem_lack\n"
            "    global mem_max\n"
            "    mem_max = mem.getMax()\n"
            "    print('task 1 running...')\n"
            "    if mem_now == 0:\n"
            "        mem_now = mem.getNow()\n"
            "    else:\n"
            "        if mem.getNow() > mem_now + 1:\n"
            "            mem_lack = True\n"
            "            print(cformat('%0.8f', mem.getNow()))\n"
            "def todo2():\n"
            "    print('task 2 running...')\n"
            "def todo3():\n"
            "    print('task 3 running...')\n"
            "def when3():\n"
            "    return True\n"
            "task = GTestTask.Task()\n"
            "task.call_period_ms(todo1, 200)\n"
            "task.call_period_ms(todo2, 500)\n"
            "# task.call_always(todo3)\n"
            "task.run_until_ms(1000)\n"
            "\n");
    /* collect */
    PIKA_BOOL mem_lack = obj_getBool(pikaMain, "mem_lack");
    /* assert */
    EXPECT_STREQ(log_buff[0], "task 1 running...\r\n");
    EXPECT_STREQ(log_buff[1], "task 1 running...\r\n");
    EXPECT_STREQ(log_buff[2], "task 2 running...\r\n");
    EXPECT_STREQ(log_buff[3], "task 1 running...\r\n");
    EXPECT_STREQ(log_buff[4], "task 1 running...\r\n");
    EXPECT_STREQ(log_buff[5], "task 2 running...\r\n");
    EXPECT_STREQ(log_buff[6], "task 1 running...\r\n");
    EXPECT_STREQ(log_buff[7], "BEGIN\r\n");
    EXPECT_EQ(mem_lack, PIKA_FALSE);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, fun_call) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def fun(a,b,c,d,e):\n"
            "    print(a)\n"
            "    print(b)\n"
            "    print(c)\n"
            "    print(d)\n"
            "    print(e)\n"
            "fun(10,20,30,40, 'xxx')\n"
            "\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "xxx\r\n");
    EXPECT_STREQ(log_buff[1], "40\r\n");
    EXPECT_STREQ(log_buff[2], "30\r\n");
    EXPECT_STREQ(log_buff[3], "20\r\n");
    EXPECT_STREQ(log_buff[4], "10\r\n");
    EXPECT_STREQ(log_buff[5], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, global) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "x = 0\n"
            "a = 0\n"
            "z = 0\n"
            "def test_global():\n"
            "    global x\n"
            "    global y, z\n"
            "    x = 1\n"
            "    a = 1\n"
            "    z = 1\n"
            "    print('test global')\n"
            "test_global()\n"
            "\n");
    /* assert */
    int x = obj_getInt(pikaMain, "x");
    int a = obj_getInt(pikaMain, "a");
    int z = obj_getInt(pikaMain, "z");
    EXPECT_EQ(x, 1);
    EXPECT_EQ(a, 0);
    EXPECT_EQ(z, 1);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, complex_str) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "print('test,test')\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "test,test\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, syntax_err_1) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "print('testtest)\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "Error: Syntax error.\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_arg) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_linesToAsm(buffs,
                                      "class Test(TinyObj):\n"
                                      "    x = 1\n"
                                      "\n"
                                      "test = Test()\n"
                                      "print(test.x)\n");
    printf("%s", pikaAsm);
    pikaVM_runAsm(pikaMain, pikaAsm);
    /* assert */
    EXPECT_STREQ(log_buff[0], "1\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_def) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class Test():\n"
            "    x = 1\n"
            "    def hello(self, x):\n"
            "        return x + 2\n"
            "test = Test()\n"
            "x = test.hello(test.x)\n"
            "print(x)\n");
    /* collect */
    PikaObj* test = obj_getObj(self, "test");
    EXPECT_EQ(obj_refcntNow(test), 1);
    /* assert */
    EXPECT_STREQ(log_buff[0], "3\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_def_print) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class Test():\n"
            "    x = 1\n"
            "    def hi(self):\n"
            "        print('hi' + str(self.x))\n"
            "test = Test()\n"
            "test.hi()\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "hi1\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_demo_1) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class MyClass:\n"
            "    i = 12345\n"
            "    def f(self):\n"
            "        return 'hello world'\n"
            "        \n"
            "x = MyClass()\n"
            " \n"
            "print(x.i)\n"
            "print(x.f())\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "hello world\r\n");
    EXPECT_STREQ(log_buff[1], "12345\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_demo_2) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class Complex:\n"
            "    def __init__(self):\n"
            "        self.r = 3.0\n"
            "        self.i = -4.5\n"
            "x = Complex()\n"
            "print(x.r)\n"
            "print(x.i)\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "-4.500000\r\n");
    EXPECT_STREQ(log_buff[1], "3.000000\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_demo_3) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class people:\n"
            "    def speak(self):\n"
            "        print('i am a people')\n"
            " \n"
            "class student(people):\n"
            "    def speak(self):\n"
            "        print('i am a student')\n"
            " \n"
            "p = people()\n"
            "s = student()\n"
            "p.speak()\n"
            "s.speak()\n");
    /* assert */
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], "i am a people\r\n");
    EXPECT_STREQ(log_buff[0], "i am a student\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_demo_save_asm) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class people:\n"
            "    def speak(self):\n"
            "        print('i am a people')\n"
            " \n"
            "class student(people):\n"
            "    def speak(self):\n"
            "        print('i am a student')\n"
            " \n");
    obj_run(self,
            "p = people()\n"
            "s = student()\n"
            "p.speak()\n"
            "s.speak()\n");
    /* assert */
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], "i am a people\r\n");
    EXPECT_STREQ(log_buff[0], "i am a student\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_if_continue_byte_code) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    pikaVM_run(pikaMain,
               "a = 0\n"
               "for i in range(0, 10):\n"
               "    if i == 5:\n"
               "        continue\n"
               "    a = a + i\n"
               "\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 40);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, print_in_def_byte_code) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_run(pikaMain,
               "def test_print():\n"
               "    print('test')\n"
               "test_print()\n");
    /* collect */
    /* assert */
    /* should only print once, so the second log (log_buff[1]) shuold be '\0' */
    EXPECT_STREQ(log_buff[0], "test\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_demo_1_file) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    char lines[] =
        "class MyClass:\n"
        "    i = 12345\n"
        "    def f(self):\n"
        "        return 'hello world'\n"
        "        \n"
        "x = MyClass()\n"
        " \n"
        "print(x.i)\n"
        "print(x.f())\n";
    pikaCompile("pika_bytecode.bin", lines);
    char bytecodebuff[4096] = {0};
    FILE* f = __platform_fopen("pika_bytecode.bin", "rb");
    printf("size: %ld\n", __platform_fread(bytecodebuff, 1, 4096, f));
    __platform_printf("BEGIN\r\n");
    pikaVM_runByteCode(self, (uint8_t*)bytecodebuff);
    /* assert */
    EXPECT_STREQ(log_buff[0], "hello world\r\n");
    EXPECT_STREQ(log_buff[1], "12345\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    __platform_fclose(f);
    EXPECT_EQ(pikaMemNow(), 0);
}

// #if !PIKA_NANO_ENABLE
// TEST(pikaMain, get_native_method) {
//     /* init */
//     g_pikaMemInfo.heapUsedMax = 0;
//     PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
//     __platform_printf("BEGIN\r\n");
//     /* do some thing */
//     Args args = {0};
//     args_setStr(&args, "val", "t");
//     obj_runNativeMethod(pikaMain, "ord", &args);
//     args_deinit_stack(&args);
//     /* assert */
//     EXPECT_STREQ(log_buff[0], "test\r\n");
//     EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
//     /* deinit */
//     obj_deinit(pikaMain);
//     EXPECT_EQ(pikaMemNow(), 0);
// }
// #endif

TEST(pikaMain, hex_list) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_run(pikaMain,
               "bytes = PikaStdData.List()\n"
               "bytes.append(0x0F)\n"
               "bytes.append(0xFF)\n"
               "sum = 0\n"
               "for byte in bytes:\n"
               "    sum = sum + byte\n"
               "\n"
               "print(sum)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "270\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, bytearray) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_run(pikaMain,
               "bytes = PikaStdData.ByteArray(b'test')\n"
               "sum = 0\n"
               "for byte in bytes:\n"
               "    sum += byte\n"
               "print(bytes)\n"
               "\n");
    /* collect */
    int sum = obj_getInt(pikaMain, "sum");
    /* assert */
    EXPECT_EQ(sum, 448);
    EXPECT_STREQ(log_buff[0], "bytearray(b'\\x74\\x65\\x73\\x74')\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, not_4_space) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_run(pikaMain, "  print('test')\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "Error: Syntax error.\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, self_operator) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_run(pikaMain,
               "a = 1\n"
               "b = 2\n"
               "c = 3\n"
               "d = 4\n"
               "a += 2\n"
               "b -= 1+1-3\n"
               "c *= 4\n"
               "d /= 2\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    int b = obj_getInt(pikaMain, "b");
    int c = obj_getInt(pikaMain, "c");
    int d = obj_getInt(pikaMain, "d");
    /* assert */
    EXPECT_EQ(a, 3);
    EXPECT_EQ(b, 3);
    EXPECT_EQ(c, 12);
    EXPECT_EQ(d, 2);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_demo_2_initwitharg) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class Complex:\n"
            "    def __init__(self, r, i):\n"
            "        self.r = r\n"
            "        self.i = i\n"
            "x = Complex(3.0, -4.5)\n"
            "print(x.r)\n"
            "print(x.i)\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "-4.500000\r\n");
    EXPECT_STREQ(log_buff[1], "3.000000\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, def_args_err) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "print()\n");
    // obj_run(pikaMain, "printNoEnd()\n");
    /* collect */

    /* assert */

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

// TEST(pikaMain, class_args_err) {
//     /* init */
//     g_pikaMemInfo.heapUsedMax = 0;
//     PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
//     /* run */
//     obj_run(pikaMain, "mem = PikaStdLib.MemChecker(0)\n");
//     /* collect */

//     /* assert */
//     EXPECT_STREQ(log_buff[4],
//                  "TypeError: PikaStdLib.MemChecker() takes no
//                  arguments\r\n");
//     /* deinit */
//     obj_deinit(pikaMain);
//     EXPECT_EQ(pikaMemNow(), 0);
// }

TEST(pikaMain, int_) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "a = int('3')\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 3);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, len_) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "a = len('331dd')\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 5);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, def_in_shell) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);

    /* skip the first obj_run */
    __platform_printf("BEGIN\r\n");
    /* as run in shell */
    obj_run(pikaMain,
            "def test():\n"
            "    print('test')\n"
            "\n");
    obj_run(pikaMain,
            "def test2():\n"
            "    print('test2')\n"
            "\n");
    obj_run(pikaMain, "test()\n");
    obj_run(pikaMain, "test2()\n");
    /* collect */
    /* assert */
    EXPECT_STREQ("test2\r\n", log_buff[0]);
    EXPECT_STREQ("test\r\n", log_buff[1]);
    EXPECT_STREQ("BEGIN\r\n", log_buff[2]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, def_in_shell_override) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);

    /* skip the first obj_run */
    __platform_printf("BEGIN\r\n");
    /* as run in shell */
    obj_run(pikaMain,
            "def test():\n"
            "    print('test')\n"
            "\n");
    obj_run(pikaMain, "test()\n");
    obj_run(pikaMain,
            "def test():\n"
            "    print('test2')\n"
            "\n");
    obj_run(pikaMain, "test()\n");
    /* collect */
    /* assert */
    EXPECT_STREQ("test2\r\n", log_buff[0]);
    EXPECT_STREQ("test\r\n", log_buff[1]);
    EXPECT_STREQ("BEGIN\r\n", log_buff[2]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_in_shell) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);

    /* skip the first obj_run */
    __platform_printf("BEGIN\r\n");
    /* as run in shell */
    obj_run(pikaMain,
            "class Test():\n"
            "    def hi(self):\n"
            "        print('hi')\n"
            "\n");
    obj_run(pikaMain,
            "t = Test()\n"
            "t.hi()\n");
    /* collect */
    /* assert */
    EXPECT_STREQ("hi\r\n", log_buff[0]);
    EXPECT_STREQ("BEGIN\r\n", log_buff[1]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_TinyObj) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);

    /* skip the first obj_run */
    __platform_printf("BEGIN\r\n");
    /* as run in shell */
    obj_run(pikaMain,
            "class MyClass( TinyObj ):\n"
            "    def __init__(self):\n"
            "        self.val = 123\n"
            "\n");
    obj_run(pikaMain,
            "x = MyClass()\n"
            "x.val\n");
    /* collect */
    /* assert */
    EXPECT_STREQ("123\r\n", log_buff[0]);
    EXPECT_STREQ("BEGIN\r\n", log_buff[1]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, list_sysobj) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);

    /* skip the first obj_run */
    obj_run(pikaMain,
            "a = list()\n"
            "a.len()\n");
    /* as run in shell */
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_BUILTIN_STRUCT_ENABLE
TEST(pikaMain, list_init) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "a = [1, 2, 3]\n"
            "a.len()\n");
    /* as run in shell */
    /* collect */
    /* assert */
    EXPECT_STREQ("3\r\n", log_buff[0]);
    EXPECT_STREQ("BEGIN\r\n", log_buff[1]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, ctypes) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    char* lines =
        "t = ctypes.Test()\n"
        "sendbuf = b'\\x03\\x04\\x33\\x00\\x05'\n"
        "rlen = ctypes.c_uint(0)\n"
        "rcvbuf = ctypes.c_char_p('')\n"
        "res = t.dc_cpuapdu_hex(5, sendbuf, rlen, rcvbuf)\n"
        "t.print_rcv(rcvbuf)\n";
    __platform_printf("%s\n", lines);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* as run in shell */
    /* collect */
    Arg* sendbuf = obj_getArg(pikaMain, "sendbuf");
    Arg* rcvbuf = obj_getArg(pikaMain, "rcvbuf.value");
    /* assert */
    EXPECT_EQ(arg_getBytesSize(sendbuf), 5);
    EXPECT_EQ(arg_getBytesSize(rcvbuf), 6);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, import_err) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    char* lines = "import qqpe\n";
    __platform_printf("%s\n", lines);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* as run in shell */
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[2],
                 "ModuleNotFoundError: No module named 'qqpe'\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, cmodule_in_py) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = pikaScriptInit();
    char* lines =
        "import test_cmodule\n"
        "test_cmodule.test_mem()\n";
    __platform_printf("%s\n", lines);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* as run in shell */
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], "mem used max:\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class___str___print) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class Test():\n"
            "    def __str__(self):\n"
            "        return 'test'\n"
            "test = Test()\n"
            "print(test)\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "test\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, CModule__str__) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "op = PikaMath.Operator()\n"
            "print(op)\n"
            "print('obj: ' + str(op))\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "obj: test\r\n");
    EXPECT_STREQ(log_buff[1], "test\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_STREQ(log_buff[0], "del operator...\r\n");
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(pikaMain, builtin_hex) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "hex(3)\n"
            "hex(-3)\n"
            "hex(1234)\n"
            "hex(-12039)\n");
    /* assert */
    EXPECT_STREQ(log_buff[4], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[3], "'0x03'\r\n");
    EXPECT_STREQ(log_buff[2], "'-0x03'\r\n");
    EXPECT_STREQ(log_buff[1], "'0x4d2'\r\n");
    EXPECT_STREQ(log_buff[0], "'-0x2f07'\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(pikaMain, builtin_ord) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "ord('a')\n");
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "97\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(pikaMain, builtin_chr) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "chr(97)\n");
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "'a'\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, iteral_oct) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "0o100\n");
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "64\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, iteral_bin) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "0b100\n");
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "4\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_push_mode) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    char lines[] =
        "print('test')\n"
        "for i in range(0, 10):\n"
        "    i\n"
        "\n";
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(self, lines[i]);
    }
    /* assert */
    // EXPECT_STREQ(log_buff[18], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[14], "test\r\n");
    EXPECT_STREQ(log_buff[11], "... ");
    EXPECT_STREQ(log_buff[1], "9\r\n");
    EXPECT_STREQ(log_buff[0], ">>> ");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_push_mode_err) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    char lines[] =
        "def test:\n"
        "    print('test')\n"
        "\n";
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(self, lines[i]);
    }
    /* assert */
    EXPECT_STREQ(log_buff[1], "Error: Syntax error.\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if 0
TEST(pikaMain, REPL_input) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    char lines[] =
        "res = input('input:\\n')\n"
        "1234\n"
        "print(res)\n";
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(self, lines[i]);
    }
    /* assert */
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, int_from_bytes) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "int(b'test'[0])\n");
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "116\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

// TEST(pikaMain, big_bytes) {
//     /* init */
//     g_pikaMemInfo.heapUsedMax = 0;
//     /* run */
//     PikaObj* self = newRootObj("pikaMain", New_PikaMain);
//     __platform_printf("BEGIN\r\n");
//     obj_run(self, "b = bytes(8192)\n");
//     /* assert */
//     /* deinit */
//     obj_deinit(self);
//     EXPECT_EQ(pikaMemNow(), 0);
// }

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, utils_int_to_bytes) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "ut = PikaStdData.Utils()\n"
            "b = b'test'\n"
            "b[0] = ut.int_to_bytes(0x35)\n"
            "b\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "b'\\x35\\x65\\x73\\x74'\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, neg_index) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "'test'[-2]\n"
            "b'test'[-2]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "b'\\x73'\r\n");
    EXPECT_STREQ(log_buff[1], "'s'\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, slice1) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "'test'[1:3]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "'es'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, slice2) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "b'test'[1:3]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "b'\\x65\\x73'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, c_buffer) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "version = ctypes.c_buffer(b\"\", 128)\n");
    /* assert */
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, str_add1) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "i = 32\n"
            "msg = \"device_names[\" + str(i) + \"]:\"\n"
            "msg\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "'device_names[32]:'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, slice_eei) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "'test'[1:-1]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "'es'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, slice_a97) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "'test'[:-2]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "'te'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, slice_a90) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "'test'[1:]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "'est'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, slice_a12) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "b'test'[1:]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "b'\\x65\\x73\\x74'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, str_string) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "a = PikaStdData.String('test')\n"
            "print(a)\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "test\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, string_index) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "a = PikaStdData.String('test')\n"
            "a[1]\n"
            "a[1] = 'q'\n"
            "print(a)\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "tqst\r\n");
    EXPECT_STREQ(log_buff[1], "'e'\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, list_for_append) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "rcv_buf = PikaStdData.List()\n"
            "for i in range(0, 1024):\n"
            "    rcv_buf.append(0)\n"
            "rcv_buf.len()\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "1024\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_str) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "str(PikaStdData.String('test'))\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "'test'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, module_import_from_module) {
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(self, pikaModules_py_a);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "import test_module1\n"
            "test_module1.test_module_import()\n");
    EXPECT_STREQ(log_buff[0], "test_module_2_hello\r\n");
    EXPECT_STREQ(log_buff[1], "in test module 2\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, module_1_module2_test) {
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(self, pikaModules_py_a);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "import test_module1\n"
            "test_module1.test_module2.mytest()\n");
    EXPECT_STREQ(log_buff[0], "test_module_2_hello\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_startswith) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test')\n"
            "res1 = a.startswith('te')\n"
            "res2 = a.startswith('st')\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");

    /* assert */
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_endswith) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test')\n"
            "res1 = a.endswith('te')\n"
            "res2 = a.endswith('st')\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");

    /* assert */
    EXPECT_EQ(res1, 0);
    EXPECT_EQ(res2, 1);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_isdigit) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test')\n"
            "b = PikaStdData.String('1234')\n"
            "c = PikaStdData.String('test1234')\n"
            "res1 = a.isdigit()\n"
            "res2 = b.isdigit()\n"
            "res3 = c.isdigit()\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");
    int res3 = obj_getInt(pikaMain, "res3");

    /* assert */
    EXPECT_EQ(res1, 0);
    EXPECT_EQ(res2, 1);
    EXPECT_EQ(res3, 0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_islower) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test')\n"
            "b = PikaStdData.String('1234')\n"
            "c = PikaStdData.String('Test')\n"
            "res1 = a.islower()\n"
            "res2 = b.islower()\n"
            "res3 = c.islower()\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");
    int res3 = obj_getInt(pikaMain, "res3");

    /* assert */
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 0);
    EXPECT_EQ(res3, 0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_isalnum) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test1234')\n"
            "b = PikaStdData.String('1234')\n"
            "c = PikaStdData.String('  ')\n"
            "res1 = a.isalnum()\n"
            "res2 = b.isalnum()\n"
            "res3 = c.isalnum()\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");
    int res3 = obj_getInt(pikaMain, "res3");

    /* assert */
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 1);
    EXPECT_EQ(res3, 0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_isalpha) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test')\n"
            "b = PikaStdData.String('1234')\n"
            "c = PikaStdData.String('  ')\n"
            "res1 = a.isalpha()\n"
            "res2 = b.isalpha()\n"
            "res3 = c.isalpha()\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");
    int res3 = obj_getInt(pikaMain, "res3");

    /* assert */
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 0);
    EXPECT_EQ(res3, 0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_isspace) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test')\n"
            "b = PikaStdData.String('  ')\n"
            "res1 = a.isspace()\n"
            "res2 = b.isspace()\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");

    /* assert */
    EXPECT_EQ(res1, 0);
    EXPECT_EQ(res2, 1);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, print_obj) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "mem = PikaStdLib.MemChecker()\n"
            "mem\n"
            "print(mem)\n"
            "res = str(mem)\n");
    /* collect */
    char* res = obj_getStr(pikaMain, "res");

    /* assert */
    EXPECT_EQ(strIsStartWith(log_buff[0], "<object at "), true);
    EXPECT_EQ(strIsStartWith(log_buff[1], "<object at "), true);
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_EQ(strIsStartWith(res, "<object at "), true);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, num_issue1) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    char* lines = "(((1 + (2 * 3)/(4 + 5))*(6 - 7) + (8 + 9) * 10)/11) - 12\n";
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "3.303030\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, returnNullString) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    char* lines =
        "import GTestTask\n"
        "task = GTestTask.Task()\n"
        "def test_return_none():\n"
        "    s = task.returnNullString()\n"
        "    return s\n"
        "s = test_return_none()\n"
        "print(s)\n"
        "if None == s:\n"
        "    print('s is None')\n";
    obj_run(pikaMain, lines);
    /* collect */
    Arg* s = obj_getArg(pikaMain, "s");
    /* assert */
    EXPECT_EQ((uintptr_t)s, NULL);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_loop_issue_1b2a3f1bdf) {
    char* lines =
        "res = 0\n"
        "for i in range(0, 10):\n"
        "    if i < 3:\n"
        "        continue\n"
        "    if i > 7:\n"
        "        continue\n"
        "    for i in range(i, i+3):\n"
        "        res += i\n"
        "\n";

    Args* buffs = New_strBuff();
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);

    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");

    obj_run(pikaMain, lines);
    /* collect */
    int res = obj_getInt(pikaMain, "res");
    /* assert */
    EXPECT_EQ(res, 90);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#define pika_floatYPE_DOUBLE 0
TEST(pikaMain, syantex_issue123lkjxi) {
    char* lines = "if i < 3\n";
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "Error: Syntax error.\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, syantex_issue_fae13) {
    char* lines = "for i < 3\n";
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "Error: Syntax error.\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, syantex_issue_1289341) {
    char* lines = "class lkj\n";
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "Error: Syntax error.\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, syantex_issue_183571) {
    char* lines = "def test\n";
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "Error: Syntax error.\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, dump_issue_12l3kjioa) {
    char* lines =
        "if i = 1:\n"
        "    print('test')\n"
        "\n";
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, issue_fa13f4) {
    char* lines =
        "d = {'language':['c','python']}\n"
        "d['language'].append('Java')\n"
        "print(d)\n";
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "{'language': ['c', 'python', 'Java']}\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, callback_issue_I5L1MI) {
    char* lines =
        "def callback(func):\n"
        "    func()\n"
        "\n"
        "def test():\n"
        "    print('hello')\n"
        "\n"
        "f = test\n"
        "callback(f)\n";
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "hello\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, runchar_multiline) {
    char* lines =
        "def test():\r\n"
        "    print('hello')\r\n"
        "\r\n"
        "test()\r\n";
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "hello\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, callback_run_char) {
    char* lines =
        "def test2():\r\n"
        "    print('hello')\r\n"
        "\r\n"
        "def test():\r\n"
        "    print('hello')\r\n"
        "    print('in test')\r\n"
        "\r\n"
        "def test3():\r\n"
        "    print('hello3')\r\n"
        "\r\n"
        "test()\r\n";
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "in test\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_backspace) {
    char* lines = "print('test'\b\b')\r\n";
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "tes\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_backspace_issue_1) {
    char* lines = "print('test'\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b'a'\r\n";
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "'a'\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_backspace_issue_2) {
    char* lines = "loop\bp\n";
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_key_left) {
    char lines[] = {'1',  '2',      '+', '3',  '4', 0x1b,
                    0x5b, KEY_LEFT, '5', '\n', 0x00};
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "366\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_key_left_del) {
    char lines[] = {'1',  '2',      '+',  '3',  '4', 0x1b,
                    0x5b, KEY_LEFT, '\b', '\n', 0x00};
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "16\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SHELL_FILTER_ENABLE

TEST(pikaMain, SHELL_filter_hi_pika) {
    char lines[] = {"###Hi Pika"};
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "Yes, I am here\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, SHELL_filter_hi_pika_sence) {
    char lines[] = {"###hi pika"};
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], ">>> ");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, SHELL_filter_bye_pika_sence) {
    char lines[] = {"###Bye piKa"};
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "OK, see you\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, SHELL_filter_bye_pika_nomache) {
    char lines[] = {"print('###Hi Pika')\nprint('###Hi_Pika')\n"};
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "###Hi_Pika\r\n");
    EXPECT_STREQ(log_buff[4], "Yes, I am here\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, mem_now) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "from PikaStdLib import MemChecker as mem\n"
            "mem.now()\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif

TEST(pikaMain, obj_setNone) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_setNone(pikaMain, "n");
    obj_run(pikaMain, "print(n)");
    EXPECT_STREQ(log_buff[0], "None\r\n");
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, obj_setStr_NULL) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_setStr(pikaMain, "n", NULL);
    obj_run(pikaMain, "print(n)");
    EXPECT_STREQ(log_buff[2], "NameError: name 'n' is not defined\r\n");
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, _obj_set_str_null) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    obj_run(pikaMain,
            "class _test(GTestTask._test):\n"
            "    def __init__(self):\n"
            "        super().__init__()\n"
            "t = _test()\n");
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST_END