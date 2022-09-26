#include "test_common.h"

#if PIKA_SYNTAX_IMPORT_EX_ENABLE
TEST(module, cmodule_import) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain,
            "from TemplateDevice import qqee\n"
            "mytime = qqee()\n");
    /* collect */
    /* assert */
    /* deinit */
    EXPECT_STREQ("NameError: name 'TemplateDevice.qqee' is not defined\r\n",
                 log_buff[2]);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(module, while_loop) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain,
            "import test_module1\n"
            "test_module1.while_loop()\n");
    /* collect */
    /* assert */
    EXPECT_STREQ("4\r\n", log_buff[0]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(module, for_loop) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain,
            "import test_module1\n"
            "test_module1.for_loop()\n");
    /* collect */
    /* assert */
    EXPECT_STREQ("9\r\n", log_buff[0]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(module, script) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain, "import test_module4\n");
    /* collect */
    /* assert */
    EXPECT_STREQ("imported test moulde4\r\n", log_buff[0]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(module, __init__) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain, "import GTestTask\n");
    /* collect */
    /* assert */
    EXPECT_EQ(obj_getInt(pikaMain, "GTestTask.testval"), 8848);
    EXPECT_STREQ(log_buff[0], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_IMPORT_EX_ENABLE
TEST(module, __init__2) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain, "import GTestTask as task\n");
    /* collect */
    /* assert */
    EXPECT_EQ(obj_getInt(pikaMain, "task.testval"), 8848);
    EXPECT_STREQ(log_buff[0], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_IMPORT_EX_ENABLE
TEST(module, import_as_issue1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain,
            "import import_test as my_import\n"
            "print(my_import.func())\n"
            "print(import_test.func())\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "hello\r\n");
    EXPECT_STREQ(log_buff[1], "hello\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MAXIMAL
TEST(unittest, test1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/unittest/test1.py");
    /* collect */
    int testsRun = obj_getInt(pikaMain, "res.testsRun");
    int errorsNum = obj_getInt(pikaMain, "res.errorsNum");
    /* assert */
    EXPECT_EQ(testsRun, 3);
    EXPECT_EQ(errorsNum, 0);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MAXIMAL
TEST(unittest, test2) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/unittest/test2.py");
    /* collect */
    int testsRun = obj_getInt(pikaMain, "res.testsRun");
    int errorsNum = obj_getInt(pikaMain, "res.errorsNum");
    /* assert */
    EXPECT_EQ(testsRun, 4);
    EXPECT_EQ(errorsNum, 1);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(socket, gethostname) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/socket/gethostname.py");
    /* collect */
    /* assert */
    EXPECT_EQ(ARG_TYPE_STRING, args_getType(pikaMain->list, "hostname"));
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(socket, server_client) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/socket/server_client.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[2], "recv from client: 127.0.0.1\r\n");
    EXPECT_STREQ(log_buff[1], "server recv: send test from client\r\n");
    EXPECT_STREQ(log_buff[0], "client recv: send test from server\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(re, match) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/re/match.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[3], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[2],
                 "matchObj.group(0) :  Cats are smarter than dogs\r\n");
    EXPECT_STREQ(log_buff[1], "matchObj.group(1) :  Cats\r\n");
    EXPECT_STREQ(log_buff[0], "matchObj.group(2) :  smarter\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(re, search) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/re/search.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], "(0, 3)\r\n");
    EXPECT_STREQ(log_buff[0], "(11, 14)\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(re, sub) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/re/sub.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], "the phone number is:  2004-959-559 \r\n");
    EXPECT_STREQ(log_buff[0], "the phone number is:  2004959559\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(re, findall) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/re/findall.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(
        log_buff[1],
        "[('2020', '1', '1'), ('2022', '12', '22'), ('2018', '3', '31')]\r\n");
    EXPECT_STREQ(
        log_buff[0],
        "date: 2020, 2022, 2018. Wrong format: 2031-13-31, 2032-12-33 ...\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE

TEST(modbus, rtu_master) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/modbus/rtu_master.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[3], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[2],
                 "b'\\x01\\x03\\x00\\x00\\x00\\x0a\\xc5\\xcd'\r\n");
    EXPECT_STREQ(log_buff[1], "[0, 0, 1234, 0, 0, 123, 0, 0, 0, 0]\r\n");
    EXPECT_STREQ(log_buff[0],
                 "b'\\x01\\x06\\x00\\x00\\x12\\x34\\x84\\xbd'\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}


TEST(proxy, test1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/proxy/proxy1.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "set value = 20\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif