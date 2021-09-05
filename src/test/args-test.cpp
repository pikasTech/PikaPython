#include "gtest/gtest.h"
extern "C"
{
#include "dataArgs.h"
#include "dataString.h"
}
static int mem;
TEST(args, test1)
{
    mem = pikaMemNow();
    Args *args = New_args(NULL);
    args_setInt(args, (char *)"a", 1);
    int a = args_getInt(args, (char *)"a");
    EXPECT_EQ(a, 1);
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, test4)
{
    int32_t testint = 12333;
    Args *args = New_args(NULL);
    args_setInt(args, (char *)"testint", testint);
    EXPECT_EQ(12333, args_getInt(args, (char *)"testint"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, test2)
{
    Args *args = New_args(NULL);
    float floatOut = 0;
    int64_t int64Out = 0;
    void *pointer = NULL;
    char *strOut = NULL;
    args_setInt(args, (char *)"int64Test", (int64_t)22221);
    args_setPtr(args, (char *)"pointerTest", (void *)2222322);
    args_setStr(args, (char *)"strTest", (char *)"teeeds");

    int64Out = args_getInt(args, (char *)"int64Test");
    pointer = args_getPtr(args, (char *)"pointerTest");
    strOut = args_getStr(args, (char *)"strTest");

    EXPECT_EQ(int64Out, 22221);
    EXPECT_EQ((uint64_t)pointer, 2222322);
    EXPECT_EQ(1, strEqu((char *)"teeeds", strOut));
    EXPECT_EQ(0, strcmp("int", args_getType(args, (char *)"int64Test")));
    EXPECT_EQ(0, strcmp("pointer", args_getType(args, (char *)"pointerTest")));
    EXPECT_EQ(0, strcmp("str", args_getType(args, (char *)"strTest")));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, test3)
{
    Args *args1 = New_args(NULL);
    Args *args2 = New_args(NULL);

    args_setFloat(args1, (char *)"argtest1", 2.883);
    args_setStr(args1, (char *)"strtest1", (char *)"teststr");
    args_copyArgByName(args1, (char *)"argtest1", args2);
    args_copyArgByName(args1, (char *)"strtest1", args2);

    EXPECT_EQ(2.8830f, args_getFloat(args1, (char *)"argtest1"));
    args_deinit(args1);
    args_deinit(args2);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, buff)
{
    Args *buffs = New_args(NULL);
    char *buff = args_getBuff(buffs, 256);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, update)
{
    Args *args = New_args(NULL);
    args_setInt(args, (char *)"testInt", 124);
    args_setInt(args, (char *)"testInt", 125);
    EXPECT_EQ(125, args_getInt(args, (char *)"testInt"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, print_int)
{
    Args *args = New_args(NULL);
    args_setInt(args, (char *)"testInt", 124);
    EXPECT_STREQ((char *)"124", args_print(args, (char *)"testInt"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}


TEST(args, test5)
{
    int32_t testInt = 124;
    Args *args = New_args(NULL);
    args_bind(args, (char *)"int", (char *)"testInt", &testInt);
    char *type = args_getType(args, (char *)"testInt");
    args_print(args, (char *)"testInt");
    EXPECT_STREQ((char *)"124", args_print(args, (char *)"testInt"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, test6)
{
    Args *args = New_args(NULL);
    args_setFloat(args, (char *)"testfloat", 1.42);
    EXPECT_STREQ((char *)"1.420000", args_print(args, (char *)"testfloat"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}
TEST(args, test7)
{
    Args *args = New_args(NULL);
    float floatBindTest = 2.314;
    args_bind(args, (char *)"float", (char *)"floatBind", &floatBindTest);
    EXPECT_STREQ((char *)"2.314000", args_print(args, (char *)"floatBind"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, test8)
{
    Args *args = New_args(NULL);
    args_setStr(args, (char *)"testString", (char *)"test string print");
    EXPECT_STREQ((char *)"test string print", args_print(args, (char *)"testString"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, test9)
{
    Args *args = New_args(NULL);
    char strBindTest[] = "test string bind";
    args_bindStr(args, (char *)"testStringBind", (char **)&strBindTest);
    EXPECT_STREQ((char *)"test string bind", args_print(args, (char *)"testStringBind"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, test10)
{
    Args *args = New_args(NULL);
    args_setInt(args, (char *)"testInt", 1);
    char *printOutBefore = args_print(args, (char *)"testInt");
    EXPECT_TRUE(strEqu(printOutBefore, (char *)"1"));
    args_set(args, (char *)"testInt", (char *)"4");
    char *printOutAfter = args_print(args, (char *)"testInt");
    EXPECT_TRUE(strEqu(printOutAfter, (char *)"4"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, test11)
{
    Args *args = New_args(NULL);
    char testStrBind[256] = "testtest";
    args_bindStr(args, (char *)"testStr", (char **)&testStrBind);
    char *printOutBefore = args_print(args, (char *)"testStr");
    EXPECT_TRUE(strEqu(printOutBefore, (char *)"testtest"));
    args_set(args, (char *)"testStr", (char *)"ttww");
    char *printOutAfter = args_print(args, (char *)"testStr");
    EXPECT_TRUE(strEqu(printOutAfter, (char *)"ttww"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, test12)
{
    Args *args = New_args(NULL);
    args_setInt(args, (char *)"test", 13);
    args_setInt(args, (char *)"kw", 25);
    EXPECT_EQ(2, args_getSize(args));
    EXPECT_EQ(13, (int)args_getInt(args, (char *)"test"));
    args_removeArg(args, (char *)"test");
    EXPECT_EQ(1, args_getSize(args));
    EXPECT_EQ(-999999999, (int)args_getInt(args, (char *)"test"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, mem)
{
    EXPECT_EQ(pikaMemNow(), mem);
    EXPECT_EQ(pikaMemNow(), 0);
}
