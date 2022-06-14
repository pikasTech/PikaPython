#include "test_common.h"

extern "C" {
/* head infomation */
typedef QueueObj AST;
char* Parser_multiLineToAsm(Args* outBuffs, char* multiLine);
char* AST_toPikaASM(AST* ast, Args* outBuffs);
AST* AST_parseLine(char* line, Stack* blockStack);
char* Parser_LineToAsm(Args* buffs, char* line, Stack* blockStack);
int32_t AST_deinit(AST* ast);
char* Lexer_getTokens(Args* outBuffs, char* stmt);
char* Lexer_printTokens(Args* outBuffs, char* tokens);
char* strsPopTokenWithSkip_byStr(Args* buffs,
                                 char* stmts,
                                 char* str,
                                 char skipStart,
                                 char skipEnd);
char* strsGetCleanCmd(Args* outBuffs, char* cmd);
}

TEST(parser, NEW) {
    AST* ast = AST_parseLine("add(a,b)", NULL);
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_toPikaASM(ast, buffs);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    AST_deinit(ast);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_b) {
    AST* ast = AST_parseLine("add( a , b)", NULL);
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_toPikaASM(ast, buffs);
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
    char* pikaAsm = AST_toPikaASM(ast, buffs);
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
    char* pikaAsm = AST_toPikaASM(ast, buffs);
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
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "1 RUN c\n"
        "0 RUN a\n"
    );

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
    Arg* buffArg = arg_setStr(NULL, "", s);
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

extern PikaMemInfo pikaMemInfo;
TEST(parser, while_true_if_false_both_exit) {
    pikaMemInfo.heapUsedMax = 0;
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
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =(char *)
        "while true:\n"
        "    rgb.flow()\n"
        "    if false:\n"
        "        a=3\n"
        "        test.on(add(2,3))\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
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
    pikaMemInfo.heapUsedMax = 0;
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = 1 + 1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 NUM 1\n"
                 "1 NUM 1\n"
                 "0 OPT +\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_3) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = 1 + 2 + 3\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 1\n"
                 "2 NUM 2\n"
                 "1 OPT +\n"
                 "1 NUM 3\n"
                 "0 OPT +\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_pp) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = a + 1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 NUM 1\n"
                 "0 OPT +\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, while_a_pp) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "while a < 10:\n"
        "    print(a)\n"
        "    a = a + 1\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
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
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = 1 * 2 + 3\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 1\n"
                 "2 NUM 2\n"
                 "1 OPT *\n"
                 "1 NUM 3\n"
                 "0 OPT +\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_m2p3_) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = 1 * (2 + 3)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 NUM 1\n"
                 "3 NUM 2\n"
                 "3 NUM 3\n"
                 "2 OPT +\n"
                 "1 RUN \n"
                 "0 OPT *\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_m12p3_) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = (1 + 2) * 3\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "3 NUM 1\n"
                 "3 NUM 2\n"
                 "2 OPT +\n"
                 "1 RUN \n"
                 "1 NUM 3\n"
                 "0 OPT *\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, method_equ) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "if right.read() == 1:\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 RUN right.read\n"
                 "1 NUM 1\n"
                 "0 OPT ==\n"
                 "0 JEZ 1\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, equ_method) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "if 1 == right.read() :\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 NUM 1\n"
                 "1 RUN right.read\n"
                 "0 OPT ==\n"
                 "0 JEZ 1\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, def_add) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def add(a, b):\n"
        "    a + b\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
    "B0\n"
    "0 DEF add(a,b)\n"
    "0 JMP 1\n"
    "B1\n"
    "1 REF a\n"
    "1 REF b\n"
    "0 OPT +\n"
    "B1\n"
    "0 RET \n"
    "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, def_add_return) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def add(a, b):\n"
        "    return a + b\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
    "B0\n"
    "0 DEF add(a,b)\n"
    "0 JMP 1\n"
    "B1\n"
    "1 REF a\n"
    "1 REF b\n"
    "0 OPT +\n"
    "0 RET \n"
    "B1\n"
    "0 RET \n"
    "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, def_while_return) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def add(a, b):\n"
        "    while True:\n"
        "        return a + b\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
    "B0\n"
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
    "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, def_while_return_void) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def add(a, b):\n"
        "    while True:\n"
        "        return\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
    "B0\n"
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
    "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, signed_num) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = -1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    char* tokens_print =
        Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines));
    printf("%s", tokens_print);
    EXPECT_STREQ(tokens_print, "{sym}a{opt}={opt}-{lit}1\n");
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "1 NUM 1\n"
        "0 OPT -\n"
        "0 OUT a\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, comp_signed_num) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "if a > -1:\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "1 REF a\n"
        "2 NUM 1\n"
        "1 OPT -\n"
        "0 OPT >\n"
        "0 JEZ 1\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, symbol_add) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, " res = add(1,2)");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf("%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens,
                 "{sym}res{opt}={sym}add{dvd}({lit}1{dvd},{lit}2{dvd})");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, symbol_1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, "a(");

    /* assert */
    EXPECT_EQ(tokens, (char*)NULL);

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, operator_not) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, "not not not ");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf("%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens, "{opt} not {opt} not {opt} not ");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, symbol_Nag) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, "-10-20");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf("%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens, "{opt}-{lit}10{opt}-{lit}20");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, operator_all) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs,
                                   "not or and "
                                   "+ += - -="
                                   "* ** *= **="
                                   "/ // /= //="
                                   "% %= = == !="
                                   "> >= >>"
                                   "< <= <<"
                                   "&|^~");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf("%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(
        printTokens,
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
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, "a+b-c(25**=ek)!=-28");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf("%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens,
                 "{sym}a{opt}+{sym}b{opt}-{sym}c{dvd}({lit}25{opt}**={sym}ek{"
                 "dvd}){opt}!={opt}-{lit}28");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, symbol_and) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, " res = add(1 and lkj,2)");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf("%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens,
                 "{sym}res{opt}={sym}add{dvd}({lit}1{opt} and "
                 "{sym}lkj{dvd},{lit}2{dvd})");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, sting) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, " a= 'elk 2'");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf("%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens, "{sym}a{opt}={lit}'elk 2'");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, num_1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, "1");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf("%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens, "{lit}1");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, jjcc) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, "a = (1 + 1.1) * 3 - 2 /4.0");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf("%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens,
                 "{sym}a{opt}={dvd}({lit}1{opt}+{lit}1.1{dvd}){opt}*{lit}3{opt}"
                 "-{lit}2{opt}/{lit}4.0");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, pop_by_str) {
    Args* buffs = New_strBuff();
    char* tokens = strsCopy(buffs, "3(>=)2>=29");
    char* token1 = strsPopTokenWithSkip_byStr(buffs, tokens, ">=", '(', ')');
    char* token2 = tokens;
    /* assert */
    EXPECT_STREQ("3(>=)2", token1);
    EXPECT_STREQ("29", token2);
    args_deinit(buffs);
}

