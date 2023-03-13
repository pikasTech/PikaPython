#include "test_common.h"
TEST_START

extern "C" {
/* head infomation */
typedef QueueObj AST;
char* Parser_linesToAsm(Args* outBuffs, char* multiLine);
char* AST_genAsm(AST* ast, Args* outBuffs);
AST* AST_parseLine(char* line, Stack* blockStack);
char* Parser_LineToAsm(Args* buffs, char* line, Stack* blockStack);
int32_t AST_deinit(AST* ast);
char* Lexer_getTokenStream(Args* outBuffs, char* stmt);
char* Lexer_printTokenStream(Args* outBuffs, char* tokens);
char* Cursor_getCleanStmt(Args* outBuffs, char* cmd);
char* Cursor_popLastToken(Args* outBuffs, char** pStmt, char* str);
}

TEST(parser, NEW) {
    AST* ast = AST_parseLine("add(a,b)", NULL);
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_genAsm(ast, buffs);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    AST_deinit(ast);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_b) {
    AST* ast = AST_parseLine("add( a , b)", NULL);
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_genAsm(ast, buffs);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 REF b\n"
                 "0 RUN add\n");
    args_deinit(buffs);
    AST_deinit(ast);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_b_c) {
    AST* ast = AST_parseLine("d = add(add(a,b)  , c)", NULL);
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_genAsm(ast, buffs);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 REF a\n"
                 "2 REF b\n"
                 "1 RUN add\n"
                 "1 REF c\n"
                 "0 RUN add\n"
                 "0 OUT d\n");
    args_deinit(buffs);
    AST_deinit(ast);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, method1) {
    AST* ast = AST_parseLine("d.p = a.add(b.add(a,se.b)  , pmw.c)", NULL);
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_genAsm(ast, buffs);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 REF a\n"
                 "2 REF se.b\n"
                 "1 RUN b.add\n"
                 "1 REF pmw.c\n"
                 "0 RUN "
                 "a.add\n"
                 "0 OUT d.p\n");
    args_deinit(buffs);
    AST_deinit(ast);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, method2) {
    char* line = "d.p = a.add(b.add(a,se.b,diek(pp))  , pmw.c())";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 REF a\n"
                 "2 REF se.b\n"
                 "3 REF pp\n"
                 "2 RUN diek\n"
                 "1 RUN b.add\n"
                 "1 RUN pmw.c\n"
                 "0 RUN a.add\n"
                 "0 OUT d.p\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, method_void) {
    char* line = "a(c())";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "1 RUN c\n"
                                "0 RUN a\n");

    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, str1) {
    char* line = "literal('2.322')";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR 2.322\n"
                 "0 RUN literal\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, str2) {
    char* line = "b=add(a,literal('1'))";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "2 STR 1\n"
                 "1 RUN literal\n"
                 "0 RUN add\n"
                 "0 OUT b\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, num1) {
    char* line = "b=add(a,1)";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 NUM 1\n"
                 "0 RUN add\n"
                 "0 OUT b\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_str) {
    char* line = "b=add(a,'1')";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 STR 1\n"
                 "0 RUN add\n"
                 "0 OUT b\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, deep4) {
    char* line = "b = add(add(add(add(1, 2), 3), 4), 5)";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "4 NUM 1\n"
                 "4 NUM 2\n"
                 "3 RUN add\n"
                 "3 NUM 3\n"
                 "2 RUN add\n"
                 "2 NUM 4\n"
                 "1 RUN add\n"
                 "1 NUM 5\n"
                 "0 RUN add\n"
                 "0 OUT b\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, a_1) {
    char* line = "a = 1";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 NUM 1\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, while_true) {
    char* line = "while true:";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 REF true\n"
                 "0 JEZ 2\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

static char* parse(const char* line,
                   Args* outBuffs,
                   char* pikaAsm,
                   Stack* blockStack) {
    printf("%s\n", line);
    Args* runBuffs = New_strBuff();
    pikaAsm = strsAppend(runBuffs, pikaAsm,
                         Parser_LineToAsm(runBuffs, (char*)line, blockStack));
    pikaAsm = strsCopy(outBuffs, pikaAsm);
    args_deinit(runBuffs);
    return pikaAsm;
}

TEST(parser, while_true_block) {
    Args* bf = New_strBuff();
    Stack bs;
    stack_init(&bs);
    char* s = strsCopy(bf, "");
    s = parse("while true:", bf, s, &bs);
    s = parse("    rgb.flow()", bf, s, &bs);
    s = parse("", bf, s, &bs);
    printf("%s", s);
    EXPECT_STREQ(s,
                 "B0\n"
                 "0 REF true\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 RUN rgb.flow\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n");
    stack_deinit(&bs);
    args_deinit(bf);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, while_true_false) {
    Args* bf = New_strBuff();
    Stack bs;
    stack_init(&bs);
    char* s = strsCopy(bf, "");
    s = parse("while true:", bf, s, &bs);
    s = parse("    rgb.flow()", bf, s, &bs);
    s = parse("    while false:", bf, s, &bs);
    s = parse("        a=3", bf, s, &bs);
    s = parse("        test.on(add(2,3))", bf, s, &bs);
    s = parse("    print('flowing')", bf, s, &bs);
    s = parse("", bf, s, &bs);
    Arg* buffArg = arg_newStr(s);
    stack_deinit(&bs);
    args_deinit(bf);
    s = arg_getStr(buffArg);
    printf("%s", s);
    EXPECT_STREQ(s,
                 "B0\n"
                 "0 REF true\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 RUN rgb.flow\n"
                 "B1\n"
                 "0 REF false\n"
                 "0 JEZ 2\n"
                 "B2\n"
                 "0 NUM 3\n"
                 "0 OUT a\n"
                 "B2\n"
                 "2 NUM 2\n"
                 "2 NUM 3\n"
                 "1 RUN add\n"
                 "0 RUN test.on\n"
                 "B1\n"
                 "0 JMP -1\n"
                 "B1\n"
                 "1 STR flowing\n"
                 "0 RUN print\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n");
    arg_deinit(buffArg);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, while_true_false_both_exit) {
    Args* bf = New_strBuff();
    Stack bs;
    stack_init(&bs);
    char* s = strsCopy(bf, "");
    s = parse("while true:", bf, s, &bs);
    s = parse("    rgb.flow()", bf, s, &bs);
    s = parse("    while false:", bf, s, &bs);
    s = parse("        a=3", bf, s, &bs);
    s = parse("        test.on(add(2,3))", bf, s, &bs);
    s = parse("", bf, s, &bs);
    printf("%s", s);
    EXPECT_STREQ(s,
                 "B0\n"
                 "0 REF true\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 RUN rgb.flow\n"
                 "B1\n"
                 "0 REF false\n"
                 "0 JEZ 2\n"
                 "B2\n"
                 "0 NUM 3\n"
                 "0 OUT a\n"
                 "B2\n"
                 "2 NUM 2\n"
                 "2 NUM 3\n"
                 "1 RUN add\n"
                 "0 RUN test.on\n"
                 "B1\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n");
    stack_deinit(&bs);
    args_deinit(bf);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, if_) {
    Args* bf = New_strBuff();
    Stack bs;
    stack_init(&bs);
    char* s = strsCopy(bf, "");
    s = parse("if true:", bf, s, &bs);
    s = parse("    rgb.flow()", bf, s, &bs);
    s = parse("", bf, s, &bs);
    printf("%s", s);
    EXPECT_STREQ(s,
                 "B0\n"
                 "0 REF true\n"
                 "0 JEZ 1\n"
                 "B1\n"
                 "0 RUN rgb.flow\n"
                 "B0\n");
    stack_deinit(&bs);
    args_deinit(bf);
    EXPECT_EQ(pikaMemNow(), 0);
}

extern PikaMemInfo g_PikaMemInfo;
TEST(parser, while_true_if_false_both_exit) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* bf = New_strBuff();
    Stack bs;
    stack_init(&bs);
    char* s = strsCopy(bf, "");
    s = parse("while true:", bf, s, &bs);
    s = parse("    rgb.flow()", bf, s, &bs);
    s = parse("    if false:", bf, s, &bs);
    s = parse("        a=3", bf, s, &bs);
    s = parse("        test.on(add(2,3))", bf, s, &bs);
    s = parse("", bf, s, &bs);
    printf("%s", s);
    EXPECT_STREQ(s,
                 "B0\n"
                 "0 REF true\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 RUN rgb.flow\n"
                 "B1\n"
                 "0 REF false\n"
                 "0 JEZ 1\n"
                 "B2\n"
                 "0 NUM 3\n"
                 "0 OUT a\n"
                 "B2\n"
                 "2 NUM 2\n"
                 "2 NUM 3\n"
                 "1 RUN add\n"
                 "0 RUN test.on\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n");
    stack_deinit(&bs);
    args_deinit(bf);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, multiLine) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char *lines = (char *)"while true:\n"
                        "    rgb.flow()\n"
                        "    if false:\n"
                        "        a=3\n"
                        "        test.on(add(2,3))\n"
                        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 REF true\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 RUN rgb.flow\n"
                 "B1\n"
                 "0 REF false\n"
                 "0 JEZ 1\n"
                 "B2\n"
                 "0 NUM 3\n"
                 "0 OUT a\n"
                 "B2\n"
                 "2 NUM 2\n"
                 "2 NUM 3\n"
                 "1 RUN add\n"
                 "0 RUN test.on\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, pikaPi) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    const char lines[] =
        "\n"
        "time = STM32.Time()\n"
        "uart = STM32.UART()\n"
        "adc = STM32.ADC()\n"
        "pin = STM32.GPIO()\n"
        "pwm = STM32.PWM()\n"
        "uart = STM32.UART()\n"
        "rgb = PikaPiZero.RGB()\n"
        "mem = PikaStdLib.MemChecker()\n"
        "op = PikaMath.Operator()\n"
        "\n"
        "uart.init()\n"
        "uart.setId(1)\n"
        "uart.setBaudRate(115200)\n"
        "uart.enable()\n"
        "\n"
        "rgb.init()\n"
        "rgb.enable()\n"
        "\n"
        "print('hello 2')\n"
        "print('mem used max:')\n"
        "mem.max() \n"
        "\n"
        "while True:\n"
        "    time.sleep_ms(10)\n"
        "    rgb.flow()\n"
        "    print('flowing')\n"
        "\n"
        "\n"
        "\n"
        "\n";

    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, (char*)lines);
    printf("mem max in parse: %0.2f Kb\n", pikaMemMax() / 1024.0);
    printf("%s", pikaAsm);

    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 RUN STM32.Time\n"
                 "0 OUT time\n"
                 "B0\n"
                 "0 RUN STM32.UART\n"
                 "0 OUT uart\n"
                 "B0\n"
                 "0 RUN STM32.ADC\n"
                 "0 OUT adc\n"
                 "B0\n"
                 "0 RUN STM32.GPIO\n"
                 "0 OUT pin\n"
                 "B0\n"
                 "0 RUN STM32.PWM\n"
                 "0 OUT pwm\n"
                 "B0\n"
                 "0 RUN STM32.UART\n"
                 "0 OUT uart\n"
                 "B0\n"
                 "0 RUN PikaPiZero.RGB\n"
                 "0 OUT rgb\n"
                 "B0\n"
                 "0 RUN PikaStdLib.MemChecker\n"
                 "0 OUT mem\n"
                 "B0\n"
                 "0 RUN PikaMath.Operator\n"
                 "0 OUT op\n"
                 "B0\n"
                 "0 RUN uart.init\n"
                 "B0\n"
                 "1 NUM 1\n"
                 "0 RUN uart.setId\n"
                 "B0\n"
                 "1 NUM 115200\n"
                 "0 RUN uart.setBaudRate\n"
                 "B0\n"
                 "0 RUN uart.enable\n"
                 "B0\n"
                 "0 RUN rgb.init\n"
                 "B0\n"
                 "0 RUN rgb.enable\n"
                 "B0\n"
                 "1 STR hello 2\n"
                 "0 RUN print\n"
                 "B0\n"
                 "1 STR mem used max:\n"
                 "0 RUN print\n"
                 "B0\n"
                 "0 RUN mem.max\n"
                 "B0\n"
                 "0 REF True\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "1 NUM 10\n"
                 "0 RUN time.sleep_ms\n"
                 "B1\n"
                 "0 RUN rgb.flow\n"
                 "B1\n"
                 "1 STR flowing\n"
                 "0 RUN print\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = 1 + 1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 NUM 1\n"
                 "1 NUM 1\n"
                 "0 OPT +\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_3) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = 1 + 2 + 3";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 1\n"
                 "2 NUM 2\n"
                 "1 OPT +\n"
                 "1 NUM 3\n"
                 "0 OPT +\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_pp) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = a + 1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 NUM 1\n"
                 "0 OPT +\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, while_a_pp) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "while a < 10:\n"
        "    print(a)\n"
        "    a = a + 1\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 NUM 10\n"
                 "0 OPT <\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "1 REF a\n"
                 "0 RUN print\n"
                 "B1\n"
                 "1 REF a\n"
                 "1 NUM 1\n"
                 "0 OPT +\n"
                 "0 OUT a\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_m2p3) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = 1 * 2 + 3\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 1\n"
                 "2 NUM 2\n"
                 "1 OPT *\n"
                 "1 NUM 3\n"
                 "0 OPT +\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_m2p3_) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = 1 * (2 + 3)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 NUM 1\n"
                 "3 NUM 2\n"
                 "3 NUM 3\n"
                 "2 OPT +\n"
                 "1 RUN \n"
                 "0 OPT *\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_m12p3_) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = (1 + 2) * 3\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "3 NUM 1\n"
                 "3 NUM 2\n"
                 "2 OPT +\n"
                 "1 RUN \n"
                 "1 NUM 3\n"
                 "0 OPT *\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, method_equ) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "if right.read() == 1:\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 RUN right.read\n"
                 "1 NUM 1\n"
                 "0 OPT ==\n"
                 "0 JEZ 1\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, equ_method) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "if 1 == right.read() :\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 NUM 1\n"
                 "1 RUN right.read\n"
                 "0 OPT ==\n"
                 "0 JEZ 1\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, def_add) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def add(a, b):\n"
        "    a + b";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "0 DEF add(a,b)\n"
                                "0 JMP 1\n"
                                "B1\n"
                                "1 REF a\n"
                                "1 REF b\n"
                                "0 OPT +\n"
                                "B1\n"
                                "0 RET \n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, def_add_return) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def add(a, b):\n"
        "    return a + b\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "0 DEF add(a,b)\n"
                                "0 JMP 1\n"
                                "B1\n"
                                "1 REF a\n"
                                "1 REF b\n"
                                "0 OPT +\n"
                                "0 RET \n"
                                "B1\n"
                                "0 RET \n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, def_while_return) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def add(a, b):\n"
        "    while True:\n"
        "        return a + b\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "0 DEF add(a,b)\n"
                                "0 JMP 1\n"
                                "B1\n"
                                "0 REF True\n"
                                "0 JEZ 2\n"
                                "B2\n"
                                "1 REF a\n"
                                "1 REF b\n"
                                "0 OPT +\n"
                                "0 RET \n"
                                "B1\n"
                                "0 JMP -1\n"
                                "B1\n"
                                "0 RET \n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, def_while_return_void) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def add(a, b):\n"
        "    while True:\n"
        "        return\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "0 DEF add(a,b)\n"
                                "0 JMP 1\n"
                                "B1\n"
                                "0 REF True\n"
                                "0 JEZ 2\n"
                                "B2\n"
                                "0 RET \n"
                                "B1\n"
                                "0 JMP -1\n"
                                "B1\n"
                                "0 RET \n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, signed_num) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = -1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    char* tokens_print =
        Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines));
    printf("%s", tokens_print);
    EXPECT_STREQ(tokens_print, "{sym}a{opt}={opt}-{lit}1\n");
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "1 NUM 1\n"
                                "0 OPT -\n"
                                "0 OUT a\n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, comp_signed_num) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "if a > -1:\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "1 REF a\n"
                                "2 NUM 1\n"
                                "1 OPT -\n"
                                "0 OPT >\n"
                                "0 JEZ 1\n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, symbol_add) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, " res = add(1,2)");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream,
                 "{sym}res{opt}={sym}add{dvd}({lit}1{dvd},{lit}2{dvd})");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, symbol_1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, "a(");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);

    /* assert */
    EXPECT_STREQ(printTokenStream, "{sym}a{dvd}(");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, operator_not) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, "not not not ");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream, "{opt} not {opt} not {opt} not ");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, symbol_Nag) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, "-10-20");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream, "{opt}-{lit}10{opt}-{lit}20");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, operator_all) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs,
                                        "not or and "
                                        "+ += - -="
                                        "* ** *= **="
                                        "/ // /= //="
                                        "% %= = == !="
                                        "> >= >>"
                                        "< <= <<"
                                        "&|^~");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(
        printTokenStream,
        "{opt} not {opt} or {opt} and {opt}+{opt}+={opt}-{opt}-={opt}*{opt}*"
        "*{opt}*={opt}**={opt}/{opt}//{opt}/={opt}//"
        "={opt}%{opt}%={opt}={opt}=={opt}!={opt}>{opt}>={opt}>>{opt}<{"
        "opt}<={opt}<<{opt}&{opt}|{opt}^{opt}~");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, symbol_2) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, "a+b-c(25**=ek)!=-28");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream,
                 "{sym}a{opt}+{sym}b{opt}-{sym}c{dvd}({lit}25{opt}**={sym}ek{"
                 "dvd}){opt}!={opt}-{lit}28");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, symbol_and) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, " res = add(1 and lkj,2)");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream,
                 "{sym}res{opt}={sym}add{dvd}({lit}1{opt} and "
                 "{sym}lkj{dvd},{lit}2{dvd})");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, sting) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, " a= 'elk 2'");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream, "{sym}a{opt}={lit}'elk 2'");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, num_1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, "1");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream, "{lit}1");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, jjcc) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, "a = (1 + 1.1) * 3 - 2 /4.0");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream,
                 "{sym}a{opt}={dvd}({lit}1{opt}+{lit}1.1{dvd}){opt}*{lit}3{opt}"
                 "-{lit}2{opt}/{lit}4.0");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, pop_by_str) {
    Args* buffs = New_strBuff();
    char* tokens = strsCopy(buffs, "3(>=)2>=29");
    char* token2 = Cursor_popLastToken(buffs, &tokens, ">=");
    char* token1 = tokens;
    /* assert */
    EXPECT_STREQ("3(>=)2", token1);
    EXPECT_STREQ("29", token2);
    args_deinit(buffs);
}

