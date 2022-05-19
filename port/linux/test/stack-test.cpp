#include "gtest/gtest.h"
#include "test_common.h"
extern "C" {
#include "BaseObj.h"
#include "dataMemory.h"
#include "dataStack.h"
}

TEST(stack, NEW) {
    Stack s;
    stack_init(&s);
    stack_deinit(&s);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(stack, size) {
    Stack s;
    stack_init(&s);

    stack_pushSize(&s, 10);
    stack_pushSize(&s, 20);
    stack_pushSize(&s, 30);

    EXPECT_EQ(stack_popSize(&s), 30);
    EXPECT_EQ(stack_popSize(&s), 20);
    EXPECT_EQ(stack_popSize(&s), 10);

    stack_deinit(&s);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(stack, arg) {
    Stack s;
    stack_init(&s);

    Arg* arg1 = arg_setInt(NULL, "", 10);
    Arg* arg2 = arg_setInt(NULL, "", 20);
    Arg* arg3 = arg_setInt(NULL, "", 30);
    stack_pushArg(&s, arg1);
    stack_pushArg(&s, arg2);
    stack_pushArg(&s, arg3);
    Arg* arg4 = stack_popArg(&s);
    Arg* arg5 = stack_popArg(&s);
    Arg* arg6 = stack_popArg(&s);
    EXPECT_EQ(arg_getInt(arg4), 30);
    EXPECT_EQ(arg_getInt(arg5), 20);
    EXPECT_EQ(arg_getInt(arg6), 10);
    stack_deinit(&s);
    arg_deinit(arg4);
    arg_deinit(arg5);
    arg_deinit(arg6);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(stack, str) {
    Stack s;
    stack_init(&s);
    stack_pushStr(&s, "abc");
    stack_pushStr(&s, "123");
    stack_pushStr(&s, "xyz");
    char buff[32] = {0};
    EXPECT_STREQ(stack_popStr(&s, buff), "xyz");
    EXPECT_STREQ(stack_popStr(&s, buff), "123");
    EXPECT_STREQ(stack_popStr(&s, buff), "abc");
    stack_deinit(&s);
    EXPECT_EQ(pikaMemNow(), 0);
}
