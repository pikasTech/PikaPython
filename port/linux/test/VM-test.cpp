#include "gtest/gtest.h"
extern "C" {
#include "BaseObj.h"
#include "PikaMath_Operator.h"
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

TEST(VM, a_1) {
    char* line = (char*)"a = 1";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    pikaVM_runAsm(self, pikaAsm);

    ASSERT_EQ(obj_getInt(self, (char*)"a"), 1);

    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_1d1) {
    char* line = (char*)"a = 1.1";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    pikaVM_runAsm(self, pikaAsm);

    ASSERT_FLOAT_EQ(obj_getFloat(self, (char*)"a"), 1.1);

    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, str_xy) {
    char* line = (char*)"a = 'xy'";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    pikaVM_runAsm(self, pikaAsm);

    ASSERT_STREQ(obj_getStr(self, (char*)"a"), (char*)"xy");

    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, str_xy_space) {
    char* line = (char*)"a = 'xy '";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    pikaVM_runAsm(self, pikaAsm);

    ASSERT_STREQ(obj_getStr(self, (char*)"a"), (char*)"xy ");

    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, ref_a_b) {
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    Args* buffs = New_strBuff();

    pikaVM_runAsm(self, pikaParseToAsm(buffs, (char*)"a = 'xy '"));
    pikaVM_runAsm(self, pikaParseToAsm(buffs, (char*)"b = a"));

    args_deinit(buffs);
    ASSERT_STREQ(obj_getStr(self, (char*)"b"), (char*)"xy ");
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, Run_add) {
    PikaObj* self = newRootObj((char*)"root", New_PikaMath_Operator);
    Args* buffs = New_strBuff();

    pikaVM_runAsm(self, pikaParseToAsm(buffs, (char*)"a = plusInt(1,2)"));

    args_deinit(buffs);
    int a = obj_getInt(self, (char*)"a");
    ASSERT_EQ(a, 3);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, Run_add_multy) {
    PikaObj* self = newRootObj((char*)"root", New_PikaMath_Operator);
    Args* buffs = New_strBuff();

    pikaVM_runAsm(self, pikaParseToAsm(buffs, (char*)"b = 2"));
    pikaVM_runAsm(self, pikaParseToAsm(buffs, (char*)"a = plusInt(1,b)"));

    args_deinit(buffs);
    int a = obj_getInt(self, (char*)"a");
    ASSERT_EQ(a, 3);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, Run_add_1_2_3) {
    PikaObj* self = newRootObj((char*)"root", New_PikaMath_Operator);
    Args* buffs = New_strBuff();

    pikaVM_runAsm(
        self, pikaParseToAsm(buffs, (char*)"a = plusInt(1, plusInt(2,3) )"));

    args_deinit(buffs);
    int a = obj_getInt(self, (char*)"a");
    ASSERT_EQ(a, 6);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}