TEST(parser, mm) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = a ** -1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "2 NUM 1\n"
                 "1 OPT -\n"
                 "0 OPT **\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, self_inc) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "a += -1\n"
        "a -= -1\n"
        "a *= -1\n"
        "a /= -1\n"
        "a **= -1\n"
        "a //= -1\n"
        "a >= -1\n"
        "a <= -1\n"
        "a != -1\n"
        "a %= -1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "1 REF a\n"
                                "3 NUM 1\n"
                                "2 OPT -\n"
                                "1 RUN \n"
                                "0 OPT +\n"
                                "0 OUT a\n"
                                "B0\n"
                                "1 REF a\n"
                                "3 NUM 1\n"
                                "2 OPT -\n"
                                "1 RUN \n"
                                "0 OPT -\n"
                                "0 OUT a\n"
                                "B0\n"
                                "1 REF a\n"
                                "3 NUM 1\n"
                                "2 OPT -\n"
                                "1 RUN \n"
                                "0 OPT *\n"
                                "0 OUT a\n"
                                "B0\n"
                                "1 REF a\n"
                                "3 NUM 1\n"
                                "2 OPT -\n"
                                "1 RUN \n"
                                "0 OPT /\n"
                                "0 OUT a\n"
                                "B0\n"
                                "1 REF a\n"
                                "2 NUM 1\n"
                                "1 OPT -\n"
                                "0 OPT **=\n"
                                "B0\n"
                                "1 REF a\n"
                                "2 NUM 1\n"
                                "1 OPT -\n"
                                "0 OPT //=\n"
                                "B0\n"
                                "1 REF a\n"
                                "2 NUM 1\n"
                                "1 OPT -\n"
                                "0 OPT >=\n"
                                "B0\n"
                                "1 REF a\n"
                                "2 NUM 1\n"
                                "1 OPT -\n"
                                "0 OPT <=\n"
                                "B0\n"
                                "1 REF a\n"
                                "2 NUM 1\n"
                                "1 OPT -\n"
                                "0 OPT !=\n"
                                "B0\n"
                                "1 REF a\n"
                                "2 NUM 1\n"
                                "1 OPT -\n"
                                "0 OPT %=\n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, n_n1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = ~-1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 1\n"
                 "1 OPT -\n"
                 "0 OPT ~\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, or_) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "( a>1) or (b<= 3)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "3 REF a\n"
                                "3 NUM 1\n"
                                "2 OPT >\n"
                                "1 RUN \n"
                                "3 REF b\n"
                                "3 NUM 3\n"
                                "2 OPT <=\n"
                                "1 RUN \n"
                                "0 OPT  or \n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, _or_) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "if (1>2) and (2>1):\n"
        "    b = 1\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "3 NUM 1\n"
                                "3 NUM 2\n"
                                "2 OPT >\n"
                                "1 RUN \n"
                                "3 NUM 2\n"
                                "3 NUM 1\n"
                                "2 OPT >\n"
                                "1 RUN \n"
                                "0 OPT  and \n"
                                "0 JEZ 1\n"
                                "B1\n"
                                "0 NUM 1\n"
                                "0 OUT b\n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, annotation) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = t#test\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "0 REF t\n"
                                "0 OUT a\n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, annotation_block) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "while True:\n"
        "    a = 1";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "0 REF True\n"
                                "0 JEZ 2\n"
                                "B1\n"
                                "0 NUM 1\n"
                                "0 OUT a\n"
                                "B0\n"
                                "0 JMP -1\n"
                                "B0\n");
    lines =
        "while True:\n"
        "    a = 1\n"
        "#";
    printf("%s", lines);
    pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "0 REF True\n"
                                "0 JEZ 2\n"
                                "B1\n"
                                "0 NUM 1\n"
                                "0 OUT a\n"
                                "B0\n"
                                "0 JMP -1\n"
                                "B0\n");
    lines =
        "while True:\n"
        "    a = 1\n"
        "#test\n"
        "    b = 2";
    printf("%s", lines);
    pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "0 REF True\n"
                                "0 JEZ 2\n"
                                "B1\n"
                                "0 NUM 1\n"
                                "0 OUT a\n"
                                "B1\n"
                                "0 NUM 2\n"
                                "0 OUT b\n"
                                "B0\n"
                                "0 JMP -1\n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, if_elif_else) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "if a > 1:\n"
        "    b = 1\n"
        "elif a > 2:\n"
        "    b = 2\n"
        "    if a > 1:\n"
        "        b = 1\n"
        "    elif a > 2:\n"
        "        b = 2\n"
        "    else:\n"
        "        b = 3\n"
        "else:\n"
        "    b = 3\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "1 REF a\n"
                                "1 NUM 1\n"
                                "0 OPT >\n"
                                "0 JEZ 1\n"
                                "B1\n"
                                "0 NUM 1\n"
                                "0 OUT b\n"
                                "B0\n"
                                "0 NEL 1\n"
                                "1 REF a\n"
                                "1 NUM 2\n"
                                "0 OPT >\n"
                                "0 JEZ 1\n"
                                "B1\n"
                                "0 NUM 2\n"
                                "0 OUT b\n"
                                "B1\n"
                                "1 REF a\n"
                                "1 NUM 1\n"
                                "0 OPT >\n"
                                "0 JEZ 1\n"
                                "B2\n"
                                "0 NUM 1\n"
                                "0 OUT b\n"
                                "B1\n"
                                "0 NEL 1\n"
                                "1 REF a\n"
                                "1 NUM 2\n"
                                "0 OPT >\n"
                                "0 JEZ 1\n"
                                "B2\n"
                                "0 NUM 2\n"
                                "0 OUT b\n"
                                "B1\n"
                                "0 NEL 1\n"
                                "B2\n"
                                "0 NUM 3\n"
                                "0 OUT b\n"
                                "B0\n"
                                "0 NEL 1\n"
                                "B1\n"
                                "0 NUM 3\n"
                                "0 OUT b\n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, for_range) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "for i in range(0,10):\n"
        "    print(i)\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 0\n"
                 "2 NUM 10\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT $l0\n"
                 "B0\n"
                 "0 RUN $l0.__next__\n"
                 "0 OUT i\n"
                 "0 EST i\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "1 REF i\n"
                 "0 RUN print\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL $l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, for_range_rtt) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "for i in range(0, 3):\n"
        "    print('hello PikaScript!')\n"
        "    thread.mdelay(500)\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 0\n"
                 "2 NUM 3\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT $l0\n"
                 "B0\n"
                 "0 RUN $l0.__next__\n"
                 "0 OUT i\n"
                 "0 EST i\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "1 STR hello PikaScript!\n"
                 "0 RUN print\n"
                 "B1\n"
                 "1 NUM 500\n"
                 "0 RUN thread.mdelay\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL $l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, for_list) {
    Args* bf = New_strBuff();
    Stack bs;
    stack_init(&bs);
    char* s = strsCopy(bf, "");
    s = parse("for arg in xrange(0,10):", bf, s, &bs);
    s = parse("    print(arg)", bf, s, &bs);
    s = parse("", bf, s, &bs);
    printf("%s", s);
    EXPECT_STREQ(s,
                 "B0\n"
                 "2 NUM 0\n"
                 "2 NUM 10\n"
                 "1 RUN xrange\n"
                 "0 RUN iter\n"
                 "0 OUT $l0\n"
                 "B0\n"
                 "0 RUN $l0.__next__\n"
                 "0 OUT arg\n"
                 "0 EST arg\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "1 REF arg\n"
                 "0 RUN print\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL $l0\n"
                 "B0\n");
    stack_deinit(&bs);
    args_deinit(bf);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, for_for_range) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "a = 0\n"
        "for i in range(0, 10):\n"
        "    for k in range(0, 3):\n"
        "        print(k)\n"
        "        a = a + k\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "0 NUM 0\n"
                                "0 OUT a\n"
                                "B0\n"
                                "2 NUM 0\n"
                                "2 NUM 10\n"
                                "1 RUN range\n"
                                "0 RUN iter\n"
                                "0 OUT $l0\n"
                                "B0\n"
                                "0 RUN $l0.__next__\n"
                                "0 OUT i\n"
                                "0 EST i\n"
                                "0 JEZ 2\n"
                                "B1\n"
                                "2 NUM 0\n"
                                "2 NUM 3\n"
                                "1 RUN range\n"
                                "0 RUN iter\n"
                                "0 OUT $l1\n"
                                "B1\n"
                                "0 RUN $l1.__next__\n"
                                "0 OUT k\n"
                                "0 EST k\n"
                                "0 JEZ 2\n"
                                "B2\n"
                                "1 REF k\n"
                                "0 RUN print\n"
                                "B2\n"
                                "1 REF a\n"
                                "1 REF k\n"
                                "0 OPT +\n"
                                "0 OUT a\n"
                                "B1\n"
                                "0 JMP -1\n"
                                "B1\n"
                                "0 DEL $l1\n"
                                "B0\n"
                                "0 JMP -1\n"
                                "B0\n"
                                "0 DEL $l0\n"
                                "B0\n");

    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    byteCodeFrame_print(&bytecode_frame);
    byteCodeFrame_deinit(&bytecode_frame);

    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, break_) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "for i in range(0, 3):\n"
        "    if i = 1:\n"
        "        break\n"
        "        break \n"
        "        continue\n"
        "        continue \n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 0\n"
                 "2 NUM 3\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT $l0\n"
                 "B0\n"
                 "0 RUN $l0.__next__\n"
                 "0 OUT i\n"
                 "0 EST i\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 NUM 1\n"
                 "0 OUT i\n"
                 "0 JEZ 1\n"
                 "B2\n"
                 "0 BRK \n"
                 "B2\n"
                 "0 BRK \n"
                 "B2\n"
                 "0 CTN \n"
                 "B2\n"
                 "0 CTN \n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL $l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, prime_100) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "num = 0\n"
        "i = 2\n"
        "for i in range(2,100):\n"
        "    j=2\n"
        "    is_prime = 1\n"
        "    for j in range(2 , i):\n"
        "        if i%j==0:\n"
        "            is_prime = 0\n"
        "            break\n"
        "    if is_prime:\n"
        "        num = num + i\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 NUM 0\n"
                 "0 OUT num\n"
                 "B0\n"
                 "0 NUM 2\n"
                 "0 OUT i\n"
                 "B0\n"
                 "2 NUM 2\n"
                 "2 NUM 100\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT $l0\n"
                 "B0\n"
                 "0 RUN $l0.__next__\n"
                 "0 OUT i\n"
                 "0 EST i\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 NUM 2\n"
                 "0 OUT j\n"
                 "B1\n"
                 "0 NUM 1\n"
                 "0 OUT is_prime\n"
                 "B1\n"
                 "2 NUM 2\n"
                 "2 REF i\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT $l1\n"
                 "B1\n"
                 "0 RUN $l1.__next__\n"
                 "0 OUT j\n"
                 "0 EST j\n"
                 "0 JEZ 2\n"
                 "B2\n"
                 "2 REF i\n"
                 "2 REF j\n"
                 "1 OPT %\n"
                 "1 NUM 0\n"
                 "0 OPT ==\n"
                 "0 JEZ 1\n"
                 "B3\n"
                 "0 NUM 0\n"
                 "0 OUT is_prime\n"
                 "B3\n"
                 "0 BRK \n"
                 "B1\n"
                 "0 JMP -1\n"
                 "B1\n"
                 "0 DEL $l1\n"
                 "B1\n"
                 "0 REF is_prime\n"
                 "0 JEZ 1\n"
                 "B2\n"
                 "1 REF num\n"
                 "1 REF i\n"
                 "0 OPT +\n"
                 "0 OUT num\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL $l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, __iter__) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "__res = __iter__()\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 RUN __iter__\n"
                 "0 OUT __res\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

