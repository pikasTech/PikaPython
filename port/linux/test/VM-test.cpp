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
    char* pikaAsm = pikaParseLineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", PikaStdLib_SysObj);
    args_deinit(pikaVM_runAsm(self, pikaAsm));
    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_1) {
    char* line = (char*)"a = 1";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseLineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", PikaStdLib_SysObj);
    args_deinit(pikaVM_runAsm(self, pikaAsm));

    ASSERT_EQ(obj_getInt(self, (char*)"a"), 1);

    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_1d1) {
    char* line = (char*)"a = 1.1";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseLineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", PikaStdLib_SysObj);
    args_deinit(pikaVM_runAsm(self, pikaAsm));

    ASSERT_FLOAT_EQ(obj_getFloat(self, (char*)"a"), 1.1);

    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, str_xy) {
    char* line = (char*)"a = 'xy'";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseLineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", PikaStdLib_SysObj);
    args_deinit(pikaVM_runAsm(self, pikaAsm));

    ASSERT_STREQ(obj_getStr(self, (char*)"a"), (char*)"xy");

    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, str_xy_space) {
    char* line = (char*)"a = 'xy '";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseLineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", PikaStdLib_SysObj);
    args_deinit(pikaVM_runAsm(self, pikaAsm));

    ASSERT_STREQ(obj_getStr(self, (char*)"a"), (char*)"xy ");

    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, ref_a_b) {
    PikaObj* self = newRootObj((char*)"root", PikaStdLib_SysObj);
    Args* buffs = New_strBuff();

    args_deinit(pikaVM_runAsm(
        self, pikaParseLineToAsm(buffs, (char*)"a = 'xy '", NULL)));
    args_deinit(
        pikaVM_runAsm(self, pikaParseLineToAsm(buffs, (char*)"b = a", NULL)));

    args_deinit(buffs);
    ASSERT_STREQ(obj_getStr(self, (char*)"b"), (char*)"xy ");
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, Run_add) {
    PikaObj* self = newRootObj((char*)"root", PikaMath_Operator);
    Args* buffs = New_strBuff();

    args_deinit(pikaVM_runAsm(
        self, pikaParseLineToAsm(buffs, (char*)"a = plusInt(1,2)", NULL)));

    args_deinit(buffs);
    int a = obj_getInt(self, (char*)"a");
    ASSERT_EQ(a, 3);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, Run_add_multy) {
    PikaObj* self = newRootObj((char*)"root", PikaMath_Operator);
    Args* buffs = New_strBuff();

    args_deinit(
        pikaVM_runAsm(self, pikaParseLineToAsm(buffs, (char*)"b = 2", NULL)));
    args_deinit(pikaVM_runAsm(
        self, pikaParseLineToAsm(buffs, (char*)"a = plusInt(1,b)", NULL)));

    args_deinit(buffs);
    int a = obj_getInt(self, (char*)"a");
    ASSERT_EQ(a, 3);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, Run_add_1_2_3) {
    PikaObj* self = newRootObj((char*)"root", PikaMath_Operator);
    Args* buffs = New_strBuff();

    args_deinit(pikaVM_runAsm(
        self, pikaParseLineToAsm(buffs, (char*)"a = plusInt(1, plusInt(2,3) )",
                                 NULL)));

    args_deinit(buffs);
    int a = obj_getInt(self, (char*)"a");
    ASSERT_EQ(a, 6);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, JEZ) {
    char* pikaAsm = (char*)
    "B0\n"
    "0 REF False\n"
    "0 JEZ 2\n"
    "B0\n"
    "B0\n";
    PikaObj* self = TinyObj(NULL);
    int lineAddr = 0;
    int size = strGetSize(pikaAsm);
    Args* sysRes = New_args(NULL);
    args_setErrorCode(sysRes, 0);
    args_setSysOut(sysRes, (char*)"");
    lineAddr = pikaVM_runAsmLine(self, pikaAsm, lineAddr, sysRes);
    lineAddr = pikaVM_runAsmLine(self, pikaAsm, lineAddr, sysRes);
    lineAddr = pikaVM_runAsmLine(self, pikaAsm, lineAddr, sysRes);
    __clearInvokeQueues(self);
    obj_deinit(self);
    args_deinit(sysRes);
    EXPECT_EQ(lineAddr, 26);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, JMP) {
    char* pikaAsm = (char*)
    "B0\n"
    "0 JMP 2\n"
    "B0\n"
    "B0\n";
    PikaObj* self = TinyObj(NULL);
    int lineAddr = 0;
    int size = strGetSize(pikaAsm);
    Args* sysRes = New_args(NULL);
    args_setErrorCode(sysRes, 0);
    args_setSysOut(sysRes, (char*)"");
    lineAddr = pikaVM_runAsmLine(self, pikaAsm, lineAddr, sysRes);
    lineAddr = pikaVM_runAsmLine(self, pikaAsm, lineAddr, sysRes);
    __clearInvokeQueues(self);
    obj_deinit(self);
    args_deinit(sysRes);
    EXPECT_EQ(lineAddr, 14);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, JMP_back1) {
    char* pikaAsm = (char*)
    "B0\n"
    "B0\n"
    "0 JMP -1\n"
    "B0\n"
    "B0\n";
    PikaObj* self = TinyObj(NULL);
    int lineAddr = 0;
    int size = strGetSize(pikaAsm);
    Args* sysRes = New_args(NULL);
    args_setErrorCode(sysRes, 0);
    args_setSysOut(sysRes, (char*)"");
    lineAddr = pikaVM_runAsmLine(self, pikaAsm, lineAddr, sysRes);
    lineAddr = pikaVM_runAsmLine(self, pikaAsm, lineAddr, sysRes);
    lineAddr = pikaVM_runAsmLine(self, pikaAsm, lineAddr, sysRes);
    __clearInvokeQueues(self);
    obj_deinit(self);
    args_deinit(sysRes);
    EXPECT_EQ(lineAddr, 0);
    EXPECT_EQ(pikaMemNow(), 0);
}

extern PikaMemInfo pikaMemInfo;
TEST(VM, WHILE) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =(char *)
        "a = 1\n"
        "b = 0\n"
        "while a:\n"
        "    b = 1\n"
        "    a = 0\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* self = TinyObj(NULL);
    Args* runRes = pikaVM_runAsm(self, pikaAsm);
    EXPECT_EQ(obj_getInt(self, (char*)"a"), 0);
    EXPECT_EQ(obj_getInt(self, (char*)"b"), 1);
    args_deinit(runRes);
    args_deinit(buffs);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
