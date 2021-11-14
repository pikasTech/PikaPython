#include "gtest/gtest.h"
extern "C" {
#include "dataArg.h"
#include "dataString.h"
}

TEST(arg_test, int_) {
    Arg* arg = New_arg(NULL);
    arg = arg_setInt(arg, (char*)"test", 1);
    int a = arg_getInt(arg);
    EXPECT_EQ(a, 1);
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, float_) {
    Arg* arg = New_arg(NULL);
    arg = arg_setFloat(arg, (char*)"test", 1.01);
    float a = arg_getFloat(arg);
    EXPECT_EQ(a, (float)1.01);
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, str_) {
    Arg* arg = New_arg(NULL);
    arg = arg_setStr(arg, (char*)"test", (char*)"test str");
    char* a = arg_getStr(arg);
    EXPECT_EQ(strEqu(a, (char*)"test str"), 1);
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, ptr) {
    Arg* arg = New_arg(NULL);
    int a = 1;
    arg = arg_setPtr(arg, (char*)"test", (char*)"p", &a);
    int* pa = (int*)arg_getPtr(arg);
    EXPECT_EQ(*pa, 1);
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, name) {
    Arg* arg = New_arg(NULL);
    arg = arg_setName(arg, (char*)"test");
    Hash nameHash = arg_getNameHash(arg);
    EXPECT_EQ(hash_time33((char*)"test"), nameHash);
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, type) {
    Arg* arg = New_arg(NULL);
    arg = arg_setType(arg, (char*)"test");
    char* type = arg_getType(arg);
    EXPECT_EQ(1, strEqu((char*)"test", type));
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}
