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
    EXPECT_STREQ(sh, "REF a\nREF b\nRUN add\n");
    AST_deinit(ast);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_b_c) {
    AST* ast = pikaParse((char*)"d = add(add(a,b)  , c)");
    Args* buffs = New_strBuff();
    char* sh = AST_toShell(ast, buffs);
    EXPECT_STREQ(sh, "REF a\nREF b\nRUN add\nREF c\nRUN add\nOUT d\n");
    AST_deinit(ast);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, method1) {
    AST* ast = pikaParse((char*)"d.p = a.add(b.add(a,se.b)  , pmw.c)");
    Args* buffs = New_strBuff();
    char* sh = AST_toShell(ast, buffs);
    EXPECT_STREQ(sh, "REF a\nREF se.b\nRUN b.add\nREF pmw.c\nRUN a.add\nOUT d.p\n");
    AST_deinit(ast);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}