/* not solve yet */

// TEST(parser, strEqu) {
//     g_PikaMemInfo.heapUsedMax = 0;
//     Args* buffs = New_strBuff();
//     char* lines =
//                  "a = '1ee' == '1ee'\n"
//                  "b = '1e' == '1ee'\n"
//                  "\n";
//     printf("%s", lines);
//     char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
//     printf("%s", pikaAsm);
//     EXPECT_STREQ(pikaAsm,
//                  "a = ('1ee' == '1ee')\n"
//                  "b = ('1e' == '1ee')\n"
//                  "\n");
//     args_deinit(buffs);
//     EXPECT_EQ(pikaMemNow(), 0);
// }

TEST(parser, for_in_string) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "for b in 'test':\n"
        "    a = 1\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR test\n"
                 "0 RUN iter\n"
                 "0 OUT $l0\n"
                 "B0\n"
                 "0 RUN $l0.__next__\n"
                 "0 OUT b\n"
                 "0 EST b\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 NUM 1\n"
                 "0 OUT a\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL $l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, print_ddd) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "print(\"[Info]: in Python config...\")\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR [Info]: in Python config...\n"
                 "0 RUN print\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, __getitem__3) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = b[c+d] + e[f*j]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 REF b\n"
                 "3 REF c\n"
                 "3 REF d\n"
                 "2 OPT +\n"
                 "1 SLC \n"
                 "2 REF e\n"
                 "3 REF f\n"
                 "3 REF j\n"
                 "2 OPT *\n"
                 "1 SLC \n"
                 "0 OPT +\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, __getitem__) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = b[c]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF b\n"
                 "1 REF c\n"
                 "0 SLC \n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, __getitem__2) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = b[c+d]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF b\n"
                 "2 REF c\n"
                 "2 REF d\n"
                 "1 OPT +\n"
                 "0 SLC \n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, __setitem__) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a[b] = c\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 REF b\n"
                 "1 REF c\n"
                 "0 RUN __setitem__\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, str_p_str) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = str(1) + str(1)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 1\n"
                 "1 RUN str\n"
                 "2 NUM 1\n"
                 "1 RUN str\n"
                 "0 OPT +\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, test__) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "len = __calls.len()\n"
        "mode = 'none'\n"
        "info_index = 0\n"
        "for i in range(0, len):\n"
        "    if len == 0:\n"
        "        break\n"
        "    if info_index == 0:\n"
        "        mode = __calls[i]\n"
        "        info_index = 1\n"
        "    elif info_index == 1:\n"
        "        if mode == 'always':\n"
        "            todo = __calls[i]\n"
        "            todo()\n"
        "            info_index = 0\n"
        "        elif mode == 'when':\n"
        "            when = __calls[i]\n"
        "            info_index = 2\n"
        "        elif mode == 'period_ms':\n"
        "            period_ms = __calls[i]\n"
        "            info_index = 2\n"
        "    elif info_index == 2:\n"
        "        if mode == 'when':\n"
        "            if when():\n"
        "                todo = __calls[i]\n"
        "                todo()\n"
        "            info_index = 0\n"
        "        elif mode == 'period_ms':\n"
        "            todo = __calls[i]\n"
        "            info_index = 3\n"
        "    elif info_index == 3:\n"
        "        if mode == 'period_ms':\n"
        "            if __tick > __calls[i]:\n"
        "                todo()\n"
        "                __calls[i] = __tick + period_ms\n"
        "            info_index = 0\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, global) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def test_global():\n"
        "    global x\n"
        "    global y, z\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "0 DEF test_global()\n"
                                "0 JMP 1\n"
                                "B1\n"
                                "0 GLB x\n"
                                "B1\n"
                                "0 GLB y,z\n"
                                "B1\n"
                                "0 RET \n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, mpy_demo_1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "chars = ' .,-:;i+hHM$*#@ '\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 STR  .,-:;i+hHM$*#@ \n"
                 "0 OUT chars\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, clean_compled_str) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* res = Cursor_getCleanStmt(buffs, "chars = ' .,-:;i+hHM$*#@ '\n");
    EXPECT_STREQ(res, "chars=' .,-:;i+hHM$*#@ '\n");
    printf("%s", res);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, class_) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "class Test(TinyObj):\n"
        "    x = 1\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "0 CLS Test()\n"
                                "0 JMP 1\n"
                                "B1\n"
                                "0 RUN TinyObj\n"
                                "0 OUT self\n"
                                "B1\n"
                                "0 RAS self\n"
                                "B1\n"
                                "0 NUM 1\n"
                                "0 OUT x\n"
                                "B1\n"
                                "0 RAS $origin\n"
                                "B1\n"
                                "0 NEW self\n"
                                "0 RET \n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, class_def) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "class Test():\n"
        "    x = 1\n"
        "    def hello():\n"
        "        print('hello')\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);

    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 CLS Test()\n"
                 "0 JMP 1\n"
                 "B1\n"
                 "0 RUN TinyObj\n"
                 "0 OUT self\n"
                 "B1\n"
                 "0 RAS self\n"
                 "B1\n"
                 "0 NUM 1\n"
                 "0 OUT x\n"
                 "B1\n"
                 "0 DEF hello()\n"
                 "0 JMP 1\n"
                 "B2\n"
                 "1 STR hello\n"
                 "0 RUN print\n"
                 "B2\n"
                 "0 RET \n"
                 "B1\n"
                 "0 RAS $origin\n"
                 "B1\n"
                 "0 NEW self\n"
                 "0 RET \n"
                 "B0\n");

    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    byteCodeFrame_print(&bytecode_frame);
    byteCodeFrame_deinit(&bytecode_frame);

    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, nag_a) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "print(-a)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(asmer, asmer_to_instructUnit) {
    char* asm_line =
        "B2\n"
        "2 NUM 2\n"
        "2 NUM 3\n"
        "1 RUN add\n"
        "0 RUN test.on\n";
    Args buffs = {0};
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, asm_line);
    byteCodeFrame_print(&bytecode_frame);
    InstructUnit* ins_unit =
        instructArray_getByOffset(&(bytecode_frame.instruct_array), 4);
    instructUnit_print(ins_unit);
    size_t byteCode_size = byteCodeFrame_getSize(&bytecode_frame);
    uint16_t offset_out =
        constPool_getOffsetByData(&(bytecode_frame.const_pool), "add");
    char* data_out =
        constPool_getByOffset(&(bytecode_frame.const_pool), offset_out);

    /* assert */
    EXPECT_STREQ(data_out, "add");
    EXPECT_EQ(byteCode_size, 33);
    EXPECT_STREQ(constPool_getNext(&(bytecode_frame.const_pool)), "2");
    EXPECT_STREQ(constPool_getNext(&(bytecode_frame.const_pool)), "3");
    EXPECT_STREQ(constPool_getNext(&(bytecode_frame.const_pool)), "add");
    EXPECT_STREQ(constPool_getNext(&(bytecode_frame.const_pool)), "test.on");
    EXPECT_EQ((uintptr_t)constPool_getNext(&(bytecode_frame.const_pool)),
              (uintptr_t)NULL);
    /* deinit */
    byteCodeFrame_deinit(&bytecode_frame);
    strsDeinit(&buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(asmer, asm_to_bytecodeArray) {
    char* pikaAsm =
        "B0\n"
        "0 RUN __init__\n";

    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    byteCodeFrame_printAsArray(&bytecode_frame);
    byteCodeFrame_deinit(&bytecode_frame);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(asmer, asm_to_bytecode_0x0d) {
    char* pikaAsm =
        "B0\n"
        "0 RUN __init__\n";
    char* pikaAsm1 =
        "B0\r\n"
        "0 RUN __init__\r\n";

    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    byteCodeFrame_printAsArray(&bytecode_frame);

    ByteCodeFrame bytecode_frame1;
    byteCodeFrame_init(&bytecode_frame1);
    byteCodeFrame_appendFromAsm(&bytecode_frame1, pikaAsm1);
    byteCodeFrame_printAsArray(&bytecode_frame1);

    EXPECT_EQ(bytecode_frame1.const_pool.size, bytecode_frame.const_pool.size);
    EXPECT_EQ(bytecode_frame1.instruct_array.size,
              bytecode_frame.instruct_array.size);

    /* deinit */
    byteCodeFrame_deinit(&bytecode_frame1);
    byteCodeFrame_deinit(&bytecode_frame);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, list_1_2) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "print(list[0] + list[1])\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "3 REF list\n"
                 "3 NUM 0\n"
                 "2 SLC \n"
                 "3 REF list\n"
                 "3 NUM 1\n"
                 "2 SLC \n"
                 "1 OPT +\n"
                 "0 RUN print\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, class_def_void_line) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "class Test():\n"
        "    x = 1\n"
        "\n"
        "    def hello():\n"
        "#test\n"
        "        print('hello')\n"
        "    \n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);

    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 CLS Test()\n"
                 "0 JMP 1\n"
                 "B1\n"
                 "0 RUN TinyObj\n"
                 "0 OUT self\n"
                 "B1\n"
                 "0 RAS self\n"
                 "B1\n"
                 "0 NUM 1\n"
                 "0 OUT x\n"
                 "B1\n"
                 "0 DEF hello()\n"
                 "0 JMP 1\n"
                 "B2\n"
                 "1 STR hello\n"
                 "0 RUN print\n"
                 "B2\n"
                 "0 RET \n"
                 "B1\n"
                 "0 RAS $origin\n"
                 "B1\n"
                 "0 NEW self\n"
                 "0 RET \n"
                 "B0\n");

    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    byteCodeFrame_print(&bytecode_frame);
    byteCodeFrame_deinit(&bytecode_frame);

    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_IMPORT_EX_ENABLE
TEST(parser, multiLine_import) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char *lines = (char *)"import TEE\n"
                        "from EE import C\n"
                        "from PikaStdLib import MemChecker as MC\n"
                        "while true:\n"
                        "    rgb.flow()\n"
                        "    if false:\n"
                        "        a=3\n"
                        "        test.on(add(2,3))\n"
                        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 IMP TEE\n"
                 "B0\n"
                 "0 IMP EE\n"
                 "B0\n"
                 "0 REF EE.C\n"
                 "0 OUT C\n"
                 "B0\n"
                 "0 IMP PikaStdLib\n"
                 "B0\n"
                 "0 REF PikaStdLib.MemChecker\n"
                 "0 OUT MC\n"
                 "B0\n"
                 "0 REF true\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 RUN rgb.flow\n"
                 "B1\n"
                 "0 REF false\n"
                 "0 JEZ 1\n"
                 "B2\n"
                 "0 NUM 3\n"
                 "0 OUT a\n"
                 "B2\n"
                 "2 NUM 2\n"
                 "2 NUM 3\n"
                 "1 RUN add\n"
                 "0 RUN test.on\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, multiLine_comment) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char *lines = (char *)"'''\n"
                        "a = 1\n"
                        "'''\n"
                        "while true:\n"
                        "    rgb.flow()\n"
                        "    ''' \n"
                        "    a = 1\n"
                        "    ''' \n"
                        "    if false:\n"
                        "    \"\"\" \n"
                        "    a = 1\n"
                        "    a = 1\n"
                        "    \"\"\"\n"
                        "        a=3\n"
                        "        test.on(add(2,3))\n"
                        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 REF true\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 RUN rgb.flow\n"
                 "B1\n"
                 "0 REF false\n"
                 "0 JEZ 1\n"
                 "B2\n"
                 "0 NUM 3\n"
                 "0 OUT a\n"
                 "B2\n"
                 "2 NUM 2\n"
                 "2 NUM 3\n"
                 "1 RUN add\n"
                 "0 RUN test.on\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, plus_equ) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a += 1+1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "3 NUM 1\n"
                 "3 NUM 1\n"
                 "2 OPT +\n"
                 "1 RUN \n"
                 "0 OPT +\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, a_j) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, "a=");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream, "{sym}a{opt}=");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, plus_equ_) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a -= (1+1-3)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "5 NUM 1\n"
                 "5 NUM 1\n"
                 "4 OPT +\n"
                 "4 NUM 3\n"
                 "3 OPT -\n"
                 "2 RUN \n"
                 "1 RUN \n"
                 "0 OPT -\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, class_demo_3) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "class people:\n"
        "    def speak(self):\n"
        "        print('i am a people')\n"
        " \n"
        "class student(people):\n"
        "    def speak(self):\n"
        "        print('i am a student')\n"
        " \n"
        "p = people()\n"
        "s = student()\n"
        "p.speak()\n"
        "s.speak()\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, a_a) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "a = 1\n"
        "a\n"
        "a\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, a_cuohao_j) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = (3 - 4) - 4\n";
    printf("%s\n",
           Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines)));
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "3 NUM 3\n"
                 "3 NUM 4\n"
                 "2 OPT -\n"
                 "1 RUN \n"
                 "1 NUM 4\n"
                 "0 OPT -\n"
                 "0 OUT a\n"
                 "B0\n");
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, _3_3) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "-3+3\n";
    printf("%s\n",
           Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines)));
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 3\n"
                 "1 OPT -\n"
                 "1 NUM 3\n"
                 "0 OPT +\n"
                 "B0\n");
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_BUILTIN_STRUCT_ENABLE
TEST(parser, list_init) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = [1, 2, 3]\n";
    printf("%s\n",
           Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines)));
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 NUM 1\n"
                 "1 NUM 2\n"
                 "1 NUM 3\n"
                 "0 LST \n"
                 "0 OUT a\n"
                 "B0\n");
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, list_init_fun) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "test([1, 2, 3])\n";
    printf("%s\n",
           Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines)));
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 1\n"
                 "2 NUM 2\n"
                 "2 NUM 3\n"
                 "1 LST \n"
                 "0 RUN test\n"
                 "B0\n");
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, bytes_iteral) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = b'\\x00\\x01'\n";
    char* tokens_str =
        Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines));
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 BYT \\x00\\x01\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_IMPORT_EX_ENABLE
TEST(parser, import_as) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "import PikaStdLib as std\n";
    char* tokens_str =
        Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines));
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 IMP PikaStdLib\n"
                 "B0\n"
                 "0 REF PikaStdLib\n"
                 "0 OUT std\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, str_equ) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = 'num ='\n";
    char* tokens_str =
        Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines));
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 STR num =\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, bytes_index) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "res2 = b'eqrt'[2]\n";
    char* tokens_str =
        Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines));
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 BYT eqrt\n"
                 "1 NUM 2\n"
                 "0 SLC \n"
                 "0 OUT res2\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, hex_iteral) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = 0b10\n";
    char* tokens_str =
        Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines));
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 NUM 0b10\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, tab) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "for i in range(0, 100):\n"
        "\tprint(i)\n"
        "\n";
    char* tokens_str =
        Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines));
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 0\n"
                 "2 NUM 100\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT $l0\n"
                 "B0\n"
                 "0 RUN $l0.__next__\n"
                 "0 OUT i\n"
                 "0 EST i\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "1 REF i\n"
                 "0 RUN print\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL $l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, parse_issue2) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "    recv_buf[1] = dat \n";
    char* tokens = Lexer_getTokenStream(buffs, lines);
    uint16_t token_size = TokenStream_getSize(tokens);
    EXPECT_EQ(token_size, 8);
    char* tokens_str = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B1\n"
                 "1 REF recv_buf\n"
                 "1 NUM 1\n"
                 "1 REF dat\n"
                 "0 RUN __setitem__\n"
                 "0 OUT recv_buf\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, parse_issue3) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "recv_buf[1] = dat ";
    char* clean_cmd = Cursor_getCleanStmt(buffs, lines);
    EXPECT_STREQ(clean_cmd, "recv_buf[1]=dat");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, slice1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = recv_buf[1:4]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF recv_buf\n"
                 "1 NUM 1\n"
                 "1 NUM 4\n"
                 "0 SLC \n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, slice2) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = recv_buf[1:4:2]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF recv_buf\n"
                 "1 NUM 1\n"
                 "1 NUM 4\n"
                 "1 NUM 2\n"
                 "0 SLC \n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, str_add1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "msg = \"device_names[\" + str(i) + \"]:\"";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    char* tokens_print =
        Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines));
    printf("%s\r\n", tokens_print);
    EXPECT_STREQ(tokens_print,
                 "{sym}msg{opt}={lit}\"device_names[\"{opt}+{sym}str{dvd}({sym}"
                 "i{dvd}){opt}+{lit}\"]:\"");
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "2 STR device_names[\n"
                                "3 REF i\n"
                                "2 RUN str\n"
                                "1 OPT +\n"
                                "1 STR ]:\n"
                                "0 OPT +\n"
                                "0 OUT msg\n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, str_add2) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "msg = \"device_names[\" + str(i)";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    char* tokens_print =
        Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines));
    printf("%s\r\n", tokens_print);
    EXPECT_STREQ(tokens_print,
                 "{sym}msg{opt}={lit}\"device_names[\"{opt}+{sym}str{dvd}({sym}"
                 "i{dvd})");
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)"B0\n"
                                "1 STR device_names[\n"
                                "2 REF i\n"
                                "1 RUN str\n"
                                "0 OPT +\n"
                                "0 OUT msg\n"
                                "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, mpy1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "# Pong!\n"
        "# Using emulated hardware i2c, we can push enough frames for\n"
        "# rough animations. Performance for this project is reduced\n"
        "# using chromium.\n"
        "\n"
        "import machine\n"
        "import framebuf\n"
        "import time\n"
        "import pyb\n"
        "\n"
        "SCREEN_WIDTH = 64\n"
        "SCREEN_HEIGHT = 32\n"
        "\n"
        "game_over = False\n"
        "score = 0\n"
        "\n"
        "class Entity:\n"
        "    def __init__(self, x, y, w, h, vx, vy):\n"
        "        self.x = x\n"
        "        self.y = y\n"
        "        self.w = w\n"
        "        self.h = h\n"
        "        self.vx = vx\n"
        "        self.vy = vy\n"
        "\n"
        "    def draw(self, fbuf):\n"
        "        fbuf.fill_rect(int(self.x), int(self.y), self.w, self.h, 1)\n"
        "\n"
        "class Ball(Entity):\n"
        "    def update(self, dt, player):\n"
        "        self.x += self.vx * dt\n"
        "        if (self.x <= 0):\n"
        "            self.x = 0\n"
        "            self.vx = -self.vx\n"
        "        if (self.x >= SCREEN_WIDTH - self.w):\n"
        "            self.x = SCREEN_WIDTH - self.w\n"
        "            self.vx = -self.vx\n"
        "        self.y += self.vy * dt\n"
        "        if (self.y <= 0):\n"
        "            self.y = 0\n"
        "            self.vy = -self.vy\n"
        "        if (self.y >= SCREEN_HEIGHT - self.h - player.h):\n"
        "            if (self.x >= player.x and self.x <= player.x + "
        "player.w):\n"
        "                self.y = SCREEN_HEIGHT - self.h - player.h\n"
        "                self.vy = -self.vy\n"
        "                global score\n"
        "                score += 1\n"
        "                if score % 2 == 0:\n"
        "                    self.vx += (self.vx/abs(self.vx)) * 1\n"
        "                if score % 3 == 0:\n"
        "                    self.vy += (self.vy/abs(self.vy)) * 1\n"
        "            else:\n"
        "                global game_over\n"
        "                game_over = True\n"
        "\n"
        "class Player(Entity):\n"
        "    pass\n"
        "\n"
        "ball = Ball(32, 16, 1, 1, 2, -2)\n"
        "player = Player(30, 31, 10, 1, 0, 0)\n"
        "\n"
        "y4 = machine.Pin('Y4')\n"
        "adc = pyb.ADC(y4)\n"
        "i2c = machine.I2C('X')\n"
        "fbuf = framebuf.FrameBuffer(bytearray(64 * 32 // 8), 64, 32, "
        "framebuf.MONO_HLSB)\n"
        "tick = time.ticks_ms()\n"
        "\n"
        "while not game_over:\n"
        "    ntick = time.ticks_ms()\n"
        "    ball.update(time.ticks_diff(ntick, tick) // 100, player)\n"
        "    tick = ntick\n"
        "    player.x = adc.read() * 58 / 255\n"
        "    fbuf.fill(0)\n"
        "    ball.draw(fbuf)\n"
        "    player.draw(fbuf)\n"
        "    i2c.writeto(8, fbuf)\n"
        "    time.sleep_ms(50) # Adjust this for performance boosts\n"
        "\n"
        "fbuf.fill(0)\n"
        "fbuf.text('GAME', 15, 8)\n"
        "fbuf.text('OVER', 15, 18)\n"
        "i2c.writeto(8, fbuf)\n"
        "\n"
        "print('Score: ', score)\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, slice_12lkj) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = b[:6]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF b\n"
                 "1 NUM 0\n"
                 "1 NUM 6\n"
                 "0 SLC \n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, slice_oifjlk) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = b[6:]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF b\n"
                 "1 NUM 6\n"
                 "2 NUM 99999\n"
                 "1 OPT -\n"
                 "0 SLC \n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, str_string) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = str(String('test'))\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 STR test\n"
                 "1 RUN String\n"
                 "0 RUN str\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, json_literal) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "a = '"
        "{"
        "\"name\": \"mculover666\","
        "\"age\": 22,"
        "\"weight\": 55.5,"
        "\"address\":"
        "{"
        "    \"country\": \"China\","
        "    \"zip-code\": 111111"
        "},"
        "\"skill\": [\"c\", \"Java\", \"Python\"],"
        "\"student\": false"
        "}'";
    printf("%s\r\n", lines);
    char* tokens_print =
        Lexer_printTokenStream(buffs, Lexer_getTokenStream(buffs, lines));
    printf("%s\r\n", tokens_print);

    EXPECT_STREQ(tokens_print,
                 "{sym}a{opt}={lit}'{\"name\": \"mculover666\",\"age\": "
                 "22,\"weight\": 55.5,\"address\":{    \"country\": \"China\", "
                 "   \"zip-code\": 111111},\"skill\": [\"c\", \"Java\", "
                 "\"Python\"],\"student\": false}'");

    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(
        pikaAsm,
        "B0\n"
        "0 STR {\"name\": \"mculover666\",\"age\": 22,\"weight\": "
        "55.5,\"address\":{    \"country\": \"China\",    \"zip-code\": "
        "111111},\"skill\": [\"c\", \"Java\", \"Python\"],\"student\": false}\n"
        "0 OUT a\n"
        "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, issuekd) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "recv_buf = PikaStdData.List()\n"
        "RECV_MAX_SIZE=128\n"
        "iteri = 0\n"
        "for i in range(0, int(RECV_MAX_SIZE)):\n"
        "    recv_buf.append(0)\n"
        "    iteri += 1\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 RUN PikaStdData.List\n"
                 "0 OUT recv_buf\n"
                 "B0\n"
                 "0 NUM 128\n"
                 "0 OUT RECV_MAX_SIZE\n"
                 "B0\n"
                 "0 NUM 0\n"
                 "0 OUT iteri\n"
                 "B0\n"
                 "2 NUM 0\n"
                 "3 REF RECV_MAX_SIZE\n"
                 "2 RUN int\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT $l0\n"
                 "B0\n"
                 "0 RUN $l0.__next__\n"
                 "0 OUT i\n"
                 "0 EST i\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "1 NUM 0\n"
                 "0 RUN recv_buf.append\n"
                 "B1\n"
                 "1 REF iteri\n"
                 "2 NUM 1\n"
                 "1 RUN \n"
                 "0 OPT +\n"
                 "0 OUT iteri\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL $l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, cjson_test4) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    Arg* lines_buff = arg_loadFile(NULL, "../../examples/cJSON/test4.py");
    char* lines = (char*)arg_getBytes(lines_buff);
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n0 IMP pika_cjson\nB0\n0 STR "
                 "{\"data\":{\"requestSocialInsuranceFromYangCheng\":\"\","
                 "\"authenticationComparison\":\"no\",\"startupLogo\":\"4\","
                 "\"cardType\":\"00,01,02,03,04\",\"synfromhis\":\"no\","
                 "\"alarmThresholdValue\":\"37.2\",\"hospitalName\":\"jell\","
                 "\"facediscernMode\":\"01\",\"hospitalCode\":\"102\"},"
                 "\"success\":true,\"resultCode\":\"0000\",\"time\":\"2022-05-"
                 "20 14:10:27\",\"message\":\"ok\"}\n0 OUT data1\nB0\n1 REF "
                 "data1\n0 RUN pika_cjson.Parse\n0 OUT a\nB0\n0 RUN a.print\n"
                 "B0\n");
    args_deinit(buffs);
    arg_deinit(lines_buff);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, connection) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "print('\\\n"
        "test')\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR test\n"
                 "0 RUN print\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, connection2) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "a = \\\n"
        "3\n"
        "print\\\n"
        "(a)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 NUM 3\n"
                 "0 OUT a\n"
                 "B0\n"
                 "1 REF a\n"
                 "0 RUN print\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_FORMAT_ENABLE
