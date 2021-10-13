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
    printf("%s", sh);
    AST_deinit(ast);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_b) {
    AST* ast = pikaParse((char*)"add( a , b)");
    Args* buffs = New_strBuff();
    char* sh = AST_toShell(ast, buffs);
    printf("%s", sh);
    EXPECT_STREQ(sh, "1 REF a\n1 REF b\n0 RUN add\n");
    AST_deinit(ast);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_b_c) {
    AST* ast = pikaParse((char*)"d = add(add(a,b)  , c)");
    Args* buffs = New_strBuff();
    char* sh = AST_toShell(ast, buffs);
    printf("%s", sh);
    EXPECT_STREQ(sh, "2 REF a\n2 REF b\n1 RUN add\n1 REF c\n0 RUN add\n0 OUT d\n");
    AST_deinit(ast);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, method1) {
    AST* ast = pikaParse((char*)"d.p = a.add(b.add(a,se.b)  , pmw.c)");
    Args* buffs = New_strBuff();
    char* sh = AST_toShell(ast, buffs);
    printf("%s", sh);
    EXPECT_STREQ(sh,
                 "2 REF a\n2 REF se.b\n1 RUN b.add\n1 REF pmw.c\n0 RUN a.add\n0 OUT d.p\n");
    AST_deinit(ast);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, method2) {
    AST* ast =
        pikaParse((char*)"d.p = a.add(b.add(a,se.b,diek(pp))  , pmw.c())");
    Args* buffs = New_strBuff();
    char* sh = AST_toShell(ast, buffs);
    printf("%s", sh);
    EXPECT_STREQ(sh,
                 "2 REF a\n2 REF se.b\n3 REF pp\n2 RUN diek\n1 RUN b.add\n1 RUN pmw.c\n0 RUN a.add\n0 OUT d.p\n");
    AST_deinit(ast);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}