/* clang-format off */
#ifndef __TEST__PUREC__H
    #include <stdio.h>
    extern volatile int test_num;
    #define __TEST__PUREC__H
    #define TEST_START extern "C" {
    #define TEST(_group, _name) void test_##_group##_##_name(void)
    #define EXPECT_EQ(_a, _b) _pika_assert((uintptr_t)_a == (uintptr_t)_b)
    #define EXPECT_STREQ(_a, _b) _pika_assert(strEqu(_a, _b) == 1)
    #define ASSERT_FLOAT_EQ(_a, _b) _pika_assert((_a - _b) * (_a - _b) < 0.0001)
    #define EXPECT_FLOAT_EQ(_a, _b) ASSERT_FLOAT_EQ(_a, _b)
    #define EXPECT_DOUBLE_EQ(_a, _b) ASSERT_FLOAT_EQ(_a, _b)
    #define ASSERT_EQ(_a, _b) _pika_assert(_a == _b)
    #define ASSERT_STREQ(_a, _b) _pika_assert(strEqu(_a, _b) == 1)
    #define EXPECT_NE(_a, _b) _pika_assert(_a != _b)
    #define EXPECT_TRUE(_a) _pika_assert(_a)
    #define TEST_END                                 \
    }
/* clang-format on */
#define RUN_TEST(_group, _name)                      \
    printf("[ RUN    ] %s_%s\r\n", #_group, #_name); \
    void test_##_group##_##_name(void);              \
    test_##_group##_##_name();                       \
    printf("[     OK ] %s_%s\r\n", #_group, #_name); \
    test_num++;
#endif