TEST(parser, format1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "s = 'res:%d' % 23";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR res:%d\n"
                 "1 NUM 23\n"
                 "0 RUN cformat\n"
                 "0 OUT s\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_FORMAT_ENABLE
TEST(parser, format2) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "'res:%d:%d' % (23, 25)";
    printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR res:%d:%d\n"
                 "1 NUM 23\n"
                 "1 NUM 25\n"
                 "0 RUN cformat\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_EXCEPTION_ENABLE
TEST(parser, try1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "try:\n"
        "    a = 1\n"
        "    raise 0x25 + 256\n"
        "    raise\n"
        "except:\n"
        "    print('in except')\n"
        "\n";
    printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 TRY \n"
                 "B1\n"
                 "0 NUM 1\n"
                 "0 OUT a\n"
                 "B1\n"
                 "1 NUM 0x25\n"
                 "1 NUM 256\n"
                 "0 OPT +\n"
                 "0 RIS \n"
                 "B1\n"
                 "0 REF RuntimeError\n"
                 "0 RIS \n"
                 "B0\n"
                 "0 NTR \n"
                 "0 GER \n"
                 "0 JEZ 2\n"
                 "B0\n"
                 "0 EXP \n"
                 "B1\n"
                 "1 STR in except\n"
                 "0 RUN print\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, except_issue) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "try:\n"
        "    a = 1\n"
        "    raise 0x25 + 256\n"
        "    raise\n"
        "except Excepthon:\n"
        "    print('in except')\n"
        "    break\n"
        "\n";
    printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 TRY \n"
                 "B1\n"
                 "0 NUM 1\n"
                 "0 OUT a\n"
                 "B1\n"
                 "1 NUM 0x25\n"
                 "1 NUM 256\n"
                 "0 OPT +\n"
                 "0 RIS \n"
                 "B1\n"
                 "0 REF RuntimeError\n"
                 "0 RIS \n"
                 "B0\n"
                 "0 NTR \n"
                 "0 GER \n"
                 "0 JEZ 2\n"
                 "B0\n"
                 "0 EXP \n"
                 "B1\n"
                 "1 STR in except\n"
                 "0 RUN print\n"
                 "B1\n"
                 "0 BRK \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif

TEST(parser, optissue1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "~-1";
    printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 1\n"
                 "1 OPT -\n"
                 "0 OPT ~\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, optissue2) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "test(not get())";
    printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 RUN get\n"
                 "1 OPT  not \n"
                 "0 RUN test\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, import_issue1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, "my_import = import_test");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream, "{sym}my_import{opt}={sym}import_test");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, dict_literal1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(
        buffs,
        "tinydict = {'name': 'runoob', 'likes': 123, 'url': 'www.runoob.com'}");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream,
                 "{sym}tinydict{opt}={dvd}{{lit}'name'{dvd}:{lit}'runoob'{dvd},"
                 "{lit}'likes'{dvd}:{lit}123{dvd},{lit}'url'{dvd}:{lit}'www."
                 "runoob.com'{dvd}}");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_BUILTIN_STRUCT_ENABLE
TEST(parser, dict_literal1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "tinydict = {'name': 'runoob', 'likes': 123, 'url': "
        "'www.runoob.com'}";
    printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR name\n"
                 "1 STR runoob\n"
                 "1 STR likes\n"
                 "1 NUM 123\n"
                 "1 STR url\n"
                 "1 STR www.runoob.com\n"
                 "0 DCT \n"
                 "0 OUT tinydict\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, common_issue1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "class ConfigParser():\n"
        "    def options(self):\n"
        "        # print(type(self.content)\n"
        "        # print(self.content.split('['))\n"
        "        print('test')\n"
        "\n";
    printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 CLS ConfigParser()\n"
                 "0 JMP 1\n"
                 "B1\n"
                 "0 RUN TinyObj\n"
                 "0 OUT self\n"
                 "B1\n"
                 "0 RAS self\n"
                 "B1\n"
                 "0 DEF options(self)\n"
                 "0 JMP 1\n"
                 "B2\n"
                 "1 STR test\n"
                 "0 RUN print\n"
                 "B2\n"
                 "0 RET \n"
                 "B1\n"
                 "0 RAS $origin\n"
                 "B1\n"
                 "0 NEW self\n"
                 "0 RET \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, def_issue1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def options(self):\n"
        "    ...\n"
        "\n";
    printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 DEF options(self)\n"
                 "0 JMP 1\n"
                 "B1\n"
                 "B1\n"
                 "0 RET \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, configparser) {
    Args buffs = {0};
    Parser_fileToAsm(&buffs, "package/pikascript/configparser.py");
    strsDeinit(&buffs);
}

TEST(parser, unittest) {
    Args buffs = {0};
    char* ASM = Parser_fileToAsm(&buffs, "package/pikascript/unittest.py");
    printf("%s\r\n", ASM);
    strsDeinit(&buffs);
}

TEST(lexser, function_chain) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, "String('a,b,c').split(',')");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(
        printTokenStream,
        "{sym}String{dvd}({lit}'a,b,c'{dvd}){sym}.split{dvd}({lit}','{dvd})");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, function_chain) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = String('a,b,c').split(',')\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 STR a,b,c\n"
                 "1 RUN String\n"
                 "1 STR ,\n"
                 "0 RUN .split\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, str_issue1) {
    char* lines =
        "if str(type(data)) == \"<class 'str'>\" and str(type(included_data)) "
        "== \"<class 'str'>\":\n"
        "\n";
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "4 REF data\n"
                 "3 RUN type\n"
                 "2 RUN str\n"
                 "2 STR <class 'str'>\n"
                 "1 OPT ==\n"
                 "4 REF included_data\n"
                 "3 RUN type\n"
                 "2 RUN str\n"
                 "2 STR <class 'str'>\n"
                 "1 OPT ==\n"
                 "0 OPT  and \n"
                 "0 JEZ 1\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_FORMAT_ENABLE
