#include "test_common.h"
TEST_START
#if PIKA_SYNTAX_IMPORT_EX_ENABLE
TEST(module, cmodule_import) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    /* run */
    obj_run(pikaMain,
            "from TemplateDevice import qqee\n"
            "mytime = qqee()\n");
    /* collect */
    /* assert */
    /* deinit */
    EXPECT_STREQ("NameError: name 'TemplateDevice.qqee' is not defined\n",
                 log_buff[2]);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(module, while_loop) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
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
    g_PikaMemInfo.heapUsedMax = 0;
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
    g_PikaMemInfo.heapUsedMax = 0;
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
    g_PikaMemInfo.heapUsedMax = 0;
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
    g_PikaMemInfo.heapUsedMax = 0;
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
    g_PikaMemInfo.heapUsedMax = 0;
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
TEST(module, unittest_test1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
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
TEST(module, unittest_test2) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
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

TEST(module, unittest_test3) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/unittest/test3.py");
    /* collect */
    int testsRun = obj_getInt(pikaMain, "res.testsRun");
    int errorsNum = obj_getInt(pikaMain, "res.errorsNum");
    /* assert */
    EXPECT_EQ(testsRun, 1);
    EXPECT_EQ(errorsNum, 0);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif

TEST(socket, gethostname) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
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
    g_PikaMemInfo.heapUsedMax = 0;
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

TEST(socket, thread) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/socket/socket_thread.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[4], "socket server accepted at 127.0.0.1\r\n");
    EXPECT_STREQ(log_buff[3], "socket server recv: hello\r\n");
    EXPECT_STREQ(log_buff[2], "client recv: hello\r\n");
    EXPECT_STREQ(log_buff[1], "socket server closing accept\r\n");
    EXPECT_STREQ(log_buff[0], "socket server closing\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_FLOAT_TYPE_DOUBLE
TEST(socket, json_issue) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/socket/socket_json.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[2],
                 "client recv: "
                 "{\n\t\"result\":\t{\n\t\t\"a_a\":\t{\n\t\t\t\"value\":\t0.29,"
                 "\n\t\t\t\"desc\":\t\"A "
                 "\347\233\270\347\224\265\346\265\201\"\n\t\t}\n\t},"
                 "\n\t\"code\":\t0\n}\r\n");
    EXPECT_STREQ(log_buff[4],
                 "client recv: "
                 "{\n\t\"result\":\t{\n\t\t\"a_a\":\t{\n\t\t\t\"value\":\t0.29,"
                 "\n\t\t\t\"desc\":\t\"A "
                 "\347\233\270\347\224\265\346\265\201\"\n\t\t}\n\t},"
                 "\n\t\"code\":\t0\n}\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST_RUN_SINGLE_FILE_ASSERT(socket,
                            socket_GET,
                            "test/python/socket/socket_GET.py",
                            obj_getBool(pikaMain, "res") == pika_true)

TEST_RUN_SINGLE_FILE_PASS(socket,
                          socket_DNS,
                          "test/python/socket/socket_DNS.py")

#endif

#if !PIKA_NANO_ENABLE
TEST(re, match) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
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
    g_PikaMemInfo.heapUsedMax = 0;
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
    g_PikaMemInfo.heapUsedMax = 0;
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
    g_PikaMemInfo.heapUsedMax = 0;
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
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/modbus/rtu_master.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[6], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[5],
                 "b'\\x01\\x03\\x00\\x00\\x00\\x0a\\xc5\\xcd'\r\n");
    EXPECT_STREQ(log_buff[4], "[0, 0, 1234, 0, 0, 123, 0, 0, 0, 0]\r\n");
    EXPECT_STREQ(log_buff[3],
                 "b'\\x01\\x04\\x00\\x00\\x00\\x02\\x71\\xcb'\r\n");
    EXPECT_STREQ(log_buff[2], "[0, 2278]\r\n");
    EXPECT_STREQ(log_buff[1],
                 "b'\\x01\\x06\\x00\\x00\\x12\\x34\\x84\\xbd'\r\n");
    EXPECT_STREQ(log_buff[0],
                 "b'\\x01\\x0f\\x00\\x00\\x00\\x08\\x01\\x57\\xbf\\x6b'\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(proxy, test1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
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

TEST(issue, global) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/issue/issue_global.py");
    /* collect */
    /* assert */
    EXPECT_EQ(obj_getInt(pikaMain, "testpara.timer2_flag"), 1);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(module, mod1_mod2_mod1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "import test_module1\n"
            "test_module1.test_module2.test_module1.mytest()");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "test_module_1_hello\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(module, improt_as_cmodule) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "from PikaStdData import String as string\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

extern "C" {
volatile FILE* f_getchar_fp = NULL;
char f_getchar(void) {
    char c = 0;
    size_t n = fread(&c, 1, 1, (FILE*)f_getchar_fp);
    if (n > 0) {
        return c;
    }
    pika_platform_printf("f_getchar error\r\n");
    pika_assert(0);
    return -1;
}
void pikaScriptShell_withGetchar(PikaObj* self, sh_getchar getchar_fn);
}

TEST(module, REPL_runbytecode) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    f_getchar_fp = fopen("package/pikascript/cjson_test.py.o", "rb");
    pikaScriptShell_withGetchar(pikaMain, f_getchar);
    fclose((FILE*)f_getchar_fp);
    obj_run(pikaMain, "test_start()");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "shopping\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(module, REPL_pya) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    f_getchar_fp =
        fopen("package/pikascript/pikascript-api/pikaModules.py.a", "rb");
    pikaScriptShell_withGetchar(pikaMain, f_getchar);
    fclose((FILE*)f_getchar_fp);
    // obj_run(pikaMain, "test_start()");
    /* collect */
    /* assert */
    // EXPECT_STREQ(log_buff[0], "shopping\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(module, REPL_script) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    f_getchar_fp = fopen("test/python/UnitTest.py", "rb");
    pikaScriptShell_withGetchar(pikaMain, f_getchar);
    fclose((FILE*)f_getchar_fp);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[3], "mem used max:\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(module, REPL_script_2) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    f_getchar_fp = fopen("test/python/UnitTest2.py", "rb");
    pikaScriptShell_withGetchar(pikaMain, f_getchar);
    fclose((FILE*)f_getchar_fp);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "mem used max:\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(module, REPL_big_script) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    f_getchar_fp = fopen("package/pikascript/cjson_test.py", "rb");
    pikaScriptShell_withGetchar(pikaMain, f_getchar);
    fclose((FILE*)f_getchar_fp);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0],
                 "\r\nError: line buff overflow, please use bigger "
                 "'PIKA_LINE_BUFF_SIZE'\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(module, REPL_stdtask) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    f_getchar_fp = fopen("test/python/std/stdtask.py", "rb");
    pikaScriptShell_withGetchar(pikaMain, f_getchar);
    fclose((FILE*)f_getchar_fp);
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST_RUN_SINGLE_FILE(modbus,
                     rtu_master_err,
                     "test/python/modbus/rtu_master_err.py")

TEST_RUN_SINGLE_FILE_PASS(proxy, proxy3, "test/python/proxy/proxy3.py")

#if PIKA_FLOAT_TYPE_DOUBLE
TEST_RUN_SINGLE_FILE_PASS(struct, pack, "test/python/struct/pack.py")
TEST_RUN_SINGLE_FILE_PASS(struct, unpack, "test/python/struct/unpack.py")
#endif

extern "C" {
#define PIKA_USING_FLASHDB 1
#if PIKA_USING_FLASHDB
// #include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "flashdb.h"
#define FDB_LOG_TAG "[main]"
// static pthread_mutex_t kv_locker, ts_locker;
static uint32_t boot_count = 0;
static time_t boot_time[10] = {0, 1, 2, 3};
/* default KV nodes */
static struct fdb_default_kv_node default_kv_table[] = {
    {"username", (void*)"armink", 0},                /* string KV */
    {"password", (void*)"123456", 0},                /* string KV */
    {"boot_count", &boot_count, sizeof(boot_count)}, /* int type KV */
    {"boot_time", &boot_time, sizeof(boot_time)},    /* int array type KV */
};
/* KVDB object */
static struct fdb_kvdb kvdb = {0};
/* TSDB object */
struct fdb_tsdb tsdb = {0};
/* counts for simulated timestamp */
static int counts = 0;

extern void kvdb_basic_sample(fdb_kvdb_t kvdb);
extern void kvdb_type_string_sample(fdb_kvdb_t kvdb);
extern void kvdb_type_blob_sample(fdb_kvdb_t kvdb);
extern void tsdb_sample(fdb_tsdb_t tsdb);

/*
static void lock(fdb_db_t db)
{
    pthread_mutex_lock((pthread_mutex_t *)db->user_data);
}

static void unlock(fdb_db_t db)
{
    pthread_mutex_unlock((pthread_mutex_t *)db->user_data);
}
*/
static fdb_time_t get_time(void) {
    return time(NULL);
}
#endif

int test_flashdb(void) {
#if PIKA_USING_FLASHDB
    __platform_printf(" FDB_LOG_TAG :%s \r\n", FDB_LOG_TAG);
    fdb_err_t result;
    bool file_mode = true;
    uint32_t sec_size = 4096, db_size = sec_size * 4;
#undef FDB_USING_KVDB
#define FDB_USING_KVDB 1
#ifdef FDB_USING_KVDB
    { /* KVDB Sample */
        struct fdb_default_kv default_kv;

        default_kv.kvs = default_kv_table;
        default_kv.num = sizeof(default_kv_table) / sizeof(default_kv_table[0]);
        /* set the lock and unlock function if you want */
        //      pthread_mutex_init(&kv_locker, NULL);
        //      fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_LOCK, (void *)lock);
        //      fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_UNLOCK, (void
        //      *)unlock);
        /* set the sector and database max size */
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_SEC_SIZE, &sec_size);
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_MAX_SIZE, &db_size);
        /* enable file mode */
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_FILE_MODE, &file_mode);
        /* create database directory */
        mkdir("test/out/fdb_kvdb1", 0777);
        /* Key-Value database initialization
         *
         *       &kvdb: database object
         *       "env": database name
         * "test/out/fdb_kvdb1": The flash partition name base on FAL. Please
         * make sure it's in FAL partition table. Please change to YOUR
         * partition name. &default_kv: The default KV nodes. It will auto add
         * to KVDB when first initialize successfully. &kv_locker: The locker
         * object.
         */
        // result = fdb_kvdb_init(&kvdb, "env", "test/out/fdb_kvdb1",
        // &default_kv, &kv_locker);
        result = fdb_kvdb_init(&kvdb, "env", "test/out/fdb_kvdb1", &default_kv,
                               NULL);

        if (result != FDB_NO_ERR) {
            return -1;
        }

        /* run basic KV samples */
        kvdb_basic_sample(&kvdb);
        /* run string KV samples */
        kvdb_type_string_sample(&kvdb);
        /* run blob KV samples */
        kvdb_type_blob_sample(&kvdb);
        fdb_kvdb_deinit(&kvdb);
    }