TEST(parser, mm) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = a ** -1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "2 REF a\n"
        "1 OPT **\n"
        "1 NUM 1\n"
        "0 OPT -\n"
        "0 OUT a\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, self_inc) {
    pikaMemInfo.heapUsedMax = 0;
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
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
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, n_n1) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = ~-1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "1 OPT ~\n"
        "1 NUM 1\n"
        "0 OPT -\n"
        "0 OUT a\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, or_) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "( a>1) or (b<= 3)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "3 REF a\n"
        "3 NUM 1\n"
        "2 OPT >\n"
        "1 RUN \n"
        "3 REF b\n"
        "3 NUM 3\n"
        "2 OPT <=\n"
        "1 RUN \n"
        "0 OPT  or \n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, _or_) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "if (1>2) and (2>1):\n"
        "    b = 1\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
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
        "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, annotation) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = t#test\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "0 REF t\n"
        "0 OUT a\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, annotation_block) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "while True:\n"
        "    a = 1\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "0 REF True\n"
        "0 JEZ 2\n"
        "B1\n"
        "0 NUM 1\n"
        "0 OUT a\n"
        "B0\n"
        "0 JMP -1\n"
        "B0\n"
    );
    lines =
        "while True:\n"
        "    a = 1\n"
        "#\n";
    printf("%s", lines);
    pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "0 REF True\n"
        "0 JEZ 2\n"
        "B1\n"
        "0 NUM 1\n"
        "0 OUT a\n"
    );
    lines =
        "while True:\n"
        "    a = 1\n"
        "#test\n"
        "    b = 2\n"
        "\n";
    printf("%s", lines);
    pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
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
        "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, if_elif_else) {
    pikaMemInfo.heapUsedMax = 0;
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
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
        "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, for_range) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "for i in range(0,10):\n"
        "    print(i)\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 0\n"
                 "2 NUM 10\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT _l0\n"
                 "B0\n"
                 "0 RUN _l0.__next__\n"
                 "0 OUT i\n"
                 "0 EST i\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "1 REF i\n"
                 "0 RUN print\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL _l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, for_range_rtt) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "for i in range(0, 3):\n"
        "    print('hello PikaScript!')\n"
        "    thread.mdelay(500)\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 0\n"
                 "2 NUM 3\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT _l0\n"
                 "B0\n"
                 "0 RUN _l0.__next__\n"
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
                 "0 DEL _l0\n"
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
                 "0 OUT _l0\n"
                 "B0\n"
                 "0 RUN _l0.__next__\n"
                 "0 OUT arg\n"
                 "0 EST arg\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "1 REF arg\n"
                 "0 RUN print\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL _l0\n"
                 "B0\n");
    stack_deinit(&bs);
    args_deinit(bf);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, for_for_range) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "a = 0\n"
        "for i in range(0, 10):\n"
        "    for k in range(0, 3):\n"
        "        print(k)\n"
        "        a = a + k\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
    "B0\n"
    "0 NUM 0\n"
    "0 OUT a\n"
    "B0\n"
    "2 NUM 0\n"
    "2 NUM 10\n"
    "1 RUN range\n"
    "0 RUN iter\n"
    "0 OUT _l0\n"
    "B0\n"
    "0 RUN _l0.__next__\n"
    "0 OUT i\n"
    "0 EST i\n"
    "0 JEZ 2\n"
    "B1\n"
    "2 NUM 0\n"
    "2 NUM 3\n"
    "1 RUN range\n"
    "0 RUN iter\n"
    "0 OUT _l1\n"
    "B1\n"
    "0 RUN _l1.__next__\n"
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
    "0 DEL _l1\n"
    "B0\n"
    "0 JMP -1\n"
    "B0\n"
    "0 DEL _l0\n"
    "B0\n"
    );

    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    byteCodeFrame_print(&bytecode_frame);
    byteCodeFrame_deinit(&bytecode_frame);

    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, break_) {
    pikaMemInfo.heapUsedMax = 0;
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 0\n"
                 "2 NUM 3\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT _l0\n"
                 "B0\n"
                 "0 RUN _l0.__next__\n"
                 "0 OUT i\n"
                 "0 EST i\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 NUM 1\n"
                 "0 OUT i\n"
                 "0 JEZ 1\n"
                 "B2\n"
                 "0 BRK\n"
                 "B2\n"
                 "0 BRK\n"
                 "B2\n"
                 "0 CTN\n"
                 "B2\n"
                 "0 CTN\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL _l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, prime_100) {
    pikaMemInfo.heapUsedMax = 0;
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
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
                 "0 OUT _l0\n"
                 "B0\n"
                 "0 RUN _l0.__next__\n"
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
                 "0 OUT _l1\n"
                 "B1\n"
                 "0 RUN _l1.__next__\n"
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
                 "0 BRK\n"
                 "B1\n"
                 "0 JMP -1\n"
                 "B1\n"
                 "0 DEL _l1\n"
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
                 "0 DEL _l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, __iter__) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "__res = __iter__()\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 RUN __iter__\n"
                 "0 OUT __res\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

/* not solve yet */

// TEST(parser, strEqu) {
//     pikaMemInfo.heapUsedMax = 0;
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
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "for b in 'test':\n"
        "    a = 1\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR test\n"
                 "0 RUN iter\n"
                 "0 OUT _l0\n"
                 "B0\n"
                 "0 RUN _l0.__next__\n"
                 "0 OUT b\n"
                 "0 EST b\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "0 NUM 1\n"
                 "0 OUT a\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL _l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, print_ddd) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "print(\"[Info]: in Python config...\")\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR [Info]: in Python config...\n"
                 "0 RUN print\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTEX_ITEM_SLICE_ENABLE
TEST(parser, __get__3) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = b[c+d] + e[f*j]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 REF b\n"
                 "3 REF c\n"
                 "3 REF d\n"
                 "2 OPT +\n"
                 "4 REF c\n"
                 "4 REF d\n"
                 "3 OPT +\n"
                 "3 NUM 1\n"
                 "2 OPT +\n"
                 "2 NUM 1\n"
                 "1 RUN __slice__\n"
                 "2 REF e\n"
                 "3 REF f\n"
                 "3 REF j\n"
                 "2 OPT *\n"
                 "4 REF f\n"
                 "4 REF j\n"
                 "3 OPT *\n"
                 "3 NUM 1\n"
                 "2 OPT +\n"
                 "2 NUM 1\n"
                 "1 RUN __slice__\n"
                 "0 OPT +\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTEX_ITEM_SLICE_ENABLE
TEST(parser, __get__) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = b[c]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF b\n"
                 "1 REF c\n"
                 "2 REF c\n"
                 "2 NUM 1\n"
                 "1 OPT +\n"
                 "1 NUM 1\n"
                 "0 RUN __slice__\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTEX_ITEM_SLICE_ENABLE
TEST(parser, __get__2) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = b[c+d]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF b\n"
                 "2 REF c\n"
                 "2 REF d\n"
                 "1 OPT +\n"
                 "3 REF c\n"
                 "3 REF d\n"
                 "2 OPT +\n"
                 "2 NUM 1\n"
                 "1 OPT +\n"
                 "1 NUM 1\n"
                 "0 RUN __slice__\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTEX_ITEM_SLICE_ENABLE
TEST(parser, __set__) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a[b] = c\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "1 REF b\n"
                 "1 REF c\n"
                 "1 STR a\n"
                 "0 RUN __set__\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, str_p_str) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = str(1) + str(1)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 1\n"
                 "1 RUN str\n"
                 "2 NUM 1\n"
                 "1 RUN str\n"
                 "0 OPT +\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, test__) {
    pikaMemInfo.heapUsedMax = 0;
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, global) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "def test_global():\n"
        "    global x\n"
        "    global y, z\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char *)
        "B0\n"
        "0 DEF test_global()\n"
        "0 JMP 1\n"
        "B1\n"
        "0 GLB x\n"
        "B1\n"
        "0 GLB y,z\n"
        "B1\n"
        "0 RET \n"
        "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, mpy_demo_1) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "chars = ' .,-:;i+hHM$*#@ '\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 STR  .,-:;i+hHM$*#@ \n"
                 "0 OUT chars\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, clean_compled_str) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* res = strsGetCleanCmd(buffs, "chars = ' .,-:;i+hHM$*#@ '\n");
    EXPECT_STREQ(res, "chars=' .,-:;i+hHM$*#@ '\n");
    printf("%s", res);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, class_) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "class Test(TinyObj):\n"
        "    x = 1\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm, (char* )
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
        "0 RAS $origin\n"
        "B1\n"
        "0 NEW self\n"
        "0 RET \n"
        "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, class_def) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "class Test():\n"
        "    x = 1\n"
        "    def hello():\n"
        "        print('hello')\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
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
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "print(-a)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
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