TEST(parser, str_issue2) {
    char* lines = "print('ret = %s' % str(ret))\n";
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 STR ret = %s\n"
                 "3 REF ret\n"
                 "2 RUN str\n"
                 "1 RUN cformat\n"
                 "0 RUN print\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, num_issue) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "(((1 + (2 * 3)/(4 + 5))*(6 - 7) + (8 + 9) * 10)/11) - 12\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "8 NUM 1\n"
                 "11 NUM 2\n"
                 "11 NUM 3\n"
                 "10 OPT *\n"
                 "9 RUN \n"
                 "11 NUM 4\n"
                 "11 NUM 5\n"
                 "10 OPT +\n"
                 "9 RUN \n"
                 "8 OPT /\n"
                 "7 OPT +\n"
                 "6 RUN \n"
                 "8 NUM 6\n"
                 "8 NUM 7\n"
                 "7 OPT -\n"
                 "6 RUN \n"
                 "5 OPT *\n"
                 "8 NUM 8\n"
                 "8 NUM 9\n"
                 "7 OPT +\n"
                 "6 RUN \n"
                 "6 NUM 10\n"
                 "5 OPT *\n"
                 "4 OPT +\n"
                 "3 RUN \n"
                 "3 NUM 11\n"
                 "2 OPT /\n"
                 "1 RUN \n"
                 "1 NUM 12\n"
                 "0 OPT -\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, branket_issue2) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "temp = hex(int('12'))[0:2]\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "3 STR 12\n"
                 "2 RUN int\n"
                 "1 RUN hex\n"
                 "1 NUM 0\n"
                 "1 NUM 2\n"
                 "0 SLC \n"
                 "0 OUT temp\n"
                 "B0\n");
    __platform_printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, branket_issue3) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = b[x][y]\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 REF b\n"
                 "2 REF x\n"
                 "1 SLC \n"
                 "1 REF y\n"
                 "0 SLC \n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, branket_issue4) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = b[c[y]]\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF b\n"
                 "2 REF c\n"
                 "2 REF y\n"
                 "1 SLC \n"
                 "0 SLC \n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, tuple1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "(a,b)\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 REF b\n"
                 "0 RUN \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, _del) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "del a\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 DEL a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, _del_issue1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "del(a)\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 DEL a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(parser, issue_fa13f4) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "d['language'].append('Java')\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 REF d\n"
                 "2 STR language\n"
                 "1 SLC \n"
                 "1 STR Java\n"
                 "0 RUN .append\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, _is) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a is b\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 REF b\n"
                 "0 OPT  is \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, _in) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a in b\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 REF b\n"
                 "0 OPT  in \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, _in2) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "res1 = 'a' in 'aaa'\n"
        "res2 = 'aaa' in 'a'\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR a\n"
                 "1 STR aaa\n"
                 "0 OPT  in \n"
                 "0 OUT res1\n"
                 "B0\n"
                 "1 STR aaa\n"
                 "1 STR a\n"
                 "0 OPT  in \n"
                 "0 OUT res2\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_EXCEPTION_ENABLE
