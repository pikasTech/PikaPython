#include "gtest/gtest.h"
extern "C" {
#include "BaseObj.h"
#include "PikaParser.h"
#include "dataMemory.h"
#include "dataStrs.h"
}

TEST(parser, NEW) {
    AST* ast = pikaParseLine((char*)"add(a,b)", NULL);
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_toPikaAsm(ast, buffs);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    AST_deinit(ast);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_b) {
    AST* ast = pikaParseLine((char*)"add( a , b)", NULL);
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_toPikaAsm(ast, buffs);
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
    AST* ast = pikaParseLine((char*)"d = add(add(a,b)  , c)", NULL);
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_toPikaAsm(ast, buffs);
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
    AST* ast =
        pikaParseLine((char*)"d.p = a.add(b.add(a,se.b)  , pmw.c)", NULL);
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_toPikaAsm(ast, buffs);
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
    char* line = (char*)"d.p = a.add(b.add(a,se.b,diek(pp))  , pmw.c())";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseLineToAsm(buffs, line, NULL);
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

TEST(parser, str1) {
    char* line = (char*)"literal('2.322')";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseLineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR 2.322\n"
                 "0 RUN literal\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, str2) {
    char* line = (char*)"b=add(a,literal('1'))";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseLineToAsm(buffs, line, NULL);
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
    char* line = (char*)"b=add(a,1)";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseLineToAsm(buffs, line, NULL);
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
    char* line = (char*)"b=add(a,'1')";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseLineToAsm(buffs, line, NULL);
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
    char* line = (char*)"b = add(add(add(add(1, 2), 3), 4), 5)";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseLineToAsm(buffs, line, NULL);
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
    char* line = (char*)"a = 1";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseLineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "0 NUM 1\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, while_true) {
    char* line = (char*)"while true:";
    Args* buffs = New_strBuff();
    char* pikaAsm = pikaParseLineToAsm(buffs, line, NULL);
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
                         pikaParseLineToAsm(runBuffs, (char*)line, blockStack));
    pikaAsm = strsCopy(outBuffs, pikaAsm);
    args_deinit(runBuffs);
    return pikaAsm;
}

TEST(parser, while_true_block) {
    Args* bf = New_strBuff();
    Stack* bs = New_Stack();
    char* s = strsCopy(bf, (char*)"");
    s = parse("while true:", bf, s, bs);
    s = parse("    rgb.flow()", bf, s, bs);
    s = parse("", bf, s, bs);
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
    stack_deinit(bs);
    args_deinit(bf);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, while_true_false) {
    Args* bf = New_strBuff();
    Stack* bs = New_Stack();
    char* s = strsCopy(bf, (char*)"");
    s = parse("while true:", bf, s, bs);
    s = parse("    rgb.flow()", bf, s, bs);
    s = parse("    while false:", bf, s, bs);
    s = parse("        a=3", bf, s, bs);
    s = parse("        test.on(add(2,3))", bf, s, bs);
    s = parse("    print('flowing')", bf, s, bs);
    s = parse("", bf, s, bs);
    Arg* buffArg = arg_setStr(NULL, (char*)"", s);
    stack_deinit(bs);
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
    Stack* bs = New_Stack();
    char* s = strsCopy(bf, (char*)"");
    s = parse("while true:", bf, s, bs);
    s = parse("    rgb.flow()", bf, s, bs);
    s = parse("    while false:", bf, s, bs);
    s = parse("        a=3", bf, s, bs);
    s = parse("        test.on(add(2,3))", bf, s, bs);
    s = parse("", bf, s, bs);
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
    stack_deinit(bs);
    args_deinit(bf);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, if_) {
    Args* bf = New_strBuff();
    Stack* bs = New_Stack();
    char* s = strsCopy(bf, (char*)"");
    s = parse("if true:", bf, s, bs);
    s = parse("    rgb.flow()", bf, s, bs);
    s = parse("", bf, s, bs);
    printf("%s", s);
    EXPECT_STREQ(s,
                 "B0\n"
                 "0 REF true\n"
                 "0 JEZ 1\n"
                 "B1\n"
                 "0 RUN rgb.flow\n"
                 "B0\n");
    stack_deinit(bs);
    args_deinit(bf);
    EXPECT_EQ(pikaMemNow(), 0);
}

