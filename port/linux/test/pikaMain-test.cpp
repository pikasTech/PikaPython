#include "gtest/gtest.h"
extern "C" {
#include "PikaMain.h"
#include "PikaStdLib_MemChecker.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "pikaScript.h"
}
extern PikaMemInfo pikaMemInfo;
TEST(pikaMain, init) {
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = pikaScriptInit();
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, new_mem) {
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    Parameters* globals =
        obj_runDirect(pikaMain, (char*)
        "mem = PikaStdLib.MemChecker()\n"
        "mem.max()\n"
        );
    PikaObj* mem = (PikaObj*)args_getPtr(globals->list, (char*)"mem");
    PikaObj* memClassPtr = (PikaObj*)obj_getPtr(mem, (char*)"_clsptr");
    EXPECT_EQ((void*)memClassPtr, (void*)New_PikaStdLib_MemChecker);
    obj_deinit(pikaMain);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, list_new) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    /* run */
    Parameters* globals =
        obj_runDirect(pikaMain, (char*)
        "list = PikaStdLib.List()\n"
        "list.init()\n"
        "list.append(7)\n"
        "list.append('eee')\n"
        "len = list.len()\n"
        "a1 = list.get(0)\n"
        "a2 = list.get(1)\n"
        );
    /* collect */
    int len = obj_getInt(globals, (char*)"len");
    int a1 = obj_getInt(globals, (char*)"a1");
    char* a2 = obj_getStr(globals, (char*)"a2");

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
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    /* run */
    Parameters* globals =
        obj_runDirect(pikaMain, (char*)
        "a = 1\n"
        "a_f = float(a)\n"
        "b = 1.3\n"
        "b_i = int(b)\n"
        );
    /* collect */
    int a = obj_getInt(globals, (char*)"a");
    float a_f = obj_getFloat(globals, (char*)"a_f");
    float b = obj_getFloat(globals, (char*)"b");
    int b_i = obj_getInt(globals, (char*)"b_i");

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
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    /* run */
    obj_runDirect(pikaMain, (char*)
        "type(1)\n"
        "b = 1.4\n"
        "type(b)\n"
        );
    /* collect */

    /* assert */

    /* deinit */
    // obj_deinit(globals);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, a_signed) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    /* run */
    obj_runDirect(pikaMain, (char*)"a = -1\n");
    /* collect */
    int a = obj_getInt(pikaMain, (char*)"a");

    /* assert */
    EXPECT_EQ(-1, a);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}


TEST(pikaMain, a_mm) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    /* run */
    obj_runDirect(pikaMain, (char*)"a = -1.1 ** 3\n");
    /* collect */
    float a = obj_getFloat(pikaMain, (char*)"a");

    /* assert */
    EXPECT_FLOAT_EQ(-1.331, a);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, a_n1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    /* run */
    obj_runDirect(pikaMain, (char*)"a = ~1\n");
    /* collect */
    int a = obj_getInt(pikaMain, (char*)"a");

    /* assert */
    EXPECT_EQ(-2, a);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}