TEST(parser, assert_) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "assert True\n"
        "assert 1 == 1, 'testparser'\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF True\n"
                 "0 ASS \n"
                 "B0\n"
                 "2 NUM 1\n"
                 "2 NUM 1\n"
                 "1 OPT ==\n"
                 "1 STR testparser\n"
                 "0 ASS \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_EXCEPTION_ENABLE
TEST(parser, except_for) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "for i in range(0, 10):\n"
        "    try:\n"
        "        a\n"
        "    except:\n"
        "        b";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 0\n"
                 "2 NUM 10\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT $l0\n"
                 "B0\n"
                 "0 RUN $l0.__next__\n"
                 "0 OUT i\n"
                 "0 EST i\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 TRY \n"
                 "B2\n"
                 "0 REF a\n"
                 "B1\n"
                 "0 NTR \n"
                 "0 GER \n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 EXP \n"
                 "B2\n"
                 "0 REF b\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL $l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, line_void_issue$l1k2i) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "mem = PikaStdLib.MemChecker()";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 RUN PikaStdLib.MemChecker\n"
                 "0 OUT mem\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, while_void_novoid) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "while True:\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 REF True\n"
                 "0 JEZ 2\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n");
    {
        char* lines = "while True:";
        __platform_printf("%s\n", lines);
        char* pikaAsm = Parser_linesToAsm(buffs, lines);
        __platform_printf("%s", pikaAsm);
        EXPECT_STREQ(pikaAsm,
                     "B0\n"
                     "0 REF True\n"
                     "0 JEZ 2\n"
                     "B0\n"
                     "0 JMP -1\n"
                     "B0\n");
    }

    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, connect_auto) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "method(a,\n"
        "        b)";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 REF b\n"
                 "0 RUN method\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, connet_part1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, "method(a,");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream, "{sym}method{dvd}({sym}a{dvd},");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, vars_runtime) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def testvars(a, *b):\n"
        "    sum = 0\n"
        "    for i in b:\n"
        "        sum += i\n"
        "    return a * sum";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 DEF testvars(a,*b)\n"
                 "0 JMP 1\n"
                 "B1\n"
                 "0 NUM 0\n"
                 "0 OUT sum\n"
                 "B1\n"
                 "1 REF b\n"
                 "0 RUN iter\n"
                 "0 OUT $l1\n"
                 "B1\n"
                 "0 RUN $l1.__next__\n"
                 "0 OUT i\n"
                 "0 EST i\n"
                 "0 JEZ 2\n"
                 "B2\n"
                 "1 REF sum\n"
                 "2 REF i\n"
                 "1 RUN \n"
                 "0 OPT +\n"
                 "0 OUT sum\n"
                 "B1\n"
                 "0 JMP -1\n"
                 "B1\n"
                 "0 DEL $l1\n"
                 "B1\n"
                 "1 REF a\n"
                 "1 REF sum\n"
                 "0 OPT *\n"
                 "0 RET \n"
                 "B1\n"
                 "0 RET \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_BUILTIN_STRUCT_ENABLE
TEST(parser, issues_I5MIFO) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "[1, 2, 3] + [4, 5, 6]";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 1\n"
                 "2 NUM 2\n"
                 "2 NUM 3\n"
                 "1 LST \n"
                 "2 NUM 4\n"
                 "2 NUM 5\n"
                 "2 NUM 6\n"
                 "1 LST \n"
                 "0 OPT +\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(lexser, science_num) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens =
        Lexer_getTokenStream(buffs, "1.0e-2, 10e2, 0.1e-2, aie2, aie-2");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream,
                 "{lit}1.0e-2{dvd},{lit}10e2{dvd},{lit}0.1e-2{dvd},{sym}aie2{"
                 "dvd},{sym}aie{opt}-{lit}2");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, issues_I5OJQB) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokenStream(buffs, "s = '\\\\'");
    char* printTokenStream = Lexer_printTokenStream(buffs, tokens);
    printf("%s\n", printTokenStream);

    /* assert */
    EXPECT_STREQ(printTokenStream, "{sym}s{opt}={lit}'\\\\'");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, issues_I5OJQB) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "s = '\\\\'";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 STR \\\\\n"
                 "0 OUT s\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, keyword1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "test(a=1, b ='test')";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 NUM 1\n"
                 "1 OUT a\n"
                 "1 STR test\n"
                 "1 OUT b\n"
                 "0 RUN test\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, keyword_class) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "t = Test(0, b = 3)\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 NUM 0\n"
                 "1 NUM 3\n"
                 "1 OUT b\n"
                 "0 RUN Test\n"
                 "0 OUT t\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, keyword_class_mqtt) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "class MQTT:\n"
        "    def __init__(self,\n"
        "                 ip: str,\n"
        "                 port=1883,\n"
        "                 clinetID='mac',\n"
        "                 username='',\n"
        "                 password='',\n"
        "                 version='3.1.1',\n"
        "                 ca='',\n"
        "                 keepalive=60):\n"
        "        print('MQTT init')\n"
        "        print(port)\n"
        "        print('ip:', ip)\n"
        "c = MQTT('test')\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(parser, except_dict) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "dict = {'a':1, 'b':2, 'c':3}\n"
        "try: \n"
        "    print(dict['a'])\n"
        "    print(dict['d'])\n"
        "    print(dict['b'])\n"
        "except :\n"
        "    print('in except')\n";

    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR a\n"
                 "1 NUM 1\n"
                 "1 STR b\n"
                 "1 NUM 2\n"
                 "1 STR c\n"
                 "1 NUM 3\n"
                 "0 DCT \n"
                 "0 OUT dict\n"
                 "B0\n"
                 "0 TRY \n"
                 "B1\n"
                 "2 REF dict\n"
                 "2 STR a\n"
                 "1 SLC \n"
                 "0 RUN print\n"
                 "B1\n"
                 "2 REF dict\n"
                 "2 STR d\n"
                 "1 SLC \n"
                 "0 RUN print\n"
                 "B1\n"
                 "2 REF dict\n"
                 "2 STR b\n"
                 "1 SLC \n"
                 "0 RUN print\n"
                 "B0\n"
                 "0 NTR \n"
                 "0 GER \n"
                 "0 JEZ 2\n"
                 "B0\n"
                 "0 EXP \n"
                 "B1\n"
                 "1 STR in except\n"
                 "0 RUN print\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(parser, default_fn_1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def test(a=1, b='test'):\n"
        "    print(a)";

    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 DEF test(a=,b=)\n"
                 "0 JMP 1\n"
                 "B1\n"
                 "0 EST a\n"
                 "0 JNZ 2\n"
                 "B1\n"
                 "0 NUM 1\n"
                 "0 OUT a\n"
                 "B1\n"
                 "0 EST b\n"
                 "0 JNZ 2\n"
                 "B1\n"
                 "0 STR test\n"
                 "0 OUT b\n"
                 "B1\n"
                 "1 REF a\n"
                 "0 RUN print\n"
                 "B1\n"
                 "0 RET \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, default_fn_1_hint) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def test(a:int =1, b:str ='test'):\n"
        "    print(a)";

    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 DEF test(a=,b=)\n"
                 "0 JMP 1\n"
                 "B1\n"
                 "0 EST a\n"
                 "0 JNZ 2\n"
                 "B1\n"
                 "0 NUM 1\n"
                 "0 OUT a\n"
                 "B1\n"
                 "0 EST b\n"
                 "0 JNZ 2\n"
                 "B1\n"
                 "0 STR test\n"
                 "0 OUT b\n"
                 "B1\n"
                 "1 REF a\n"
                 "0 RUN print\n"
                 "B1\n"
                 "0 RET \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(parser, multi_return) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a , b = c";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 REF c\n"
                 "0 OUT $tmp\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 0\n"
                 "0 SLC \n"
                 "0 OUT a\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 1\n"
                 "0 SLC \n"
                 "0 OUT b\n"
                 "B0\n"
                 "0 DEL $tmp\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, multi_return_) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "test['a , b'] = c";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF test\n"
                 "1 STR a , b\n"
                 "1 REF c\n"
                 "0 RUN __setitem__\n"
                 "0 OUT test\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, multi_return_2) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "'test,q'[0] = c";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR test,q\n"
                 "1 NUM 0\n"
                 "1 REF c\n"
                 "0 RUN __setitem__\n"
                 "0 OUT 'test,q'\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(parser, multi_return_3) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "return a,b";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 REF b\n"
                 "0 RUN \n"
                 "0 RET \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, multi_return_4) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "for a,b in c:\n"
        "    print(a,b)";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, for_multi) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "d = {'a':1, 'b':2}\n"
        "for k, v in d.items():\n"
        "    print(k, v)\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR a\n"
                 "1 NUM 1\n"
                 "1 STR b\n"
                 "1 NUM 2\n"
                 "0 DCT \n"
                 "0 OUT d\n"
                 "B0\n"
                 "1 RUN d.items\n"
                 "0 RUN iter\n"
                 "0 OUT $l0\n"
                 "B0\n"
                 "0 RUN $l0.__next__\n"
                 "0 OUT $tmp\n"
                 "0 EST $tmp\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "1 REF $tmp\n"
                 "1 NUM 0\n"
                 "0 SLC \n"
                 "0 OUT k\n"
                 "B1\n"
                 "1 REF $tmp\n"
                 "1 NUM 1\n"
                 "0 SLC \n"
                 "0 OUT v\n"
                 "B1\n"
                 "0 DEL $tmp\n"
                 "B1\n"
                 "1 REF k\n"
                 "1 REF v\n"
                 "0 RUN print\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL $l0\n"
                 "B0\n"

    );

    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, pass_) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "pass\n"
        "def testpass():\n"
        "    pass\n"
        "for i in range(10):\n"
        "    pass\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "B0\n"
                 "0 DEF testpass()\n"
                 "0 JMP 1\n"
                 "B1\n"
                 "B1\n"
                 "0 RET \n"
                 "B0\n"
                 "2 NUM 10\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT $l0\n"
                 "B0\n"
                 "0 RUN $l0.__next__\n"
                 "0 OUT i\n"
                 "0 EST i\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL $l0\n"
                 "B0\n");

    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, modbus_1) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "dest[i + 1] * 256\n";
    __platform_printf("%s\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 REF dest\n"
                 "3 REF i\n"
                 "3 NUM 1\n"
                 "2 OPT +\n"
                 "1 SLC \n"
                 "1 NUM 256\n"
                 "0 OPT *\n"
                 "B0\n");

    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, issue_big_dict) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* pikaAsm =
        Parser_fileToAsm(buffs, "test/python/issue/issue_big_dict.py");
    __platform_printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "3 STR type\n"
                 "3 STR div\n"
                 "3 STR attributes\n"
                 "4 STR class\n"
                 "4 STR container\n"
                 "4 STR style\n"
                 "5 STR width\n"
                 "5 NUM 240\n"
                 "5 STR height\n"
                 "5 NUM 240\n"
                 "5 STR margin\n"
                 "5 NUM 0\n"
                 "5 STR padding\n"
                 "5 NUM 0\n"
                 "5 STR border-radius\n"
                 "5 NUM 0\n"
                 "5 STR border-width\n"
                 "5 NUM 0\n"
                 "5 STR border-color\n"
                 "5 STR red\n"
                 "5 STR background-color\n"
                 "5 NUM 37864\n"
                 "4 DCT \n"
                 "3 DCT \n"
                 "3 STR nodes\n"
                 "5 STR type\n"
                 "5 STR text\n"
                 "5 STR attributes\n"
                 "6 STR font-size\n"
                 "6 STR 30\n"
                 "6 STR class\n"
                 "6 STR result-text\n"
                 "6 STR style\n"
                 "7 STR top\n"
                 "7 NUM 5\n"
                 "7 STR left\n"
                 "7 NUM 5\n"
                 "7 STR width\n"
                 "7 NUM 220\n"
                 "7 STR text-align\n"
                 "7 STR right\n"
                 "7 STR height\n"
                 "7 NUM 30\n"
                 "7 STR color\n"
                 "7 STR white\n"
                 "7 STR text-overflow\n"
                 "7 STR longbreak\n"
                 "7 STR border-width\n"
                 "7 NUM 0\n"
                 "7 STR border-color\n"
                 "7 STR red\n"
                 "7 STR background-color\n"
                 "7 STR transparent\n"
                 "6 DCT \n"
                 "5 DCT \n"
                 "5 STR nodes\n"
                 "5 LST \n"
                 "5 STR bindings\n"
                 "7 STR attrName\n"
                 "7 STR text\n"
                 "7 STR key\n"
                 "7 STR result\n"
                 "7 STR isText\n"
                 "7 REF true\n"
                 "6 DCT \n"
                 "5 LST \n"
                 "5 STR events\n"
                 "5 LST \n"
                 "5 STR value\n"
                 "5 STR 0\n"
                 "5 STR widgetName\n"
                 "5 STR widget1\n"
                 "4 DCT \n"
                 "5 STR type\n"
                 "5 STR div\n"
                 "5 STR attributes\n"
                 "6 STR class\n"
                 "6 STR key-wrapper\n"
                 "6 STR onclick\n"
                 "6 STR onclick\n"
                 "6 STR style\n"
                 "7 STR width\n"
                 "7 NUM 60\n"
                 "7 STR height\n"
                 "7 NUM 50\n"
                 "7 STR border-width\n"
                 "7 NUM 0\n"
                 "7 STR border-color\n"
                 "7 STR red\n"
                 "7 STR background-color\n"
                 "7 STR transparent\n"
                 "7 STR top\n"
                 "7 NUM 40\n"
                 "7 STR left\n"
                 "7 NUM 10\n"
                 "6 DCT \n"
                 "5 DCT \n"
                 "5 STR nodes\n"
                 "7 STR type\n"
                 "7 STR text\n"
                 "7 STR attributes\n"
                 "8 STR font-size\n"
                 "8 STR 30\n"
                 "8 STR class\n"
                 "8 STR key\n"
                 "8 STR style\n"
                 "9 STR left\n"
                 "9 NUM 0\n"
                 "9 STR top\n"
                 "9 NUM 0\n"
                 "9 STR width\n"
                 "9 NUM 50\n"
                 "9 STR height\n"
                 "9 NUM 40\n"
                 "9 STR margin\n"
                 "9 NUM 0\n"
                 "9 STR padding\n"
                 "9 NUM 0\n"
                 "9 STR color\n"
                 "9 STR white\n"
                 "9 STR font-size\n"
                 "9 NUM 30\n"
                 "9 STR text-align\n"
                 "9 STR center\n"
                 "9 STR background-color\n"
                 "9 STR transparent\n"
                 "8 DCT \n"
                 "7 DCT \n"
                 "7 STR nodes\n"
                 "7 LST \n"
                 "7 STR bindings\n"
                 "7 LST \n"
                 "7 STR events\n"
                 "7 LST \n"
                 "7 STR value\n"
                 "7 STR 1\n"
                 "7 STR widgetName\n"
                 "7 STR widget3\n"
                 "6 DCT \n"
                 "5 LST \n"
                 "5 STR bindings\n"
                 "5 LST \n"
                 "5 STR events\n"
                 "7 STR onclick\n"
                 "7 STR onclick\n"
                 "6 DCT \n"
                 "5 LST \n"
                 "5 STR widgetName\n"
                 "5 STR widget2\n"
                 "4 DCT \n"
                 "5 STR type\n"
                 "5 STR div\n"
                 "5 STR attributes\n"
                 "6 STR class\n"
                 "6 STR key-wrapper\n"
                 "6 STR onclick\n"
                 "6 STR onclick\n"
                 "6 STR style\n"
                 "7 STR width\n"
                 "7 NUM 60\n"
                 "7 STR height\n"
                 "7 NUM 50\n"
                 "7 STR border-width\n"
                 "7 NUM 0\n"
                 "7 STR border-color\n"
                 "7 STR red\n"
                 "7 STR background-color\n"
                 "7 STR transparent\n"
                 "7 STR top\n"
                 "7 NUM 40\n"
                 "7 STR left\n"
                 "7 NUM 60\n"
                 "6 DCT \n"
                 "5 DCT \n"
                 "5 STR nodes\n"
                 "7 STR type\n"
                 "7 STR text\n"
                 "7 STR attributes\n"
                 "8 STR font-size\n"
                 "8 STR 30\n"
                 "8 STR class\n"
                 "8 STR key\n"
                 "8 STR style\n"
                 "9 STR left\n"
                 "9 NUM 0\n"
                 "9 STR top\n"
                 "9 NUM 0\n"
                 "9 STR width\n"
                 "9 NUM 50\n"
                 "9 STR height\n"
                 "9 NUM 40\n"
                 "9 STR margin\n"
                 "9 NUM 0\n"
                 "9 STR padding\n"
                 "9 NUM 0\n"
                 "9 STR color\n"
                 "9 STR white\n"
                 "9 STR font-size\n"
                 "9 NUM 30\n"
                 "9 STR text-align\n"
                 "9 STR center\n"
                 "9 STR background-color\n"
                 "9 STR transparent\n"
                 "8 DCT \n"
                 "7 DCT \n"
                 "7 STR nodes\n"
                 "7 LST \n"
                 "7 STR bindings\n"
                 "7 LST \n"
                 "7 STR events\n"
                 "7 LST \n"
                 "7 STR value\n"
                 "7 STR 2\n"
                 "7 STR widgetName\n"
                 "7 STR widget5\n"
                 "6 DCT \n"
                 "5 LST \n"
                 "5 STR bindings\n"
                 "5 LST \n"
                 "5 STR events\n"
                 "7 STR onclick\n"
                 "7 STR onclick\n"
                 "6 DCT \n"
                 "5 LST \n"
                 "5 STR widgetName\n"
                 "5 STR widget4\n"
                 "4 DCT \n"
                 "5 STR type\n"
                 "5 STR div\n"
                 "5 STR attributes\n"
                 "6 STR class\n"
                 "6 STR key-wrapper\n"
                 "6 STR onclick\n"
                 "6 STR onclick\n"
                 "6 STR style\n"
                 "7 STR width\n"
                 "7 NUM 60\n"
                 "7 STR height\n"
                 "7 NUM 50\n"
                 "7 STR border-width\n"
                 "7 NUM 0\n"
                 "7 STR border-color\n"
                 "7 STR red\n"
                 "7 STR background-color\n"
                 "7 STR transparent\n"
                 "7 STR top\n"
                 "7 NUM 40\n"
                 "7 STR left\n"
                 "7 NUM 110\n"
                 "6 DCT \n"
                 "5 DCT \n"
                 "5 STR nodes\n"
                 "7 STR type\n"
                 "7 STR text\n"
                 "7 STR attributes\n"
                 "8 STR font-size\n"
                 "8 STR 30\n"
                 "8 STR class\n"
                 "8 STR key\n"
                 "8 STR style\n"
                 "9 STR left\n"
                 "9 NUM 0\n"
                 "9 STR top\n"
                 "9 NUM 0\n"
                 "9 STR width\n"
                 "9 NUM 50\n"
                 "9 STR height\n"
                 "9 NUM 40\n"
                 "9 STR margin\n"
                 "9 NUM 0\n"
                 "9 STR padding\n"
                 "9 NUM 0\n"
                 "9 STR color\n"
                 "9 STR white\n"
                 "9 STR font-size\n"
                 "9 NUM 30\n"
                 "9 STR text-align\n"
                 "9 STR center\n"
                 "9 STR background-color\n"
                 "9 STR transparent\n"
                 "8 DCT \n"
                 "7 DCT \n"
                 "7 STR nodes\n"
                 "7 LST \n"
                 "7 STR bindings\n"
                 "7 LST \n"
                 "7 STR events\n"
                 "7 LST \n"
                 "7 STR value\n"
                 "7 STR 3\n"
                 "7 STR widgetName\n"
                 "7 STR widget7\n"
                 "6 DCT \n"
                 "5 LST \n"
                 "5 STR bindings\n"
                 "5 LST \n"
                 "5 STR events\n"
                 "7 STR onclick\n"
                 "7 STR onclick\n"
                 "6 DCT \n"
                 "5 LST \n"
                 "5 STR widgetName\n"
                 "5 STR widget6\n"
                 "4 DCT \n"
                 "5 STR type\n"
                 "5 STR div\n"
                 "5 STR attributes\n"
                 "6 STR class\n"
                 "6 STR key-wrapper\n"
                 "6 STR onclick\n"
                 "6 STR onclick\n"
                 "6 STR style\n"
                 "7 STR width\n"
                 "7 NUM 60\n"
                 "7 STR height\n"
                 "7 NUM 50\n"
                 "7 STR border-width\n"
                 "7 NUM 0\n"
                 "7 STR border-color\n"
                 "7 STR red\n"
                 "7 STR background-color\n"
                 "7 STR transparent\n"
                 "7 STR top\n"
                 "7 NUM 40\n"
                 "7 STR left\n"
                 "7 NUM 160\n"
                 "6 DCT \n"
                 "5 DCT \n"
                 "5 STR nodes\n"
                 "7 STR type\n"
                 "7 STR text\n"
                 "7 STR attributes\n"
                 "8 STR font-size\n"
                 "8 STR 30\n"
                 "8 STR class\n"
                 "8 STR key\n"
                 "8 STR style\n"
                 "9 STR left\n"
                 "9 NUM 0\n"
                 "9 STR top\n"
                 "9 NUM 0\n"
                 "9 STR width\n"
                 "9 NUM 50\n"
                 "9 STR height\n"
                 "9 NUM 40\n"
                 "9 STR margin\n"
                 "9 NUM 0\n"
                 "9 STR padding\n"
                 "9 NUM 0\n"
                 "9 STR color\n"
                 "9 STR white\n"
                 "9 STR font-size\n"
                 "9 NUM 30\n"
                 "9 STR text-align\n"
                 "9 STR center\n"
                 "9 STR background-color\n"
                 "9 STR transparent\n"
                 "8 DCT \n"
                 "7 DCT \n"
                 "7 STR nodes\n"
                 "7 LST \n"
                 "7 STR bindings\n"
                 "7 LST \n"
                 "7 STR events\n"
                 "7 LST \n"
                 "7 STR value\n"
                 "7 STR +\n"
                 "7 STR widgetName\n"
                 "7 STR widget9\n"
                 "6 DCT \n"
                 "5 LST \n"
                 "5 STR bindings\n"
                 "5 LST \n"
                 "5 STR events\n"
                 "7 STR onclick\n"
                 "7 STR onclick\n"
                 "6 DCT \n"
                 "5 LST \n"
                 "5 STR widgetName\n"
                 "5 STR widget8\n"
                 "4 DCT \n"
                 "3 LST \n"
                 "3 STR bindings\n"
                 "3 LST \n"
                 "3 STR events\n"
                 "3 LST \n"
                 "3 STR widgetName\n"
                 "3 STR widget0\n"
                 "2 DCT \n"
                 "1 DCT \n"
                 "0 RUN nodes.update\n"
                 "B0\n");

    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, issue_dict_update) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* pikaAsm =
        Parser_fileToAsm(buffs, "test/python/issue/issue_dict_update.py");
    __platform_printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, object_test2) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"hello(name = 'world', isShow = isShow)";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR world\n"
                 "1 OUT name\n"
                 "1 REF isShow\n"
                 "1 OUT isShow\n"
                 "0 RUN hello\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, i_pp) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "i = 0\n"
        "while i < 10000:\n"
        "    i += 1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 NUM 0\n"
                 "0 OUT i\n"
                 "B0\n"
                 "1 REF i\n"
                 "1 NUM 10000\n"
                 "0 OPT <\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "1 REF i\n"
                 "2 NUM 1\n"
                 "1 RUN \n"
                 "0 OPT +\n"
                 "0 OUT i\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n");

    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, multi_num) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a,b ,c = 1, 2, 3\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 NUM 1\n"
                 "1 NUM 2\n"
                 "1 NUM 3\n"
                 "0 RUN \n"
                 "0 OUT $tmp\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 0\n"
                 "0 SLC \n"
                 "0 OUT a\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 1\n"
                 "0 SLC \n"
                 "0 OUT b\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 2\n"
                 "0 SLC \n"
                 "0 OUT c\n"
                 "B0\n"
                 "0 DEL $tmp\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, multi_import) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "import  a,b ,c\n"
        "import  a,b ,c as d, e ,f\n"
        "from a import b, c, d\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 IMP a\n"
                 "B0\n"
                 "0 IMP b\n"
                 "B0\n"
                 "0 IMP c\n"
                 "B0\n"
                 "0 IMP a\n"
                 "B0\n"
                 "0 IMP b\n"
                 "B0\n"
                 "0 IMP c\n"
                 "B0\n"
                 "1 REF a\n"
                 "1 REF b\n"
                 "1 REF c\n"
                 "0 RUN \n"
                 "0 OUT $tmp\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 0\n"
                 "0 SLC \n"
                 "0 OUT d\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 1\n"
                 "0 SLC \n"
                 "0 OUT e\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 2\n"
                 "0 SLC \n"
                 "0 OUT f\n"
                 "B0\n"
                 "0 DEL $tmp\n"
                 "B0\n"
                 "0 IMP a\n"
                 "B0\n"
                 "1 REF a.b\n"
                 "1 REF a.c\n"
                 "1 REF a.d\n"
                 "0 RUN \n"
                 "0 OUT $tmp\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 0\n"
                 "0 SLC \n"
                 "0 OUT b\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 1\n"
                 "0 SLC \n"
                 "0 OUT c\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 2\n"
                 "0 SLC \n"
                 "0 OUT d\n"
                 "B0\n"
                 "0 DEL $tmp\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, from_import_as) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "from PikaStdData import String as string";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 IMP PikaStdData\n"
                 "B0\n"
                 "0 REF PikaStdData.String\n"
                 "0 OUT string\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, multi_from_import_as) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "from a import b, c, d as e, f, g\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 IMP a\n"
                 "B0\n"
                 "1 REF a.b\n"
                 "1 REF a.c\n"
                 "1 REF a.d\n"
                 "0 RUN \n"
                 "0 OUT $tmp\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 0\n"
                 "0 SLC \n"
                 "0 OUT e\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 1\n"
                 "0 SLC \n"
                 "0 OUT f\n"
                 "B0\n"
                 "1 REF $tmp\n"
                 "1 NUM 2\n"
                 "0 SLC \n"
                 "0 OUT g\n"
                 "B0\n"
                 "0 DEL $tmp\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, print_ssa) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "print(**a)\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 REF a\n"
                 "1 OPT **\n"
                 "0 RUN print\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, not_in) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "1 not in a\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 1\n"
                 "2 REF a\n"
                 "1 OPT  in \n"
                 "0 OPT  not \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

