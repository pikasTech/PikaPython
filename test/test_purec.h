/* clang-format off */
#ifndef __TEST__PUREC__H
    #include <assert.h>
    #define __TEST__PUREC__H
    #define TEST_END }
    #define TEST_START extern "C" {
    #define TEST(_group, _name) static void test_##_group##_##_name(void)
    #define EXPECT_EQ(_a, _b) assert(_a == _b)
    #define EXPECT_STREQ(_a, _b) assert(strEqu(_a, _b) == 1)
    #define ASSERT_FLOAT_EQ(_a, _b) assert(_a == _b)
    #define EXPECT_FLOAT_EQ(_a, _b) assert(_a == _b)
    #define EXPECT_DOUBLE_EQ(_a, _b) assert(_a == _b)
    #define ASSERT_EQ(_a, _b) assert(_a == _b)
    #define ASSERT_STREQ(_a, _b) assert(strEqu(_a, _b) == 1)
    #define EXPECT_NE(_a, _b) assert(_a != _b)
    #define EXPECT_TRUE(_a) assert(_a)
#endif
/* clang-format on */