#include "gtest/gtest.h"
extern "C" {
#include "BaseObj.h"
#include "dataMemory.h"
#include "dataStack.h"
}

TEST(stack, NEW) {
    Stack* s = New_Stack();
    stack_deinit(s);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(stack, str) {
    Stack* s = New_Stack();
    stack_pushStr(s, (char*)"abc");
    stack_pushStr(s, (char*)"123");
    stack_pushStr(s, (char*)"xyz");
    char buff[32] = {0};
    EXPECT_STREQ(stack_popStr(s, buff), (char*)"xyz");
    EXPECT_STREQ(stack_popStr(s, buff), (char*)"123");
    EXPECT_STREQ(stack_popStr(s, buff), (char*)"abc");
    stack_deinit(s);
    EXPECT_EQ(pikaMemNow(), 0);
}
