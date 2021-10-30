// #include "gtest/gtest.h"
// extern "C" {
// #include "dataMemory.h"
// #include "pikaScript.h"
// }
// extern PikaMemInfo pikaMemInfo;
// TEST(pikaMain, init) {
//     pikaMemInfo.heapUsedMax = 0;
//     PikaObj* pikaMain = pikaScriptInit();
//     obj_deinit(pikaMain);
//     EXPECT_EQ(pikaMemNow(), 0);
// }