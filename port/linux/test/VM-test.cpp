#include "gtest/gtest.h"
extern "C" {
#include "BaseObj.h"
#include "PikaParser.h"
#include "PikaStdLib_SysObj.h"
#include "PikaVM.h"
#include "dataMemory.h"
#include "dataQueue.h"
#include "dataStrs.h"
}

TEST(VM, num1) {
    char* line = (char*)"1";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    pikaVM_runAsm(self, pikaAsm);
    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}


// TEST(VM, a_1) {
//     char* line = (char*)"a = 1";
//     Args* buffs = New_strBuff();
//     char* pikaAsm = pikaParseToAsm(buffs, line);
//     printf("%s", pikaAsm);
//     PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
//     pikaVM_runAsm(self, pikaAsm);
//     obj_deinit(self);
//     args_deinit(buffs);
//     EXPECT_EQ(pikaMemNow(), 0);
// }