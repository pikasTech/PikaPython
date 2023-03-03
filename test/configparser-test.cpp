#include "test_common.h"
TEST_START
#if PIKA_SYNTAX_SLICE_ENABLE 
TEST(configparser, test1) {
    char* s =
        "[DEFAULT]\n"
        "ServerAliveInterval = 45\n"
        "Compression = yes\n"
        "CompressionLevel = 9\n"
        "ForwardX11 = yes\n"
        "\n"
        "[bitbucket.org]\n"
        "User = hg\n"
        "\n"
        "[topsecret.server.com]\n"
        "Port = 50022\n"
        "ForwardX11 = no\n";

    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    obj_setStr(pikaMain, "s", s);
    obj_run(pikaMain,
            "import configparser\n"
            "config = configparser.ConfigParser()\n"
            "config.read_string(s)\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif


#if PIKA_SYNTAX_SLICE_ENABLE
TEST(configparser, test2) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    pikaVM_runSingleFile(pikaMain,"test/python/configparser/test2.py");
    /* collect */
    char* font_color = obj_getStr(pikaMain, "font_color");
    /* assert */
    EXPECT_STREQ("#000000", font_color);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif
TEST_END