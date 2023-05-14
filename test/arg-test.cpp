#include "test_common.h"
TEST_START

TEST(arg_test, cache_) {
    Arg* arg1 = arg_newInt(1);
    arg_deinit(arg1);
    Arg* arg2 = arg_newInt(2);
    int val = arg_getInt(arg2);
    EXPECT_EQ(val, 2);
    arg_deinit(arg2);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, int_) {
    Arg* arg = New_arg(NULL);
    arg = arg_setInt(arg, "test", 1);
    int a = arg_getInt(arg);
    EXPECT_EQ(a, 1);
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, bool_) {
    Arg* arg = New_arg(NULL);
    arg = arg_setBool(arg, "test", PIKA_TRUE);
    PIKA_BOOL a = arg_getBool(arg);
    EXPECT_EQ(a, PIKA_TRUE);
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, float_) {
    Arg* arg = New_arg(NULL);
    arg = arg_setFloat(arg, "test", 1.01);
    float a = arg_getFloat(arg);
    EXPECT_EQ(a, (float)1.01);
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, str_) {
    Arg* arg = New_arg(NULL);
    arg = arg_setStr(arg, "test", "test str");
    char* a = arg_getStr(arg);
    EXPECT_EQ(strEqu(a, "test str"), 1);
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, ptr) {
    Arg* arg = New_arg(NULL);
    int a = 1;
    arg = arg_setPtr(arg, "test", ARG_TYPE_POINTER, &a);
    int* pa = (int*)arg_getPtr(arg);
    EXPECT_EQ(*pa, 1);
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, name) {
    Arg* arg = New_arg(NULL);
    arg = arg_setName(arg, "test");
    Hash nameHash = arg_getNameHash(arg);
    EXPECT_EQ(hash_time33("test"), nameHash);
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, type) {
    Arg* arg = arg_newInt(1);
    pika_assert(NULL != arg);
    arg_setType(arg, ARG_TYPE_NONE);
    ArgType type = arg_getType(arg);
    EXPECT_EQ(ARG_TYPE_NONE, type);
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, null) {
    Arg* arg = New_arg(NULL);
    arg = arg_setNone(arg);
    ArgType type = arg_getType(arg);
    EXPECT_EQ(ARG_TYPE_NONE, type);
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

Arg* arg_strAppend(Arg* arg_in, char* str_to_append);
TEST(arg_test, append) {
    Arg* arg = arg_newStr("test");
    arg = arg_strAppend(arg, "hello");
    EXPECT_STREQ(arg_getStr(arg), "testhello");
    arg_deinit(arg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(arg_test, hash_1) {
    EXPECT_EQ(hash_time33("print"), 271190290);
}

TEST_END