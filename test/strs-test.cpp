#include "test_common.h"
TEST_START

extern pikaMemInfo g_pikaMemInfo;
/* the log_buff of printf */
extern char log_buff[LOG_BUFF_MAX][LOG_SIZE];

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
    char* defSentenceWithBlock = strsRemovePrefix(buffs, line, "    def ");
    char* defSentence = strsDeleteChar(buffs, defSentenceWithBlock, ' ');
    printInfo("defSentence", defSentence);
    char* methodName = strsGetFirstToken(buffs, defSentence, '(');
    printInfo("methodName", methodName);
    char* methodObjPath =
        strsAppend(buffs, strsAppend(buffs, currentClassName, "."), methodName);
    printInfo("methodObjPath", methodObjPath);
    char* returnType = strsCut(buffs, defSentence, '>', ':');
    printInfo("returnType", returnType);

    char* typeList = strsCut(buffs, defSentence, '(', ')');
    printInfo("typeList", typeList);
    if (0 != strGetSize(typeList)) {
        int argNum = strCountSign(typeList, ',') + 1;
        char* typeListBuff = strsCopy(buffs, typeList);
        for (int i = 0; i < argNum; i++) {
            char* typeDeclareation = strsPopToken(buffs, &typeListBuff, ',');
            printInfo("typeDeclareation", typeDeclareation);
            char* argName = strsGetFirstToken(buffs, typeDeclareation, ':');
            printInfo("argName", argName);
            char* argType = strPointToLastToken(typeDeclareation, ':');
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
    Arg* str_arg = arg_newStr("a");
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

TEST(str, transfer) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_run(pikaMain, "'\\r\\n'\n");
    /* collect */
    /* assert */

    EXPECT_STREQ(log_buff[0], "'\r\n'\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(str, transfer_issue_jfo4i) {
    /* init */
    g_pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_run(pikaMain, "'\\\\replace'");
    pikaVM_run(pikaMain, "'\\\\non'");
    pikaVM_run(pikaMain, "'\\\\title'");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[2], "'\\replace'\r\n");
    EXPECT_STREQ(log_buff[1], "'\\non'\r\n");
    EXPECT_STREQ(log_buff[0], "'\\title'\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(strs, path_join) {
    Args* buffs = New_strBuff();
    EXPECT_STREQ(strsPathJoin(buffs, "", "test.py"), "test.py");
    EXPECT_STREQ(strsPathJoin(buffs, "test", "test.py"), "test/test.py");
    EXPECT_STREQ(strsPathJoin(buffs, "test/", "test.py"), "test/test.py");
    EXPECT_STREQ(strsPathJoin(buffs, "test/", "/test.py"), "test/test.py");
    EXPECT_STREQ(strsPathJoin(buffs, "path1", "path2/"), "path1/path2/");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST_END