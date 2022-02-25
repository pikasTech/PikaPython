#include "gtest/gtest.h"
extern "C" {
#include "BaseObj.h"
#include "PikaParser.h"
#include "dataMemory.h"
#include "dataStrs.h"

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
    AST* ast = AST_parseLine((char*)"add(a,b)", NULL);
    Args* buffs = New_strBuff();
    char* pikaAsm = AST_toPikaASM(ast, buffs);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    AST_deinit(ast);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, add_a_b) {
    AST* ast = AST_parseLine((char*)"add( a , b)", NULL);
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
    AST* ast = AST_parseLine((char*)"d = add(add(a,b)  , c)", NULL);
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
    AST* ast =
        AST_parseLine((char*)"d.p = a.add(b.add(a,se.b)  , pmw.c)", NULL);
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
    char* line = (char*)"d.p = a.add(b.add(a,se.b,diek(pp))  , pmw.c())";
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
    char* line = (char*)"a(c())";
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
    char* line = (char*)"literal('2.322')";
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
    char* line = (char*)"b=add(a,literal('1'))";
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
    char* line = (char*)"b=add(a,1)";
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
    char* line = (char*)"b=add(a,'1')";
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
    char* line = (char*)"b = add(add(add(add(1, 2), 3), 4), 5)";
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
    char* line = (char*)"a = 1";
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
    char* line = (char*)"while true:";
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    "B1\n"
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    "B1\n"
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    "B1\n"
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* tokens = Lexer_getTokens(buffs, (char*)"not not not ");
    char* printTokens = Lexer_printTokens(buffs, tokens);
    printf((char*)"%s\n", printTokens);

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

TEST(parser, pop_by_str) {
    Args* buffs = New_strBuff();
    char* tokens = strsCopy(buffs, (char*)"3(>=)2>=29");
    char* token1 =
        strsPopTokenWithSkip_byStr(buffs, tokens, (char*)">=", '(', ')');
    char* token2 = tokens;
    /* assert */
    EXPECT_STREQ((char*)"3(>=)2", token1);
    EXPECT_STREQ((char*)"29", token2);
    args_deinit(buffs);
}

TEST(parser, mm) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"a = a ** -1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "1 REF a\n"
        "1 NUM -1\n"
        "0 OPT **\n"
        "0 OUT a\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, self_inc) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)
    "a += -1\n"
    "a -= -1\n"
    "a *= -1\n"
    "a /= -1\n"
    "a **= -1\n"
    "a //= -1\n"
    "a >= -1\n"
    "a <= -1\n"
    "a != -1\n"
    "a %= -1\n"
    ;
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "1 REF a\n"
        "1 NUM -1\n"
        "0 OPT +=\n"
        "B0\n"
        "1 REF a\n"
        "1 NUM -1\n"
        "0 OPT -=\n"
        "B0\n"
        "1 REF a\n"
        "1 NUM -1\n"
        "0 OPT *=\n"
        "B0\n"
        "1 REF a\n"
        "1 NUM -1\n"
        "0 OPT /=\n"
        "B0\n"
        "1 REF a\n"
        "1 NUM -1\n"
        "0 OPT **=\n"
        "B0\n"
        "1 REF a\n"
        "1 NUM -1\n"
        "0 OPT //=\n"
        "B0\n"
        "1 REF a\n"
        "1 NUM -1\n"
        "0 OPT >=\n"
        "B0\n"
        "1 REF a\n"
        "1 NUM -1\n"
        "0 OPT <=\n"
        "B0\n"
        "1 REF a\n"
        "1 NUM -1\n"
        "0 OPT !=\n"
        "B0\n"
        "1 REF a\n"
        "1 NUM -1\n"
        "0 OPT %=\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, n_n1) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"a = ~-1\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "1 NUM -1\n"
        "0 OPT ~\n"
        "0 OUT a\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, or_) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"( a>1) or (b<= 3)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)
        "if (1>2) and (2>1):\n"
        "    b = 1\n"
        "\n"
    ;
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)"a = t#test\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)
        "while True:\n"
        "    a = 1\n"
        "\n"
        ;
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    lines = (char*)
        "while True:\n"
        "    a = 1\n"
        "#\n"
        ;
    printf("%s", lines);
    pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char *)
        "B0\n"
        "0 REF True\n"
        "0 JEZ 2\n"
        "B1\n"
        "0 NUM 1\n"
        "0 OUT a\n"
    );
    lines = (char*)
        "while True:\n"
        "    a = 1\n"
        "#test\n"
        "    b = 2\n"
        "\n"
        ;
    printf("%s", lines);
    pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)
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
    "\n"
    ;
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)
    "for i in range(0,10):\n"
    "    print(i)\n"
    "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 0\n"
                 "2 NUM 10\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT _l0\n"
                 "0 REF _r1\n"
                 "0 REF _r2\n"
                 "0 REF _r3\n"
                 "0 OUT _l0.a1\n"
                 "0 OUT _l0.a2\n"
                 "0 OUT _l0.a3\n"
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
    char* lines = (char*)
            "for i in range(0, 3):\n"
            "    print('hello PikaScript!')\n"
            "    thread.mdelay(500)\n"
            "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 0\n"
                 "2 NUM 3\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT _l0\n"
                 "0 REF _r1\n"
                 "0 REF _r2\n"
                 "0 REF _r3\n"
                 "0 OUT _l0.a1\n"
                 "0 OUT _l0.a2\n"
                 "0 OUT _l0.a3\n"
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
    Stack* bs = New_Stack();
    char* s = strsCopy(bf, (char*)"");
    s = parse("for arg in xrange(0,10):", bf, s, bs);
    s = parse("    print(arg)", bf, s, bs);
    s = parse("", bf, s, bs);
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
    stack_deinit(bs);
    args_deinit(bf);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, for_for_range) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)
         "a = 0\n"
         "for i in range(0, 10):\n"
         "    for k in range(0, 3):\n"
         "        print(k)\n"
         "        a = a + k\n"
         "\n"
    ;
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    "0 REF _r1\n"
    "0 REF _r2\n"
    "0 REF _r3\n"
    "0 OUT _l0.a1\n"
    "0 OUT _l0.a2\n"
    "0 OUT _l0.a3\n"
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
    "0 REF _r1\n"
    "0 REF _r2\n"
    "0 REF _r3\n"
    "0 OUT _l1.a1\n"
    "0 OUT _l1.a2\n"
    "0 OUT _l1.a3\n"
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
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, break_) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)
            "for i in range(0, 3):\n"
            "    if i = 1:\n"
            "        break\n"
            "        break \n"
            "        continue\n"
            "        continue \n"
            "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 NUM 0\n"
                 "2 NUM 3\n"
                 "1 RUN range\n"
                 "0 RUN iter\n"
                 "0 OUT _l0\n"
                 "0 REF _r1\n"
                 "0 REF _r2\n"
                 "0 REF _r3\n"
                 "0 OUT _l0.a1\n"
                 "0 OUT _l0.a2\n"
                 "0 OUT _l0.a3\n"
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
    char* lines = (char*)
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
                 "0 REF _r1\n"
                 "0 REF _r2\n"
                 "0 REF _r3\n"
                 "0 OUT _l0.a1\n"
                 "0 OUT _l0.a2\n"
                 "0 OUT _l0.a3\n"
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
                 "0 REF _r1\n"
                 "0 REF _r2\n"
                 "0 REF _r3\n"
                 "0 OUT _l1.a1\n"
                 "0 OUT _l1.a2\n"
                 "0 OUT _l1.a3\n"
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
    char* lines = (char*)"__res = __iter__()\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