#if PIKA_SYNTEX_ITEM_SLICE_ENABLE
TEST(parser, list_1_2) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "print(list[0] + list[1])\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "3 REF list\n"
                 "3 NUM 0\n"
                 "4 NUM 0\n"
                 "4 NUM 1\n"
                 "3 OPT +\n"
                 "3 NUM 1\n"
                 "2 RUN __slice__\n"
                 "3 REF list\n"
                 "3 NUM 1\n"
                 "4 NUM 1\n"
                 "4 NUM 1\n"
                 "3 OPT +\n"
                 "3 NUM 1\n"
                 "2 RUN __slice__\n"
                 "1 OPT +\n"
                 "0 RUN print\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, class_def_void_line) {
    pikaMemInfo.heapUsedMax = 0;
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
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

TEST(parser, multiLine_import) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =(char *)
        "import TEE\n"
        "from EE import C\n"
        "from PikaStdLib import MemChecker as MC\n"
        "while true:\n"
        "    rgb.flow()\n"
        "    if false:\n"
        "        a=3\n"
        "        test.on(add(2,3))\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
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

TEST(parser, multiLine_comment) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =(char *)
        "'''\n"
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
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
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a += 1+1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF a\n"
                 "3 NUM 1\n"
                 "3 NUM 1\n"
                 "2 OPT +\n"
                 "1 RUN \n"
                 "0 OPT +\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, a_j) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, "a=");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf("%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens, "{sym}a{opt}=");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, plus_equ_) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a -= (1+1-3)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
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
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, class_demo_3) {
    pikaMemInfo.heapUsedMax = 0;
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, a_a) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "a = 1\n"
        "a\n"
        "a\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, a_cuohao_j) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = (3 - 4) - 4\n";
    printf("%s\n", Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines)));
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "3 NUM 3\n"
                 "3 NUM 4\n"
                 "2 OPT -\n"
                 "1 RUN \n"
                 "1 NUM 4\n"
                 "0 OPT -\n"
                 "0 OUT a\n");
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, _3_3) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "-3+3\n";
    printf("%s\n", Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines)));
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 3\n"
                 "1 OPT -\n"
                 "1 NUM 3\n"
                 "0 OPT +\n");
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_BUILTIN_LIST_ENABLE
TEST(parser, list_init) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = [1, 2, 3]\n";
    printf("%s\n", Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines)));
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 NUM 1\n"
                 "1 NUM 2\n"
                 "1 NUM 3\n"
                 "0 LST \n"
                 "0 OUT a\n");
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, list_init_fun) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "test([1, 2, 3])\n";
    printf("%s\n", Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines)));
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 1\n"
                 "2 NUM 2\n"
                 "2 NUM 3\n"
                 "1 LST \n"
                 "0 RUN test\n");
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, bytes_iteral) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = b'\\x00\\x01'\n";
    char* tokens_str = Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines));
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 BYT \\x00\\x01\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, import_as) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "import PikaStdLib as std\n";
    char* tokens_str = Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines));
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 IMP PikaStdLib\n"
                 "B0\n"
                 "0 REF PikaStdLib\n"
                 "0 OUT std\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, str_equ) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = 'num ='\n";
    char* tokens_str = Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines));
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 STR num =\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTEX_ITEM_SLICE_ENABLE
TEST(parser, bytes_index) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "res2 = b'eqrt'[2]\n";
    char* tokens_str = Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines));
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 BYT eqrt\n"
                 "1 NUM 2\n"
                 "2 NUM 2\n"
                 "2 NUM 1\n"
                 "1 OPT +\n"
                 "1 NUM 1\n"
                 "0 RUN __slice__\n"
                 "0 OUT res2\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, hex_iteral) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = 0b10\n";
    char* tokens_str = Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines));
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 NUM 0b10\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, tab) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "for i in range(0, 100):\n"
        "\tprint(i)\n"
        "\n";
    char* tokens_str = Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines));
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 0\n"
                 "2 NUM 100\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT _l0\n"
                 "B0\n"
                 "0 RUN _l0.__next__\n"
                 "0 OUT i\n"
                 "0 EST i\n"
                 "0 JEZ 2\n"
                 "B1\n"
                 "1 REF i\n"
                 "0 RUN print\n"
                 "B0\n"
                 "0 JMP -1\n"
                 "B0\n"
                 "0 DEL _l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTEX_ITEM_SLICE_ENABLE