#endif /* FDB_USING_KVDB */

// #define FDB_USING_TSDB 1
#ifdef FDB_USING_TSDB
    { /* TSDB Sample */
        /* set the lock and unlock function if you want */
        //   pthread_mutex_init(&ts_locker, NULL);
        //    fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_LOCK, (void *)lock);
        //  fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_UNLOCK, (void *)unlock);
        /* set the sector and database max size */
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_SEC_SIZE, &sec_size);
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_MAX_SIZE, &db_size);
        /* enable file mode */
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_FILE_MODE, &file_mode);
        /* create database directory */
        mkdir("test/out/fdb_tsdb1", 0777);
        /* Time series database initialization
         *
         *       &tsdb: database object
         *       "log": database name
         * "test/out/fdb_tsdb1": The flash partition name base on FAL. Please
         * make sure it's in FAL partition table. Please change to YOUR
         * partition name. get_time: The get current timestamp function. 128:
         * maximum length of each log ts_locker: The locker object.
         */
        // result = fdb_tsdb_init(&tsdb, "log", "test/out/fdb_tsdb1", get_time,
        // 128, &ts_locker);
        result = fdb_tsdb_init(&tsdb, "log", "test/out/fdb_tsdb1", get_time,
                               128, NULL);

        /* read last saved time for simulated timestamp */
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_GET_LAST_TIME, &counts);

        if (result != FDB_NO_ERR) {
            return -1;
        }

        /* run TSDB sample */
        tsdb_sample(&tsdb);

        /* close TSDB */
        fdb_tsdb_deinit(&tsdb);
    }
#endif /* FDB_USING_TSDB */
#else
    __platform_printf(" FDB NOT INUSE \r\n");
#endif
    return 0;
}
}

TEST(flashdb, base) {
    EXPECT_EQ(test_flashdb(), 0);
}

TEST_RUN_SINGLE_FILE_EXCEPT_OUTPUT(flashdb,
                                   kvdb2,
                                   "test/python/flashdb/flashdb_kvdb2.py",
                                   "PASS\r\n")

TEST_RUN_SINGLE_FILE_EXCEPT_OUTPUT(flashdb,
                                   kvdb1,
                                   "test/python/flashdb/flashdb_kvdb1.py",
                                   "PASS\r\n")

#endif