//     char* lines = (char*)
//                  "a = '1ee' == '1ee'\n"
//                  "b = '1e' == '1ee'\n"
//                  "\n";
//     printf("%s", lines);
//     char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)
                 "for b in 'test':\n"
                 "    a = 1\n"
                 "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* lines = (char*)"print(\"[Info]: in Python config...\")\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 STR [Info]: in Python config...\n"
                 "0 RUN print\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, __get__3) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"a = b[c+d] + e[f*j]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "2 REF b\n"
                 "3 REF c\n"
                 "3 REF d\n"
                 "2 OPT +\n"
                 "1 RUN __get__\n"
                 "2 REF e\n"
                 "3 REF f\n"
                 "3 REF j\n"
                 "2 OPT *\n"
                 "1 RUN __get__\n"
                 "0 OPT +\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, __get__) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"a = b[c]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF b\n"
                 "1 REF c\n"
                 "0 RUN __get__\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, __get__2) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"a = b[c+d]\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,
                 "B0\n"
                 "1 REF b\n"
                 "2 REF c\n"
                 "2 REF d\n"
                 "1 OPT +\n"
                 "0 RUN __get__\n"
                 "0 OUT a\n");
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, __set__) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"a[b] = c\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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

TEST(parser, str_p_str) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"a = str(1) + str(1)\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    EXPECT_STREQ(pikaAsm,(char*)
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
    char* lines = (char*)
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
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, global) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)
        "def test_global():\n"
        "    global x\n"
        "    global y, z\n"
        "\n"
        ;
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
        "0 RET\n"
        "B0\n"
    );
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, mpy_demo_1) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)"chars = ' .,-:;i+hHM$*#@ '\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
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
    char* res = strsGetCleanCmd(buffs, (char*)"chars = ' .,-:;i+hHM$*#@ '\n");
    EXPECT_STREQ(res, "chars=' .,-:;i+hHM$*#@ '\n");
    printf("%s", res);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(parser, class_) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = (char*)
        "class Test():\n"
        "    x = 1\n"
        "    def hello():\n"
        "        print('hello')\n"
        "\n"
        ;
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