TEST(parser, parse_issue2) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "    recv_buf[1] = dat \n";
    char* tokens = Lexer_getTokens(buffs, lines);
    uint16_t token_size = Tokens_getSize(tokens);
    EXPECT_EQ(token_size, 8);
    char* tokens_str = Lexer_printTokens(buffs, tokens);
    printf("%s\n", tokens_str);
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B1\n"
                 "1 REF recv_buf\n"
                 "1 NUM 1\n"
                 "1 REF dat\n"
                 "1 STR recv_buf\n"
                 "0 RUN __set__\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, parse_issue3) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "recv_buf[1] = dat ";
    char* clean_cmd = strsGetCleanCmd(buffs, lines);
    EXPECT_STREQ(clean_cmd, "recv_buf[1]=dat");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTEX_ITEM_SLICE_ENABLE
TEST(parser, slice1) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = recv_buf[1:4]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF recv_buf\n"
                 "1 NUM 1\n"
                 "1 NUM 4\n"
                 "1 NUM 1\n"
                 "0 RUN __slice__\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTEX_ITEM_SLICE_ENABLE
TEST(parser, slice2) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = recv_buf[1:4:2]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF recv_buf\n"
                 "1 NUM 1\n"
                 "1 NUM 4\n"
                 "1 NUM 2\n"
                 "0 RUN __slice__\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, str_add1) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "msg = \"device_names[\" + str(i) + \"]:\"";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    char* tokens_print =
        Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines));
    printf("%s\r\n", tokens_print);
    EXPECT_STREQ(tokens_print,
                 "{sym}msg{opt}={lit}\"device_names[\"{opt}+{sym}str{dvd}({sym}"
                 "i{dvd}){opt}+{lit}\"]:\"");
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "2 STR device_names[\n"
        "3 REF i\n"
        "2 RUN str\n"
        "1 OPT +\n"
        "1 STR ]:\n"
        "0 OPT +\n"
        "0 OUT msg\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, str_add2) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "msg = \"device_names[\" + str(i)";
    printf("%s\r\n", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    char* tokens_print =
        Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines));
    printf("%s\r\n", tokens_print);
    EXPECT_STREQ(tokens_print,
                 "{sym}msg{opt}={lit}\"device_names[\"{opt}+{sym}str{dvd}({sym}"
                 "i{dvd})");
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "1 STR device_names[\n"
        "2 REF i\n"
        "1 RUN str\n"
        "0 OPT +\n"
        "0 OUT msg\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, mpy1) {
    pikaMemInfo.heapUsedMax = 0;
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTEX_ITEM_SLICE_ENABLE
TEST(parser, slice_12lkj) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = b[:6]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF b\n"
                 "1 NUM 0\n"
                 "1 NUM 6\n"
                 "1 NUM 1\n"
                 "0 RUN __slice__\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(parser, str_string) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = str(String('test'))\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 STR test\n"
                 "1 RUN String\n"
                 "0 RUN str\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, json_literal) {
    pikaMemInfo.heapUsedMax = 0;
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
        Lexer_printTokens(buffs, Lexer_getTokens(buffs, lines));
    printf("%s\r\n", tokens_print);

    EXPECT_STREQ(tokens_print,
                 "{sym}a{opt}={lit}'{\"name\": \"mculover666\",\"age\": "
                 "22,\"weight\": 55.5,\"address\":{    \"country\": \"China\", "
                 "   \"zip-code\": 111111},\"skill\": [\"c\", \"Java\", "
                 "\"Python\"],\"student\": false}'");

    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(
        pikaAsm,
        "B0\n"
        "0 STR {\"name\": \"mculover666\",\"age\": 22,\"weight\": "
        "55.5,\"address\":{    \"country\": \"China\",    \"zip-code\": "
        "111111},\"skill\": [\"c\", \"Java\", \"Python\"],\"student\": false}\n"
        "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, issuekd) {
    pikaMemInfo.heapUsedMax = 0;
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
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
                 "0 OUT _l0\n"
                 "B0\n"
                 "0 RUN _l0.__next__\n"
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
                 "0 DEL _l0\n"
                 "B0\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, cjson_test4) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    Arg* lines_buff = arg_loadFile(NULL, "../../examples/cJSON/test4.py");
    char* lines = (char*)arg_getBytes(lines_buff);
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
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
                 "data1\n0 RUN pika_cjson.Parse\n0 OUT a\nB0\n0 RUN a.print\n");
    args_deinit(buffs);
    arg_deinit(lines_buff);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, connection) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "print('\\\n"
        "test')\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR test\n"
                 "0 RUN print\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, connection2) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =
        "a = \\\n"
        "3\n"
        "print\\\n"
        "(a)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 NUM 3\n"
                 "0 OUT a\n"
                 "B0\n"
                 "1 REF a\n"
                 "0 RUN print\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTEX_ITEM_FORMAT_ENABLE
TEST(parser, format1) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "s = 'res:%d' % 23";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR res:%d\n"
                 "1 NUM 23\n"
                 "0 RUN cformat\n"
                 "0 OUT s\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTEX_ITEM_FORMAT_ENABLE
TEST(parser, format2) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "'res:%d:%d' % (23, 25)";
    printf("%s\n", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR res:%d:%d\n"
                 "1 NUM 23\n"
                 "1 NUM 25\n"
                 "0 RUN cformat\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif
