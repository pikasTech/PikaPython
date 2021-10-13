#include "gtest/gtest.h"
extern "C" {
#include "BaseObj.h"
#include "PikaParser.h"
#include "dataMemory.h"
#include "dataStrs.h"
}

TEST(parser, NEW) {
    AST* ast = pikaParse((char*)"add(a,b)");
    Args* buffs = New_strBuff();
    char* sh = AST_toShell(ast, buffs);
    AST_deinit(ast);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_b) {
    AST* ast = pikaParse((char*)"add( a , b)");
    Args* buffs = New_strBuff();
    char* sh = AST_toShell(ast, buffs);
    EXPECT_STREQ(sh, "RUN add a b\n");
    AST_deinit(ast);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}