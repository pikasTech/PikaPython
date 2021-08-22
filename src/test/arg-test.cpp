#include "gtest/gtest.h"
extern "C"
{
#include "dataArg.h"
#include "dataString.h"
}
static int mem;
extern DMEM_STATE DMEMS;

TEST(arg_test, int_)
{
    Arg *arg = New_arg(NULL);
    arg_setInt(arg, 1);
    int a = arg_getInt(arg);
    EXPECT_EQ(a, 1);
    arg_deinit(arg);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(arg_test, float_)
{
    Arg *arg = New_arg(NULL);
    arg_setFloat(arg, 1.01);
    float a = arg_getFloat(arg);
    EXPECT_EQ(a, (float)1.01);
    arg_deinit(arg);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(arg_test, str_)
{
    Arg *arg = New_arg(NULL);
    arg_setStr(arg, (char *)"test str");
    char *a = arg_getStr(arg);
    EXPECT_EQ(strEqu(a, (char *)"test str"), 1);
    arg_deinit(arg);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(arg_test, ptr)
{
    Arg *arg = New_arg(NULL);
    int a = 1;
    arg_setPtr(arg, &a);
    int *pa = (int *)arg_getPtr(arg);
    EXPECT_EQ(*pa, 1);
    arg_deinit(arg);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(arg_test, name)
{
    Arg *arg = New_arg(NULL);
    arg_setName(arg, (char *)"test");
    char *name = arg_getName(arg);
    EXPECT_EQ(1, strEqu((char *)"test", name));
    arg_deinit(arg);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(arg_test, type)
{
    Arg *arg = New_arg(NULL);
    arg_setType(arg, (char *)"test");
    char *type = arg_getType(arg);
    EXPECT_EQ(1, strEqu((char *)"test", type));
    arg_deinit(arg);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}