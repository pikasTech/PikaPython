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
    char* pikaAsm = AST_toPikaAsm(ast, buffs);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    AST_deinit(ast);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_b) {
    AST* ast = pikaParse((char*)"add( a , b)");
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_toPikaAsm(ast, buffs);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, "1 REF a\n1 REF b\n0 RUN add\n");
    args_deinit(buffs);
    AST_deinit(ast);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_b_c) {
    AST* ast = pikaParse((char*)"d = add(add(a,b)  , c)");
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_toPikaAsm(ast, buffs);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "2 REF a\n2 REF b\n1 RUN add\n1 REF c\n0 RUN add\n0 OUT d\n");
    args_deinit(buffs);
    AST_deinit(ast);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, method1) {
    AST* ast = pikaParse((char*)"d.p = a.add(b.add(a,se.b)  , pmw.c)");
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_toPikaAsm(ast, buffs);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "2 REF a\n2 REF se.b\n1 RUN b.add\n1 REF pmw.c\n0 RUN "
                 "a.add\n0 OUT d.p\n");
    args_deinit(buffs);
    AST_deinit(ast);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, method2) {
    char* line = (char*)"d.p = a.add(b.add(a,se.b,diek(pp))  , pmw.c())";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseToAsm(buffs, line);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "2 REF a\n2 REF se.b\n3 REF pp\n2 RUN diek\n1 RUN b.add\n1 "
                 "RUN pmw.c\n0 RUN a.add\n0 OUT d.p\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, str1) {
    char* line = (char*)"literal('2.322')";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseToAsm(buffs, line);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, "1 STR 2.322\n0 RUN literal\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, str2) {
    char* line = (char*)"b=add(a,literal('1'))";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseToAsm(buffs, line);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "1 REF a\n2 STR 1\n1 RUN literal\n0 RUN add\n0 OUT b\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, num1) {
    char* line = (char*)"b=add(a,1)";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseToAsm(buffs, line);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, "1 REF a\n1 NUM 1\n0 RUN add\n0 OUT b\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_str) {
    char* line = (char*)"b=add(a,'1')";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseToAsm(buffs, line);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, "1 REF a\n1 STR 1\n0 RUN add\n0 OUT b\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, deep4) {
    char* line = (char*)"b = add(add(add(add(1, 2), 3), 4), 5)";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseToAsm(buffs, line);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, "4 NUM 1\n4 NUM 2\n3 RUN add\n3 NUM 3\n2 RUN add\n2 NUM 4\n1 RUN add\n1 NUM 5\n0 RUN add\n0 OUT b\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, a_1) {
    char* line = (char*)"a = 1";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseToAsm(buffs, line);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, "0 NUM 1\n0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}