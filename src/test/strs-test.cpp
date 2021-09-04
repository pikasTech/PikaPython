#include "gtest/gtest.h"
extern "C"
{
#include "dataStrs.h"
#include "dataString.h"
#include "dataArgs.h"
#include "dataMemory.h"
}

static void printInfo(const char *argName, char *argVal)
{
    printf("\t\t[info] %s: \"%s\"\r\n", argName, argVal);
}
static int mem;

TEST(strs, append)
{
    Args *buffs = New_strBuff();
    char *res = strsAppend(buffs, (char *)"a", (char *)"b");
    EXPECT_STREQ((char *)"ab", res);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(strs, formatInt)
{
    Args *buffs = New_strBuff();
    char *res = strsFormat(buffs, 32, "test: %d", 3);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(strs, analizeDef)
{
    mem = pikaMemNow();
    Args *buffs = New_args(NULL);
    char currentClassName[] = "Compiler";
    char line[] = "    def analizeFile(pythonApiPath: str):";
    printInfo("currentClassName", currentClassName);
    char *defSentenceWithBlock = strsRemovePrefix(buffs, line, (char *)"    def ");
    char *defSentence = strsDeleteChar(buffs, defSentenceWithBlock, ' ');
    printInfo("defSentence", defSentence);
    char *methodName = strsGetFirstToken(buffs, defSentence, '(');
    printInfo("methodName", methodName);
    char *methodObjPath = strsAppend(buffs, strsAppend(buffs, currentClassName, (char *)"."), methodName);
    printInfo("methodObjPath", methodObjPath);
    char *returnType = strsCut(buffs, defSentence, '>', ':');
    printInfo("returnType", returnType);

    char *typeList = strsCut(buffs, defSentence, '(', ')');
    printInfo("typeList", typeList);
    if (0 != strGetSize(typeList))
    {
        int argNum = strCountSign(typeList, ',') + 1;
        char *typeListBuff = strsCopy(buffs, typeList);
        for (int i = 0; i < argNum; i++)
        {
            char *typeDeclearation = strsPopToken(buffs, typeListBuff, ',');
            printInfo("typeDeclearation", typeDeclearation);
            char *argName = strsGetFirstToken(buffs, typeDeclearation, ':');
            printInfo("argName", argName);
            char *argType = strsGetLastToken(buffs, typeDeclearation, ':');
            printInfo("argType", argType);
        }
    }
    args_deinit(buffs);
    return;
}

TEST(strs, format)
{
    Args *buffs = New_args(NULL);
    char *fmt = strsFormat(buffs, 128, "test int: %d, float %f", 1, 34.2);
    EXPECT_TRUE(strEqu((char *)"test int: 1, float 34.200000", fmt));
    args_deinit(buffs);
}

TEST(strs, mem)
{
    EXPECT_EQ(pikaMemNow(), mem);
}
