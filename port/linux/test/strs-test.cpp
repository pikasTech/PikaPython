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
    char* res = strsAppend(buffs, "a", "b");
    EXPECT_STREQ("ab", res);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(strs, formatInt) {
    Args* buffs = New_strBuff();
    char* res = strsFormat(buffs, 32, "test: %d", 3);
    EXPECT_STREQ("test: 3", res);
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
        strsRemovePrefix(buffs, line, "    def ");
    char* defSentence = strsDeleteChar(buffs, defSentenceWithBlock, ' ');
    printInfo("defSentence", defSentence);
    char* methodName = strsGetFirstToken(buffs, defSentence, '(');
    printInfo("methodName", methodName);
    char* methodObjPath = strsAppend(
        buffs, strsAppend(buffs, currentClassName, "."), methodName);
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
            char* argType = strPointToLastToken( typeDeclearation, ':');
            printInfo("argType", argType);
        }
    }
    args_deinit(buffs);
    return;
}

TEST(strs, format) {
    Args* buffs = New_args(NULL);
    char* fmt = strsFormat(buffs, 128, "test int: %d, float %f", 1, 34.2);
    EXPECT_TRUE(strEqu("test int: 1, float 34.200000", fmt));
    args_deinit(buffs);
}

TEST(strs, mem) {
    EXPECT_EQ(pikaMemNow(), mem);
}

TEST(strs, arg_strAppend) {
    Arg* str_arg = arg_setStr(NULL, "", "a");
    str_arg = arg_strAppend(str_arg, "b");
    EXPECT_STREQ(arg_getStr(str_arg), "ab");
    arg_deinit(str_arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(strs, strsReplace) {
    Args* buffs = New_strBuff();
    char* res = strsReplace(buffs, "abcdefg", "cd", "47");
    EXPECT_STREQ(res, "ab47efg");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(strs, cut_) {
    Args* buffs = New_strBuff();
    char* res = strsCut(buffs, "print('test,test')", '(', ')');
    EXPECT_STREQ(res, "'test,test'");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(str, strPointToLastToken) {
    char* tokens = "abc.efg";
    char* last_token = strPointToLastToken(tokens, '.');
    EXPECT_STREQ(last_token, "efg");
}
