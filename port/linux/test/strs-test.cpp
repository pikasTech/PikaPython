#include "gtest/gtest.h"
extern "C" {
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"
}

static void printInfo(const char* argName, char* argVal) {
    printf("\t\t[info] %s: \"%s\"\r\n", argName, argVal);
}
static int mem;

TEST(strs, append) {
    Args* buffs = New_strBuff();
    char* res = strsAppend(buffs, (char*)"a", (char*)"b");
    EXPECT_STREQ((char*)"ab", res);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(strs, formatInt) {
    Args* buffs = New_strBuff();
    char* res = strsFormat(buffs, 32, "test: %d", 3);
    EXPECT_STREQ((char*)"test: 3", res);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(strs, analizeDef) {
    mem = pikaMemNow();
    Args* buffs = New_args(NULL);
    char currentClassName[] = "Compiler";
    char line[] = "    def analizeFile(pythonApiPath: str):";
    printInfo("currentClassName", currentClassName);
    char* defSentenceWithBlock =
        strsRemovePrefix(buffs, line, (char*)"    def ");
    char* defSentence = strsDeleteChar(buffs, defSentenceWithBlock, ' ');
    printInfo("defSentence", defSentence);
    char* methodName = strsGetFirstToken(buffs, defSentence, '(');
    printInfo("methodName", methodName);
    char* methodObjPath = strsAppend(
        buffs, strsAppend(buffs, currentClassName, (char*)"."), methodName);
    printInfo("methodObjPath", methodObjPath);
    char* returnType = strsCut(buffs, defSentence, '>', ':');
    printInfo("returnType", returnType);

    char* typeList = strsCut(buffs, defSentence, '(', ')');
    printInfo("typeList", typeList);
    if (0 != strGetSize(typeList)) {
        int argNum = strCountSign(typeList, ',') + 1;
        char* typeListBuff = strsCopy(buffs, typeList);
        for (int i = 0; i < argNum; i++) {
            char* typeDeclearation = strsPopToken(buffs, typeListBuff, ',');
            printInfo("typeDeclearation", typeDeclearation);
            char* argName = strsGetFirstToken(buffs, typeDeclearation, ':');
            printInfo("argName", argName);
            char* argType = strsGetLastToken(buffs, typeDeclearation, ':');
            printInfo("argType", argType);
        }
    }
    args_deinit(buffs);
    return;
}

TEST(strs, format) {
    Args* buffs = New_args(NULL);
    char* fmt = strsFormat(buffs, 128, "test int: %d, float %f", 1, 34.2);
    EXPECT_TRUE(strEqu((char*)"test int: 1, float 34.200000", fmt));
    args_deinit(buffs);
}

TEST(strs, mem) {
    EXPECT_EQ(pikaMemNow(), mem);
}

TEST(strs, arg_strAppend) {
    Arg* str_arg = arg_setStr(NULL, (char*)"", (char*)"a");
    str_arg = arg_strAppend(str_arg, (char*)"b");
    EXPECT_STREQ(arg_getStr(str_arg), (char*)"ab");
    arg_deinit(str_arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(strs, strsReplace) {
    Args* buffs = New_strBuff();
    char* res = strsReplace(buffs, (char*)"abcdefg", (char*)"cd", (char*)"47");
    EXPECT_STREQ(res, (char*)"ab47efg");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(strs, cut_){
    Args* buffs = New_strBuff();
    char* res = strsCut(buffs, (char*)"print('test,test')", '(', ')');
    EXPECT_STREQ(res, (char*)"'test,test'");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