// TEST(parser, def_not_in) {
//     g_PikaMemInfo.heapUsedMax = 0;
//     Args* buffs = New_strBuff();
//     char* lines =
//         "def test(a=\"hmac-md5\"):\n"
//         "    print(\"input:\",a)\n"
//         "    print(\"table:\"t )\n"
//         "    if a not in t:\n"
//         "        print(\"a not in t\")else:\n"
//         "        print(\"a in t\")\n"
//         "test()\n";
//     printf("%s\r\n", lines);
//     char* pikaAsm = Parser_linesToAsm(buffs, lines);
//     printf("%s", pikaAsm);
//     EXPECT_STREQ(pikaAsm, "");
//     args_deinit(buffs);
//     EXPECT_EQ(pikaMemNow(), 0);
// }

TEST(parser, syntex_issue_lwekj) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "json.loads('{a:1}'}";
    EXPECT_EQ((uintptr_t)Parser_linesToAsm(buffs, lines), 0);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, syntex_issue_lekj) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a[b()] = 1";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 RUN b\n"
                 "1 NUM 1\n"
                 "0 RUN __setitem__\n"
                 "0 OUT a\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, syntex_issue_l1l2) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "test(a=1, b= 2)";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 NUM 1\n"
                 "1 OUT a\n"
                 "1 NUM 2\n"
                 "1 OUT b\n"
                 "0 RUN test\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, syntex_issue_12ojd) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "class Test:\n"
        "    def test(self):\n"
        "        a, b = c\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 CLS Test()\n"
                 "0 JMP 1\n"
                 "B1\n"
                 "0 RUN TinyObj\n"
                 "0 OUT self\n"
                 "B1\n"
                 "0 RAS self\n"
                 "B1\n"
                 "0 DEF test(self)\n"
                 "0 JMP 1\n"
                 "B2\n"
                 "0 REF c\n"
                 "0 OUT $tmp\n"
                 "B2\n"
                 "1 REF $tmp\n"
                 "1 NUM 0\n"
                 "0 SLC \n"
                 "0 OUT a\n"
                 "B2\n"
                 "1 REF $tmp\n"
                 "1 NUM 1\n"
                 "0 SLC \n"
                 "0 OUT b\n"
                 "B2\n"
                 "0 DEL $tmp\n"
                 "B2\n"
                 "0 RET \n"
                 "B1\n"
                 "0 RAS $origin\n"
                 "B1\n"
                 "0 NEW self\n"
                 "0 RET \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, tuple_void) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "()\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 RUN \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, page_add) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "PikaUI.Page().add(\n"
        "    ui.Widget(\n"
        "    ).add(\n"
        "        ui.Text()\n"
        "    ),\n"
        ")\n"
        "page.add(\n"
        "    ui.Widget(\n"
        "    ).add(\n"
        "        ui.Text()\n"
        "    ),\n"
        ")\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 RUN PikaUI.Page\n"
                 "2 RUN ui.Widget\n"
                 "2 RUN ui.Text\n"
                 "1 RUN .add\n"
                 "0 RUN .add\n"
                 "B0\n"
                 "2 RUN ui.Widget\n"
                 "2 RUN ui.Text\n"
                 "1 RUN .add\n"
                 "0 RUN page.add\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, hint_assign) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "a:int = 1\n"
        "d:{str:int} = {}\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 NUM 1\n"
                 "0 OUT a\n"
                 "B0\n"
                 "0 DCT \n"
                 "0 OUT d\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, while_try_while) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "while True:\n"
        "    print(\"before try\")\n"
        "    try:\n"
        "        print(\"after try\")\n"
        "        while True:\n"
        "            raise\n"
        "    except:\n"
        "        break\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 REF True\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "1 STR before try\n"
                 "0 RUN print\n"
                 "B1\n"
                 "0 TRY \n"
                 "B2\n"
                 "1 STR after try\n"
                 "0 RUN print\n"
                 "B2\n"
                 "0 REF True\n"
                 "0 JEZ 2\n"
                 "B3\n"
                 "0 REF RuntimeError\n"
                 "0 RIS \n"
                 "B2\n"
                 "0 JMP -1\n"
                 "B1\n"
                 "0 NTR \n"
                 "0 GER \n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 EXP \n"
                 "B2\n"
                 "0 BRK \n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, return_list) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "return [\n"
        "    ui.Text(\n"
        "        align=ui.ALIGN.CENTER\n"
        "    ),\n"
        "    ui.Button(\n"
        "        onclick=self.onclick_next\n"
        "    )\n"
        "]\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 REF ui.ALIGN.CENTER\n"
                 "2 OUT align\n"
                 "1 RUN ui.Text\n"
                 "2 REF self.onclick_next\n"
                 "2 OUT onclick\n"
                 "1 RUN ui.Button\n"
                 "0 LST \n"
                 "0 RET \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, not_in_or) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a not in x or b not in x or c not in x\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "4 REF a\n"
                 "4 REF x\n"
                 "3 OPT  in \n"
                 "2 OPT  not \n"
                 "4 REF b\n"
                 "4 REF x\n"
                 "3 OPT  in \n"
                 "2 OPT  not \n"
                 "1 OPT  or \n"
                 "3 REF c\n"
                 "3 REF x\n"
                 "2 OPT  in \n"
                 "1 OPT  not \n"
                 "0 OPT  or \n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, for_in_split) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "for rows in data[1:]:\n"
        "    print(rows)"
        "while data[1:]:\n"
        "    print(data[1:])\n";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 REF data\n"
                 "2 NUM 1\n"
                 "3 NUM 99999\n"
                 "2 OPT -\n"
                 "1 SLC \n"
                 "0 RUN iter\n"
                 "0 OUT $l0\n"
                 "B0\n"
                 "0 RUN $l0.__next__\n"
                 "0 OUT rows\n"
                 "0 EST rows\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "2 REF rows\n"
                 "1 RUN print\n"
                 "1 NUM 1\n"
                 "2 NUM 99999\n"
                 "1 OPT -\n"
                 "0 SLC \n"
                 "B1\n"
                 "2 REF data\n"
                 "2 NUM 1\n"
                 "3 NUM 99999\n"
                 "2 OPT -\n"
                 "1 SLC \n"
                 "0 RUN print\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL $l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, common_issue_1b23f4c1bf) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* pikaAsm =
        Parser_fileToAsm(buffs, "test/python/issue/common_issue_1b23f4c1bf.py");
    __platform_printf("%s", pikaAsm);

    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 CLS Test1()\n"
                 "0 JMP 1\n"
                 "B1\n"
                 "0 RUN TinyObj\n"
                 "0 OUT self\n"
                 "B1\n"
                 "0 RAS self\n"
                 "B1\n"
                 "0 RAS $origin\n"
                 "B1\n"
                 "0 NEW self\n"
                 "0 RET \n"
                 "B0\n"
                 "0 CLS Test2()\n"
                 "0 JMP 1\n"
                 "B1\n"
                 "0 RUN Test1\n"
                 "0 OUT self\n"
                 "B1\n"
                 "0 RAS self\n"
                 "B1\n"
                 "0 DEF print(self)\n"
                 "0 JMP 1\n"
                 "B2\n"
                 "1 STR Test2\n"
                 "0 RUN print\n"
                 "B2\n"
                 "0 RET \n"
                 "B1\n"
                 "0 RAS $origin\n"
                 "B1\n"
                 "0 NEW self\n"
                 "0 RET \n"
                 "B0\n"
                 "0 RUN Test2\n"
                 "0 OUT a\n"
                 "B0\n"
                 "0 RUN a.print\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, str_join) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_fileToAsm(buffs, "test/python/builtin/str_join.py");
    __platform_printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, csv) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_fileToAsm(buffs, "package/pikascript/csv.py");
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, default_issue) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def __init__(self, csvfile, delimiter=\",\", quotechar='\"') -> "
        "None:\n"
        "    pass";

    printf("%s\r\n", lines);
    char* pikaAsm = Parser_linesToAsm(buffs, lines);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 DEF __init__(self,csvfile,delimiter=,quotechar=)\n"
                 "0 JMP 1\n"
                 "B1\n"
                 "0 EST delimiter\n"
                 "0 JNZ 2\n"
                 "B1\n"
                 "0 STR ,\n"
                 "0 OUT delimiter\n"
                 "B1\n"
                 "0 EST quotechar\n"
                 "0 JNZ 2\n"
                 "B1\n"
                 "0 STR \"\n"
                 "0 OUT quotechar\n"
                 "B1\n"
                 "0 EST \n"
                 "0 JNZ 2\n"
                 "B1\n"
                 "B1\n"
                 "B1\n"
                 "0 RET \n"
                 "B0\n");
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif

TEST_END