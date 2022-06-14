#include "test_common.h"

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
                 log_buff[5]);
    EXPECT_STREQ("NameError: name 'qqee' is not defined\r\n", log_buff[2]);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

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
