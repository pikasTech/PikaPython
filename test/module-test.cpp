#include "test_common.h"
TEST_START

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
TEST(module, unittest_test1) {
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
TEST(module, unittest_test2) {
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

TEST(module, unittest_test3) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
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

TEST(socket, thread) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
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
    pikaMemInfo.heapUsedMax = 0;
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
                 "{\n\t\"code\":\t0,\n\t\"result\":\t{\n\t\t\"a_a\":\t{"
                 "\n\t\t\t\"desc\":\t\"A "
                 "\347\233\270\347\224\265\346\265\201\",\n\t\t\t\"value\":\t0."
                 "29\n\t\t}\n\t}\n}\r\n");
    EXPECT_STREQ(log_buff[4],
                 "client recv: "
                 "{\n\t\"code\":\t0,\n\t\"result\":\t{\n\t\t\"a_a\":\t{"
                 "\n\t\t\t\"desc\":\t\"A "
                 "\347\233\270\347\224\265\346\265\201\",\n\t\t\t\"value\":\t0."
                 "29\n\t\t}\n\t}\n}\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

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
    EXPECT_STREQ(log_buff[5], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[4],
                 "b'\\x01\\x03\\x00\\x00\\x00\\x0a\\xc5\\xcd'\r\n");
    EXPECT_STREQ(log_buff[3], "[0, 0, 1234, 0, 0, 123, 0, 0, 0, 0]\r\n");
    EXPECT_STREQ(log_buff[2],
                 "b'\\x01\\x04\\x00\\x00\\x00\\x02\\x71\\xcb'\r\n");
    EXPECT_STREQ(log_buff[1], "[0, 2278]\r\n");
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

TEST(issue, global) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
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
    pikaMemInfo.heapUsedMax = 0;
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
    pikaMemInfo.heapUsedMax = 0;
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
    return 0;
}
void pikaScriptShell_withGetchar(PikaObj* self, sh_getchar getchar_fn);
}

TEST(module, REPL_runbytecode) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
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

TEST(module, REPL_script) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
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
    EXPECT_STREQ(log_buff[4], "mem used max:\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(module, REPL_script_2) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
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
    pikaMemInfo.heapUsedMax = 0;
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
    pikaMemInfo.heapUsedMax = 0;
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
#endif

TEST_END