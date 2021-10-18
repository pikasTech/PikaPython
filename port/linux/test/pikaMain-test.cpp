#include "gtest/gtest.h"
extern "C" {
#include "pikaScript.h"
}

TEST(pikaMain, init) {
    PikaObj* pikaMain = pikaScriptInit();
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}