extern PikaMemInfo pikaMemInfo;
TEST(parser, while_true_if_false_both_exit) {
    pikaMemInfo.heapUsedMax = 0;
    Args* bf = New_strBuff();
    Stack* bs = New_Stack();
    char* s = strsCopy(bf, (char*)"");
    s = parse("while true:", bf, s, bs);
    s = parse("    rgb.flow()", bf, s, bs);
    s = parse("    if false:", bf, s, bs);
    s = parse("        a=3", bf, s, bs);
    s = parse("        test.on(add(2,3))", bf, s, bs);
    s = parse("", bf, s, bs);
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
    stack_deinit(bs);
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
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
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
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
    printf("mem max in parse: %0.2f Kb\n", pikaMemMax() / 1024.0);
    printf("%s", pikaAsm);

    EXPECT_STREQ(pikaAsm,
                 "B0\n"
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
                 "B0\n"
                 "0 RUN rgb.init\n"
                 "B0\n"
                 "0 RUN rgb.enable\n"
                 "B0\n"
                 "B0\n"
                 "1 STR hello 2\n"
                 "0 RUN print\n"
                 "B0\n"
                 "1 STR mem used max:\n"
                 "0 RUN print\n"
                 "B0\n"
                 "0 RUN mem.max\n"
                 "B0\n"
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
                 "B0\n"
                 "B0\n"
                 "B0\n"
                 "B0\n");

    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"a = 1 + 1\n";
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)"a = 1 + 2 + 3\n";
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 NUM 1\n"
                 "2 NUM 2\n"
                 "2 NUM 3\n"
                 "1 OPT +\n"
                 "0 OPT +\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_pp) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"a = a + 1\n";
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)
    "while a < 10:\n"
    "    print(a)\n"
    "    a = a + 1\n"
    "\n";
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)"a = 1 * 2 + 3\n";
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)"a = 1 * (2 + 3)\n";
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)"a = (1 + 2) * 3\n";
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)"if right.read() == 1:\n";
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)"if 1 == right.read() :\n";
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)
    "def add(a, b):\n"
    "    a + b\n"
    "\n"
    ;
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
    "B0\n"
    "0 DEF add(a,b)\n"
    "0 JMP 1\n"
    "B1\n"
    "1 REF a\n"
    "1 REF b\n"
    "0 OPT +\n"
    "0 RET\n"
    "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, def_add_return) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)
    "def add(a, b):\n"
    "    return a + b\n"
    "\n"
    ;
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
    "B0\n"
    "0 DEF add(a,b)\n"
    "0 JMP 1\n"
    "B1\n"
    "1 REF a\n"
    "1 REF b\n"
    "0 OPT +\n"
    "0 RET\n"
    "0 RET\n"
    "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, def_while_return) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)
    "def add(a, b):\n"
    "    while True:\n"
    "        return a + b\n"
    "\n"
    ;
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
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
    "0 RET\n"
    "B1\n"
    "0 JMP -1\n"
    "0 RET\n"
    "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, def_while_return_void) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)
    "def add(a, b):\n"
    "    while True:\n"
    "        return\n"
    "\n"
    ;
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
    "B0\n"
    "0 DEF add(a,b)\n"
    "0 JMP 1\n"
    "B1\n"
    "0 REF True\n"
    "0 JEZ 2\n"
    "B2\n"
    "0 RET\n"
    "B1\n"
    "0 JMP -1\n"
    "0 RET\n"
    "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, signed_num) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"a = -1\n";
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
    "B0\n"
    "0 NUM -1\n"
    "0 OUT a\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, comp_signed_num) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"if a > -1:\n";
    printf("%s", lines);
    char* pikaAsm = pikaParseMultiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "1 REF a\n"
        "1 NUM -1\n"
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
    char* tokens = Lexer_getTokens(buffs, (char*)" res = add(1,2)");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf((char*)"%s\n", printTokens);

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
    char* tokens = Lexer_getTokens(buffs, (char*)"a(");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf((char*)"%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens, "{sym}a{dvd}(");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, operator_not) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, (char*)"not not not ");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf((char*)"%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens, "{opt}not{opt}not{opt}not");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, symbol_Nag) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, (char*)"-10-20");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf((char*)"%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens, "{lit}-10{opt}-{lit}20");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, operator_all) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, (char*)
                   "not or and "
                   "+ += - -="
                   "* ** *= **="
                   "/ // /= //="
                   "% %= = == !="
                   "> >= >>"
                   "< <= <<"
                   "&|^~"
                    );
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf((char*)"%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens,
                 "{opt}not{opt}or{opt}and{opt}+{opt}+={opt}-{opt}-={opt}*{opt}*"
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
    char* tokens = Lexer_getTokens(buffs, (char*)"a+b-c(25**=ek)!=-28");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf((char*)"%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens,
                 "{sym}a{opt}+{sym}b{opt}-{sym}c{dvd}({lit}25{opt}**={sym}ek{"
                 "dvd}){opt}!={lit}-28");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, symbol_and) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, (char*)" res = add(1 and lkj,2)");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf((char*)"%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(
        printTokens,
        "{sym}res{opt}={sym}add{dvd}({lit}1{opt}and{sym}lkj{dvd},{lit}2{dvd})");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(lexser, sting) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();

    /* run */
    char* tokens = Lexer_getTokens(buffs, (char*)" a= 'elk 2'");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf((char*)"%s\n", printTokens);

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
    char* tokens = Lexer_getTokens(buffs, (char*)"1");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf((char*)"%s\n", printTokens);

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
    char* tokens = Lexer_getTokens(buffs, (char*)"a = (1 + 1.1) * 3 - 2 /4.0");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf((char*)"%s\n", printTokens);

    /* assert */
    EXPECT_STREQ(printTokens,
                 "{sym}a{opt}={dvd}({lit}1{opt}+{lit}1.1{dvd}){opt}*{lit}3{opt}"
                 "-{lit}2{opt}/{lit}4.0");

    /* deinit */
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}