TEST(fuzzypid, base) {
#define DOF 6
    // Default fuzzy rule base of delta kp/ki/kd
    int rule_base[][qf_default] = {// delta kp rule base
                                   {PB, PB, PM, PM, PS, ZO, ZO},
                                   {PB, PB, PM, PS, PS, ZO, NS},
                                   {PM, PM, PM, PS, ZO, NS, NS},
                                   {PM, PM, PS, ZO, NS, NM, NM},
                                   {PS, PS, ZO, NS, NS, NM, NM},
                                   {PS, ZO, NS, NM, NM, NM, NB},
                                   {ZO, ZO, NM, NM, NM, NB, NB},
                                   // delta ki rule base
                                   {NB, NB, NM, NM, NS, ZO, ZO},
                                   {NB, NB, NM, NS, NS, ZO, ZO},
                                   {NB, NM, NS, NS, ZO, PS, PS},
                                   {NM, NM, NS, ZO, PS, PM, PM},
                                   {NM, NS, ZO, PS, PS, PM, PB},
                                   {ZO, ZO, PS, PS, PM, PB, PB},
                                   {ZO, ZO, PS, PM, PM, PB, PB},
                                   // delta kd rule base
                                   {PS, NS, NB, NB, NB, NM, PS},
                                   {PS, NS, NB, NM, NM, NS, ZO},
                                   {ZO, NS, NM, NM, NS, NS, ZO},
                                   {ZO, NS, NS, NS, NS, NS, ZO},
                                   {ZO, ZO, ZO, ZO, ZO, ZO, ZO},
                                   {PB, PS, PS, PS, PS, PS, PB},
                                   {PB, PM, PM, PM, PS, PS, PB}};

    // Default parameters of membership function
    int mf_params[4 * qf_default] = {-3, -3, -2, 0, -3, -2, -1, 0, -2, -1,
                                     0,  0,  -1, 0, 1,  0,  0,  1, 2,  0,
                                     1,  2,  3,  0, 2,  3,  3,  0};

    // Default parameters of pid controller
    float fuzzy_pid_params[DOF][pid_params_count] = {
        {0.65f, 0, 0, 0, 0, 0, 1}, {-0.34f, 0, 0, 0, 0, 0, 1},
        {-1.1f, 0, 0, 0, 0, 0, 1}, {-2.4f, 0, 0, 0, 0, 0, 1},
        {1.2f, 0, 0, 0, 0, 0, 1},  {1.2f, 0.05f, 0.1f, 0, 0, 0, 1}};

    // Obtain the PID controller vector according to the parameters
    struct PID** pid_vector = fuzzy_pid_vector_init(
        fuzzy_pid_params, 2.0f, 4, 1, 0, mf_params, rule_base, DOF);

    printf("output:\n");
    int control_id = 5;
    float real = 0;
    float idea = max_error * 0.9f;
    printf("idea value: %f\n", idea);
    bool direct[DOF] = {true, false, false, false, true, true};
    for (int j = 0; j < 10; ++j) {
        int out = fuzzy_pid_motor_pwd_output(real, idea, direct[control_id],
                                             pid_vector[control_id]);
        real += (float)(out - middle_pwm_output) / (float)middle_pwm_output *
                (float)max_error * 0.1f;
        printf("%d,%f\n", out, real);
    }

    delete_pid_vector(pid_vector, DOF);
}

TEST_RUN_SINGLE_FILE(fuzzypid, fuzzypid1, "test/python/fuzzypid/fuzzypid1.py");

TEST_RUN_SINGLE_FILE(lvgl, base, "test/python/pika_lvgl/base.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_arc1, "test/python/pika_lvgl/lv_arc1.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_arc2, "test/python/pika_lvgl/lv_arc2.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_bar1, "test/python/pika_lvgl/lv_bar1.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_btn1, "test/python/pika_lvgl/lv_btn1.py");
TEST_RUN_SINGLE_FILE(lvgl,
                     lv_callback1,
                     "test/python/pika_lvgl/lv_callback1.py");
TEST_RUN_SINGLE_FILE(lvgl,
                     lv_checkbox,
                     "test/python/pika_lvgl/lv_checkbox1.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_drag, "test/python/pika_lvgl/lv_drag.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_label1, "test/python/pika_lvgl/lv_label1.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_list1, "test/python/pika_lvgl/lv_list1.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_obj1, "test/python/pika_lvgl/lv_obj1.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_obj2, "test/python/pika_lvgl/lv_obj2.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_obj3, "test/python/pika_lvgl/lv_obj3.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_png, "test/python/pika_lvgl/lv_png.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_roller1, "test/python/pika_lvgl/lv_roller1.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_slider1, "test/python/pika_lvgl/lv_slider1.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_style1, "test/python/pika_lvgl/lv_style1.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_switch1, "test/python/pika_lvgl/lv_switch1.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_table1, "test/python/pika_lvgl/lv_table1.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_tabview1, "test/python/pika_lvgl/lv_tabview1.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_tabview2, "test/python/pika_lvgl/lv_tabview2.py");
TEST_RUN_SINGLE_FILE(lvgl,
                     lv_textarea1,
                     "test/python/pika_lvgl/lv_textarea1.py");
TEST_RUN_SINGLE_FILE(lvgl, lv_tim, "test/python/pika_lvgl/lv_tim.py");

TEST_END
