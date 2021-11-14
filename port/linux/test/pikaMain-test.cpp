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
    obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}