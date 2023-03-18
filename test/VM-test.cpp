#include "test_common.h"
TEST_START

TEST(VM, num1) {
    char* line = "1";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    __platform_printf("BEGIN\r\n");
    pikaVM_runAsm(self, pikaAsm);
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "1\r\n");
    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_1) {
    char* line = "a = 1";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    pikaVM_runAsm(self, pikaAsm);
    ASSERT_EQ(args_getInt(self->list, "a"), 1);

    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_1d1) {
    char* line = "a = 1.1";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);
    ASSERT_FLOAT_EQ(args_getFloat(globals->list, "a"), 1.1);

    obj_deinit(self);
    // obj_deinit(globals);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, str_xy) {
    char* line = "a = 'xy'";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    ASSERT_STREQ(args_getStr(globals->list, "a"), "xy");

    obj_deinit(self);
    // obj_deinit(globals);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, str_xy_space) {
    char* line = "a = 'xy '";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    ASSERT_STREQ(args_getStr(globals->list, "a"), "xy ");

    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, ref_a_b) {
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    Args* buffs = New_strBuff();

    pikaVM_runAsm(self, pika_lines2Asm(buffs, "a = 'xy '"));

    args_deinit(buffs);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, Run_add) {
    PikaObj* self = newRootObj("root", New_PikaMath_Operator);
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, "a = plusInt(1,2)");
    __platform_printf("%s", pikaAsm);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    args_deinit(buffs);
    int a = args_getInt(globals->list, "a");
    ASSERT_EQ(a, 3);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, Run_add_multy) {
    PikaObj* self = newRootObj("root", New_PikaMath_Operator);
    Args* buffs = New_strBuff();

    pikaVM_runAsm(self, pika_lines2Asm(buffs, "b = 2"));

    args_deinit(buffs);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, Run_add_1_2_3) {
    PikaObj* self = newRootObj("root", New_PikaMath_Operator);
    Args* buffs = New_strBuff();
    char* pikaAsm =
        pika_lines2Asm(buffs, "a = plusInt(1, plusInt(2,3) )");
    __platform_printf("%s", pikaAsm);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    args_deinit(buffs);
    int a = args_getInt(globals->list, "a");
    ASSERT_EQ(a, 6);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

extern PikaMemInfo g_PikaMemInfo;
TEST(VM, WHILE) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =(char *)
        "a = 1\n"
        "b = 0\n"
        "while a:\n"
        "    b = 1\n"
        "    a = 0\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = pika_lines2Asm(buffs, lines);
    printf("%s", pikaAsm);
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* self = newRootObj("root", New_TinyObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);
    EXPECT_EQ(args_getInt(globals->list, "a"), 0);
    EXPECT_EQ(args_getInt(globals->list, "b"), 1);
    // obj_deinit(globals);
    args_deinit(buffs);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_1_1) {
    char* line = "a = 1 + 1";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int res = args_getInt(globals->list, "a");
    obj_deinit(self);
    args_deinit(buffs);
    ASSERT_EQ(res, 2);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_1_1d1) {
    char* line = "a = 1 + 1.1";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    float res = args_getFloat(globals->list, "a");
    obj_deinit(self);
    // obj_deinit(globals);
    args_deinit(buffs);
    ASSERT_FLOAT_EQ(res, 2.1);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_jjcc) {
    char* line = "a = (1 + 1.1) * 3 - 2 /4.0";
    printf("%s\r\n", line);
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    float res = args_getFloat(globals->list, "a");
    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    ASSERT_FLOAT_EQ(res, 5.8);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, while_a_1to10) {
    char* line =
        "a = 0\n"
        "while a < 10:\n"
        "    a += 1\n"
        "    print(a)\n"
        "\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int res = args_getInt(globals->list, "a");
    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    ASSERT_FLOAT_EQ(res, 10);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, mem_x) {
    char* line =
        "mem = PikaStdLib.MemChecker()\n"
        "mem.x = 1\n"
        "print(mem.x)\n"
        "mem.x = 2\n"
        "print(mem.x)\n"
        "\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("", New_PikaMain);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int res = obj_getInt(globals, "mem.x");
    EXPECT_EQ(2, res);
    obj_deinit(self);
    // obj_deinit(globals);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RET_instruct) {
    char* pikaAsm =
        "B1\n"
        "0 NUM 13\n"
        "0 RET \n"
        "0 NUM 2\n"
        "0 RET \n"
        "B0\n";
    PikaObj* self = New_TinyObj(NULL);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);
    Arg* return_arg = args_getArg(globals->list, "@rt");
    int num = arg_getInt(return_arg);
    EXPECT_EQ(num, 13);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_DEF) {
    char* pikaAsm =
        "B0\n"
        "0 DEF test()\n"
        "0 JMP 1\n"
        "B1\n"
        "0 NUM 1\n"
        "0 RET \n"
        "B0\n"
        "0 RUN test\n"
        "0 OUT a\n";
    PikaObj* self = New_TinyObj(NULL);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);
    int num = obj_getInt(globals, "a");
    EXPECT_EQ(num, 1);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_global) {
    char* pikaAsm =
        "B0\n"
        "0 NUM 1\n"
        "0 OUT a\n"
        "B0\n"
        "0 DEF test()\n"
        "0 JMP 1\n"
        "B1\n"
        "0 REF a\n"
        "0 RET \n"
        "B0\n"
        "0 RUN test\n"
        "0 OUT b\n";
    PikaObj* self = New_TinyObj(NULL);
    pikaVM_runAsm(self, pikaAsm);
    int a = obj_getInt(self, "a");
    int b = obj_getInt(self, "b");
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 1);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_local_b) {
    char* pikaAsm =
        "B0\n"
        "0 NUM 1\n"
        "0 OUT a\n"
        "B0\n"
        "0 DEF test()\n"
        "0 JMP 1\n"
        "B1\n"
        "0 REF a\n"
        "0 OUT b\n"
        "1 REF b\n"
        "1 REF a\n"
        "0 OPT +\n"
        "0 RET \n"
        "B0\n"
        "0 RUN test\n"
        "0 OUT c\n";
    PikaObj* self = newRootObj("", New_BaseObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);
    int a = obj_getInt(globals, "a");
    int b = obj_getInt(globals, "b");
    int c = obj_getInt(globals, "c");
    EXPECT_EQ(a, 1);
    /* b is local, should not be exist in globals */
    EXPECT_EQ(b, _PIKA_INT_ERR);
    EXPECT_EQ(c, 2);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_DEF_add) {
    char* pikaAsm =
        "B0\n"
        "0 DEF add(a,b)\n"
        "0 JMP 1\n"
        "B1\n"
        "1 REF b\n"
        "1 REF a\n"
        "0 OPT +\n"
        "0 RET \n"
        "B0\n"
        "1 NUM 1\n"
        "1 NUM 2\n"
        "0 RUN add\n"
        "0 OUT c\n";
    PikaObj* self = newRootObj("", New_BaseObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);
    int a = obj_getInt(globals, "a");
    int b = obj_getInt(globals, "b");
    int c = obj_getInt(globals, "c");
    /* a is local, should not be exist in globals */
    EXPECT_EQ(a, _PIKA_INT_ERR);
    /* b is local, should not be exist in globals */
    EXPECT_EQ(b, _PIKA_INT_ERR);
    EXPECT_EQ(c, 3);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, run_def_add) {
    char* line =
        "def add(a, b):\n"
        "    return a + b\n"
        "c = add(1,3)\n"
        "\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int c = args_getInt(globals->list, "c");
    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    ASSERT_FLOAT_EQ(c, 4);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, equ) {
    char* line =
        "if -1 == -1:\n"
        "    a = 1\n"
        "\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int c = args_getInt(globals->list, "a");
    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    ASSERT_FLOAT_EQ(c, 1);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, if_elif) {
    char* line =
        "a = 2\n"
        "b = 0\n"
        "if a > 1:\n"
        "    b = 1\n"
        "elif a > 0:\n"
        "    b = 2\n"
        "\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int b = args_getInt(globals->list, "b");
    obj_deinit(self);
    args_deinit(buffs);
    ASSERT_FLOAT_EQ(b, 1);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, if_else) {
    char* line =
        "a = 0\n"
        "b = 0\n"
        "if a > 1:\n"
        "    b = 1\n"
        "else:\n"
        "    b = 2\n"
        "\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int b = args_getInt(globals->list, "b");
    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    ASSERT_FLOAT_EQ(b, 2);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, if_else_) {
    char* line =
        "a = 2\n"
        "b = 0\n"
        "if a > 1:\n"
        "    b = 1\n"
        "else:\n"
        "    b = 2\n"
        "\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int b = args_getInt(globals->list, "b");
    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    ASSERT_FLOAT_EQ(b, 1);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, EST) {
    char* pikaAsm =
        "B0\n"
        "0 EST a\n"
        "0 OUT b\n";
    PikaObj* self = newRootObj("", New_BaseObj);
    pikaVM_runAsm(self, pikaAsm);
    int a = obj_getInt(self, "a");
    int b = obj_getInt(self, "b");
    /* a is local, should not be exist in globals */
    EXPECT_EQ(a, _PIKA_INT_ERR);
    /* b is local, should not be exist in globals */
    EXPECT_EQ(b, 0);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_AS) {
    char* pikaAsm =
        "B0\n"
        "0 RUN TinyObj\n"
        "0 OUT as\n"
        "B0\n"
        "0 RAS as\n"
        "B0\n"
        "0 NUM 1\n"
        "0 OUT x\n"
        "B0\n"
        "0 RAS $origin\n"
        "B0\n"
        "0 NUM 2\n"
        "0 OUT x\n";
    PikaObj* self = newRootObj("", New_PikaMain);
    pikaVM_runAsm(self, pikaAsm);
    PikaObj* as = obj_getObj(self, "as");
    int x_as_ = obj_getInt(as, "x");
    int x_as = obj_getInt(self, "as.x");
    int x_origin = obj_getInt(self, "x");
    EXPECT_EQ(x_as_, 1);
    EXPECT_EQ(x_as, 1);
    EXPECT_EQ(x_origin, 2);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_NEW) {
    char* pikaAsm =
        "B0\n"
        "0 RUN TinyObj\n"
        "0 OUT newObj\n"
        "0 NUM 1\n"
        "0 OUT newObj.x\n"
        "B0\n"
        "0 NEW newObj\n"
        "0 OUT outObj\n";
    PikaObj* self = newRootObj("", New_PikaMain);
    pikaVM_runAsm(self, pikaAsm);
    Arg* newObj = obj_getArg(self, "newObj");
    Arg* outObj = obj_getArg(self, "outObj");
    void* newObj_ptr = arg_getPtr(newObj);
    void* outObj_ptr = arg_getPtr(outObj);
    EXPECT_EQ(newObj_ptr, outObj_ptr);
    int x = obj_getInt(self, "outObj.x");
    ArgType newObj_type = arg_getType(newObj);
    ArgType outObj_type = arg_getType(outObj);
    EXPECT_EQ(x, 1);
    EXPECT_EQ(newObj_type, ARG_TYPE_OBJECT);
    EXPECT_EQ(outObj_type, ARG_TYPE_OBJECT);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_DEF_NEW) {
    char* pikaAsm =
        "B0\n"
        "0 DEF testnew()\n"
        "0 JMP 1\n"
        "B1\n"
        "0 RUN TinyObj\n"
        "0 OUT newObj\n"
        "B1\n"
        "0 NUM 1\n"
        "0 OUT newObj.x\n"
        "B1\n"
        "0 NEW newObj\n"
        "0 RET \n"
        "B1\n"
        "0 RET \n"
        "B0\n"
        "0 RUN testnew\n"
        "0 OUT outobj\n";
    PikaObj* self = newRootObj("", New_PikaMain);
    pikaVM_runAsm(self, pikaAsm);
    /* assert */
    PikaObj* outobj = obj_getObj(self, "outobj");
    int x = obj_getInt(outobj, "x");
    EXPECT_EQ(x, 1);
    /* deinit */
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, class_x_1) {
    char* line =
        "class Test(TinyObj):\n"
        "    x = 1\n"
        "test = Test()\n"
        "print('test.x: ' + str(test.x))\n"
        "\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("", New_PikaMain);
    pikaVM_runAsm(self, pikaAsm);

    PikaObj* test = obj_getObj(self, "test");
    Arg* test_arg = obj_getArg(self, "test");
    ArgType test_arg_type = arg_getType(test_arg);
    EXPECT_EQ(test_arg_type, ARG_TYPE_OBJECT);
    int x = obj_getInt(test, "x");
    EXPECT_EQ(x, 1);
    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, nag_a) {
    char* line =
        "a = 1\n"
        "print(-a)\n"
        "b = 0.5\n"
        "print(-b)\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    pikaVM_runAsm(self, pikaAsm);
    /* assert */
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], "-1\r\n");
    EXPECT_STREQ(log_buff[0], "-0.500000\r\n");
    /* deinit */
    obj_deinit(self);
    args_deinit(buffs);
    /* check mem */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(InstructUnit, base) {
    __platform_printf("BEGIN\r\n");
    InstructUnit ins_unit;
    instructUnit_init(&ins_unit);
    instructUnit_setBlockDeepth(&ins_unit, 2);
    instructUnit_setIsNewLine(&ins_unit, 1);
    instructUnit_setInvokeDeepth(&ins_unit, 3);
    instructUnit_setInstruct(&ins_unit, (InstructIndex)4);
    instructUnit_setConstPoolIndex(&ins_unit, 12);

    EXPECT_EQ(instructUnit_getBlockDeepth(&ins_unit), 2);
    EXPECT_EQ(instructUnit_getIsNewLine(&ins_unit), 1);
    EXPECT_EQ(instructUnit_getInvokeDeepth(&ins_unit), 3);
    EXPECT_EQ(instructUnit_getInstructIndex(&ins_unit), 4);
    EXPECT_EQ(instructUnit_getConstPoolIndex(&ins_unit), 12);

    instructUnit_print(&ins_unit);
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], "B2\r\n");
    EXPECT_STREQ(log_buff[0], "3 OUT #12\r\n");
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(ConstPool, get) {
    __platform_printf("BEGIN\r\n");
    ConstPool cp;
    constPool_init(&cp);
    char* next_content;
    uint16_t next_offset;
    next_offset = constPool_getLastOffset(&cp);
    constPool_append(&cp, "hello");
    next_content = constPool_getByOffset(&cp, next_offset);
    EXPECT_STREQ(next_content, "hello");
    next_offset = constPool_getLastOffset(&cp);
    constPool_append(&cp, "world");
    next_content = constPool_getByOffset(&cp, next_offset);
    EXPECT_STREQ(next_content, "world");
    char* first = constPool_getNow(&cp);
    char* second = constPool_getNext(&cp);
    char* third = constPool_getNext(&cp);
    char* forth = constPool_getNext(&cp);
    EXPECT_STREQ(first, "");
    EXPECT_STREQ(second, "hello");
    EXPECT_STREQ(third, "world");
    EXPECT_EQ((uintptr_t)forth, (uintptr_t)NULL);
    EXPECT_STREQ(constPool_getByIndex(&cp, 0), "");
    EXPECT_STREQ(constPool_getByIndex(&cp, 1), "hello");
    EXPECT_STREQ(constPool_getByIndex(&cp, 2), "world");
    EXPECT_EQ((uintptr_t)constPool_getByIndex(&cp, 3), (uintptr_t)NULL);
    constPool_print(&cp);
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], "1: hello\r\n");
    EXPECT_STREQ(log_buff[0], "7: world\r\n");
    constPool_deinit(&cp);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(InstructArray, set) {
    __platform_printf("BEGIN\r\n");
    InstructUnit ins_unit;
    instructUnit_init(&ins_unit);
    instructUnit_setBlockDeepth(&ins_unit, 2);
    instructUnit_setIsNewLine(&ins_unit, 1);
    instructUnit_setInvokeDeepth(&ins_unit, 3);
    instructUnit_setInstruct(&ins_unit, (InstructIndex)4);
    instructUnit_setConstPoolIndex(&ins_unit, 12);

    InstructArray ins_array;
    instructArray_init(&ins_array);
    instructArray_append(&ins_array, &ins_unit);
    instructArray_print(&ins_array);
    instructArray_deinit(&ins_array);
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], "B2\r\n");
    EXPECT_STREQ(log_buff[0], "3 OUT #12\r\n");
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, bytecode_jjcc) {
    char* line = "a = (1 + 1.1) * 3 - 2 /4.0";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    ByteCodeFrame byte_frame;
    byteCodeFrame_init(&byte_frame);
    byteCodeFrame_appendFromAsm(&byte_frame, pikaAsm);
    byteCodeFrame_print(&byte_frame);

    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    // pikaVM_runAsm(self, pikaAsm);
    pikaVM_runByteCodeFrame(self, &byte_frame);

    float res = obj_getFloat(self, "a");

    obj_deinit(self);
    args_deinit(buffs);
    byteCodeFrame_deinit(&byte_frame);

    ASSERT_FLOAT_EQ(res, 5.8);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, WHILE_byte) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =(char *)
        "a = 1\n"
        "b = 0\n"
        "while a:\n"
        "    b = 1\n"
        "    a = 0\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = pika_lines2Asm(buffs, lines);
    printf("%s", pikaAsm);
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* self = newRootObj("root", New_TinyObj);
    pikaVM_run(self, lines);
    EXPECT_EQ(obj_getInt(self, "a"), 0);
    EXPECT_EQ(obj_getInt(self, "b"), 1);
    args_deinit(buffs);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, for_break_byte) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =(char *)
         "a = 0\n"
         "for i in range(0, 10):\n"
         "    if i == 5:\n"
         "        break\n"
         "    a = a + i\n"
         "\n";
    printf("%s", lines);
    char* pikaAsm = pika_lines2Asm(buffs, lines);
    printf("%s", pikaAsm);
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    pikaVM_run(self, lines);
    /* assert */
    int a = obj_getInt(self, "a");
    EXPECT_EQ(a, 10);
    /* deinit */
    args_deinit(buffs);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, load_static_bytes) {
    const uint8_t bytes[] = {
        0x08, 0x02, 0x00, 0x00, /* instruct array size */
        0x00, 0x82, 0x01, 0x00, 0x00, 0x04, 0x0d, 0x00, 0x00, 0x83, 0x11, 0x00,
        0x00, 0x04, 0x16, 0x00, 0x00, 0x85, 0x1b, 0x00, 0x00, 0x04, 0x1d, 0x00,
        0x20, 0x85, 0x1b, 0x00, 0x20, 0x01, 0x0d, 0x00, 0x10, 0x02, 0x28, 0x00,
        0x00, 0x02, 0x2e, 0x00, 0x00, 0x04, 0x33, 0x00, 0x00, 0x01, 0x37, 0x00,
        0x00, 0x01, 0x3b, 0x00, 0x00, 0x01, 0x3f, 0x00, 0x00, 0x04, 0x43, 0x00,
        0x00, 0x04, 0x4a, 0x00, 0x00, 0x04, 0x51, 0x00, 0x00, 0x82, 0x58, 0x00,
        0x00, 0x04, 0x65, 0x00, 0x00, 0x0d, 0x65, 0x00, 0x00, 0x07, 0x67, 0x00,
        0x11, 0x81, 0x0d, 0x00, 0x11, 0x05, 0x1b, 0x00, 0x01, 0x08, 0x69, 0x00,
        0x01, 0x07, 0x6c, 0x00, 0x02, 0x8e, 0x6c, 0x00, 0x11, 0x81, 0x1d, 0x00,
        0x11, 0x05, 0x1b, 0x00, 0x01, 0x08, 0x69, 0x00, 0x01, 0x07, 0x6c, 0x00,
        0x12, 0x81, 0x6e, 0x00, 0x12, 0x01, 0x65, 0x00, 0x02, 0x02, 0x76, 0x00,
        0x02, 0x04, 0x16, 0x00, 0x02, 0x85, 0x6c, 0x00, 0x02, 0x04, 0x1d, 0x00,
        0x01, 0x8b, 0x6c, 0x00, 0x11, 0x01, 0x1d, 0x00, 0x11, 0x05, 0x6c, 0x00,
        0x01, 0x08, 0x69, 0x00, 0x01, 0x07, 0x6c, 0x00, 0x12, 0x81, 0x16, 0x00,
        0x12, 0x03, 0x7e, 0x00, 0x02, 0x08, 0x69, 0x00, 0x02, 0x07, 0x6c, 0x00,
        0x13, 0x81, 0x6e, 0x00, 0x13, 0x01, 0x65, 0x00, 0x03, 0x02, 0x76, 0x00,
        0x03, 0x04, 0x85, 0x00, 0x03, 0x82, 0x85, 0x00, 0x03, 0x85, 0x1b, 0x00,
        0x03, 0x04, 0x1d, 0x00, 0x02, 0x8b, 0x6c, 0x00, 0x12, 0x01, 0x16, 0x00,
        0x12, 0x03, 0x8a, 0x00, 0x02, 0x08, 0x69, 0x00, 0x02, 0x07, 0x6c, 0x00,
        0x13, 0x81, 0x6e, 0x00, 0x13, 0x01, 0x65, 0x00, 0x03, 0x02, 0x76, 0x00,
        0x03, 0x04, 0x8a, 0x00, 0x03, 0x85, 0x67, 0x00, 0x03, 0x04, 0x1d, 0x00,
        0x02, 0x8b, 0x6c, 0x00, 0x12, 0x01, 0x16, 0x00, 0x12, 0x03, 0x8f, 0x00,
        0x02, 0x08, 0x69, 0x00, 0x02, 0x07, 0x6c, 0x00, 0x13, 0x81, 0x6e, 0x00,
        0x13, 0x01, 0x65, 0x00, 0x03, 0x02, 0x76, 0x00, 0x03, 0x04, 0x8f, 0x00,
        0x03, 0x85, 0x67, 0x00, 0x03, 0x04, 0x1d, 0x00, 0x01, 0x8b, 0x6c, 0x00,
        0x11, 0x01, 0x1d, 0x00, 0x11, 0x05, 0x67, 0x00, 0x01, 0x08, 0x69, 0x00,
        0x01, 0x07, 0x6c, 0x00, 0x12, 0x81, 0x16, 0x00, 0x12, 0x03, 0x8a, 0x00,
        0x02, 0x08, 0x69, 0x00, 0x02, 0x07, 0x6c, 0x00, 0x03, 0x82, 0x8a, 0x00,
        0x03, 0x07, 0x6c, 0x00, 0x14, 0x81, 0x6e, 0x00, 0x14, 0x01, 0x65, 0x00,
        0x04, 0x02, 0x76, 0x00, 0x04, 0x04, 0x85, 0x00, 0x04, 0x82, 0x85, 0x00,
        0x03, 0x85, 0x1b, 0x00, 0x03, 0x04, 0x1d, 0x00, 0x02, 0x8b, 0x6c, 0x00,
        0x12, 0x01, 0x16, 0x00, 0x12, 0x03, 0x8f, 0x00, 0x02, 0x08, 0x69, 0x00,
        0x02, 0x07, 0x6c, 0x00, 0x13, 0x81, 0x6e, 0x00, 0x13, 0x01, 0x65, 0x00,
        0x03, 0x02, 0x76, 0x00, 0x03, 0x04, 0x85, 0x00, 0x03, 0x85, 0x99, 0x00,
        0x03, 0x04, 0x1d, 0x00, 0x01, 0x8b, 0x6c, 0x00, 0x11, 0x01, 0x1d, 0x00,
        0x11, 0x05, 0x99, 0x00, 0x01, 0x08, 0x69, 0x00, 0x01, 0x07, 0x6c, 0x00,
        0x12, 0x81, 0x16, 0x00, 0x12, 0x03, 0x8f, 0x00, 0x02, 0x08, 0x69, 0x00,
        0x02, 0x07, 0x6c, 0x00, 0x13, 0x81, 0x9b, 0x00, 0x23, 0x01, 0x6e, 0x00,
        0x23, 0x01, 0x65, 0x00, 0x13, 0x02, 0x76, 0x00, 0x03, 0x08, 0xa2, 0x00,
        0x03, 0x07, 0x6c, 0x00, 0x04, 0x82, 0x85, 0x00, 0x14, 0x81, 0x6e, 0x00,
        0x14, 0x01, 0x65, 0x00, 0x24, 0x01, 0x9b, 0x00, 0x24, 0x01, 0x8f, 0x00,
        0x14, 0x08, 0xa4, 0x00, 0x14, 0x03, 0x6e, 0x00, 0x04, 0x02, 0xa6, 0x00,
        0x03, 0x85, 0x1b, 0x00, 0x03, 0x04, 0x1d, 0x00, 0x00, 0x86, 0xae, 0x00,
        0x00, 0x8c, 0x33, 0x00, /* instruct array */
        0xb1, 0x00, 0x00, 0x00, /* const pool size */
        0x00, 0x5f, 0x5f, 0x63, 0x61, 0x6c, 0x6c, 0x73, 0x2e, 0x6c, 0x65, 0x6e,
        0x00, 0x6c, 0x65, 0x6e, 0x00, 0x6e, 0x6f, 0x6e, 0x65, 0x00, 0x6d, 0x6f,
        0x64, 0x65, 0x00, 0x30, 0x00, 0x69, 0x6e, 0x66, 0x6f, 0x5f, 0x69, 0x6e,
        0x64, 0x65, 0x78, 0x00, 0x72, 0x61, 0x6e, 0x67, 0x65, 0x00, 0x69, 0x74,
        0x65, 0x72, 0x00, 0x5f, 0x6c, 0x30, 0x00, 0x5f, 0x72, 0x31, 0x00, 0x5f,
        0x72, 0x32, 0x00, 0x5f, 0x72, 0x33, 0x00, 0x5f, 0x6c, 0x30, 0x2e, 0x61,
        0x31, 0x00, 0x5f, 0x6c, 0x30, 0x2e, 0x61, 0x32, 0x00, 0x5f, 0x6c, 0x30,
        0x2e, 0x61, 0x33, 0x00, 0x5f, 0x6c, 0x30, 0x2e, 0x5f, 0x5f, 0x6e, 0x65,
        0x78, 0x74, 0x5f, 0x5f, 0x00, 0x69, 0x00, 0x32, 0x00, 0x3d, 0x3d, 0x00,
        0x31, 0x00, 0x5f, 0x5f, 0x63, 0x61, 0x6c, 0x6c, 0x73, 0x00, 0x5f, 0x5f,
        0x67, 0x65, 0x74, 0x5f, 0x5f, 0x00, 0x61, 0x6c, 0x77, 0x61, 0x79, 0x73,
        0x00, 0x74, 0x6f, 0x64, 0x6f, 0x00, 0x77, 0x68, 0x65, 0x6e, 0x00, 0x70,
        0x65, 0x72, 0x69, 0x6f, 0x64, 0x5f, 0x6d, 0x73, 0x00, 0x33, 0x00, 0x5f,
        0x5f, 0x74, 0x69, 0x63, 0x6b, 0x00, 0x3e, 0x00, 0x2b, 0x00, 0x5f, 0x5f,
        0x73, 0x65, 0x74, 0x5f, 0x5f, 0x00, 0x2d, 0x31, 0x00, /* const pool */
    };
    ByteCodeFrame bytecode_frame = {.const_pool =
                                        {
                                            .arg_buff = NULL,
                                            .content_offset_now = 0,
                                            .size = 0,
                                            .content_start = NULL,
                                            .output_redirect_fun = NULL,
                                            .output_f = NULL,
                                        },
                                    .instruct_array = {
                                        .arg_buff = NULL,
                                        .content_offset_now = 0,
                                        .size = 0,
                                        .content_start = NULL,
                                        .output_redirect_fun = NULL,
                                        .output_f = NULL,
                                    }};

    byteCodeFrame_loadByteCode(&bytecode_frame, (uint8_t*)bytes);
    byteCodeFrame_print(&bytecode_frame);
    EXPECT_EQ(instructArray_getSize(&(bytecode_frame.instruct_array)), 520);
    EXPECT_EQ(bytecode_frame.const_pool.size, 177);
    byteCodeFrame_deinit(&bytecode_frame);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, multi_jian) {
    g_PikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines = "a = (3-4) - 4\n";
    printf("%s", lines);
    char* pikaAsm = pika_lines2Asm(buffs, lines);
    printf("%s", pikaAsm);
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    pikaVM_run(self, lines);
    /* assert */
    int a = obj_getInt(self, "a");
    EXPECT_EQ(a, -5);
    /* deinit */
    args_deinit(buffs);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, _3_p_3) {
    char* line = "-3+3";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    __platform_printf("BEGIN\r\n");
    pikaVM_runAsm(self, pikaAsm);
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "0\r\n");
    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, _3_3) {
    char* line = "-3-3";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    __platform_printf("BEGIN\r\n");
    pikaVM_runAsm(self, pikaAsm);
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "-6\r\n");
    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_3) {
    char* line = "a-3";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    __platform_printf("BEGIN\r\n");
    pikaVM_runAsm(self, pikaAsm);
    EXPECT_STREQ(log_buff[3], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[2], "NameError: name 'a' is not defined\r\n");
    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, hex_str) {
    char* line = "a = '\\x33\\x35'";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    __platform_printf("BEGIN\r\n");
    pikaVM_runAsm(self, pikaAsm);
    /* collect */
    char* a = obj_getStr(self, "a");
    /* assert */
    EXPECT_STREQ(a, "35");
    /* deinit */
    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, hex_bytes) {
    char* line =
        "a = b'\\x03\\x05'\n"
        "a\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    __platform_printf("BEGIN\r\n");
    pikaVM_runAsm(self, pikaAsm);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "b'\\x03\\x05'\r\n");
    /* deinit */
    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, bytes_equ) {
    char* line =
        "a = b'\\x03\\x05'\n"
        "b = b'\\x03\\x05'\n"
        "if a == b:\n"
        "    c = 1\n"
        "b = b'\\x02\\x05'\n"
        "if a != b:\n"
        "    d = 1\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    __platform_printf("BEGIN\r\n");
    pikaVM_runAsm(self, pikaAsm);
    /* collect */
    /* assert */
    int c = obj_getInt(self, "c");
    int d = obj_getInt(self, "d");
    EXPECT_EQ(c, 1);
    EXPECT_EQ(d, 1);
    /* deinit */
    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_BUILTIN_STRUCT_ENABLE
TEST(VM, tuple_literal) {
    char* line = "(1,2,3)\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    __platform_printf("BEGIN\r\n");
    pikaVM_runAsm(self, pikaAsm);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "(1, 2, 3)\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(VM, dvd_opt) {
    char* line = "a = 10%4\n";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    int a = obj_getInt(self, "a");
    /* assert */
    EXPECT_EQ(a, 2);
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, del_) {
    char* line =
        "a = 1\n"
        "print(a)\n"
        "del a\n"
        "print(a)\n";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    /* assert */
    EXPECT_EQ(obj_isArgExist(self, "a"), 0);
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, is) {
    char* line =
        "a = 1\n"
        "b = a\n"
        "res1 = a is b\n"
        "res2 = 1 is 1\n"
        "c = TinyObj()\n"
        "d = c\n"
        "res3 = c is d\n"
        "res4 = c is None\n";
    ;
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    /* assert */
    EXPECT_EQ(obj_getBool(self, "res1"), PIKA_TRUE);
    EXPECT_EQ(obj_getBool(self, "res2"), PIKA_TRUE);
    EXPECT_EQ(obj_getBool(self, "res3"), PIKA_TRUE);
    EXPECT_EQ(obj_getBool(self, "res4"), PIKA_FALSE);
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, in) {
    char* line =
        "res1 = 'a' in 'aaa'\n"
        "res2 = 'aaa' in 'a'\n";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    /* assert */
    EXPECT_EQ(obj_getInt(self, "res1"), 1);
    EXPECT_EQ(obj_getInt(self, "res2"), 0);
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(VM, assert) {
    char* line =
        "assert True\n"
        "assert 1 == 1, 'testparser'\n"
        "res1 = 0\n"
        "try:\n"
        "    assert False\n"
        "except:\n"
        "    res1 = 1\n";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    int res1 = obj_getInt(self, "res1");
    /* assert */
    EXPECT_EQ(res1, 1);
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(VM, issue_I5LHJG) {
    char* line =
        "res = (a, 2, 3)\n"
        "[a, 2, 3]\n"
        "{'x': a, 'y': 2, 'z': 3}\n";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    /* assert */
    EXPECT_EQ(obj_isArgExist(self, "res"), 0);
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, vars_runtime) {
    char* line =
        "def testvars(a, *b):\n"
        "    sum = 0\n"
        "    for i in b:\n"
        "        sum += i\n"
        "    return a * sum\n"
        "res = testvars(6, 2, 3, 4, 5)\n";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    int res = obj_getInt(self, "res");
    /* assert */
    EXPECT_EQ(res, 84);
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_BUILTIN_STRUCT_ENABLE
TEST(VM, list_add) {
    char* line = "[1, 2, 3] + [4, 5, 6]";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "[1, 2, 3, 4, 5, 6]\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(VM, science_num) {
    char* line =
        "a = 1.0e-3\n"
        "b = 2e-5\n"
        "c = -3e-5\n"
        "d = 0.4e2\n";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    double a = obj_getFloat(self, "a");
    double b = obj_getFloat(self, "b");
    double c = obj_getFloat(self, "c");
    double d = obj_getFloat(self, "d");
/* assert */
#if PIKA_FLOAT_TYPE_DOUBLE
    EXPECT_DOUBLE_EQ(a, 1.0e-3);
    EXPECT_DOUBLE_EQ(b, 2.0e-5);
    EXPECT_DOUBLE_EQ(c, -3e-5);
    EXPECT_DOUBLE_EQ(d, 0.4e2);
#else
    EXPECT_FLOAT_EQ(a, 1.0e-3);
    EXPECT_FLOAT_EQ(b, 2.0e-5);
    EXPECT_FLOAT_EQ(c, -3e-5);
    EXPECT_FLOAT_EQ(d, 0.4e2);
#endif
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, issue_I5OJQB) {
    char* line = "s = '\\\\'";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    char* s = obj_getStr(self, "s");
    /* assert */
    EXPECT_STREQ(s, "\\");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(vm, keyword_2) {
    char* line =
        "def test(a, b):\n"
        "    print(a, b)\n"
        "test(a=1, b= 2)";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "1 2\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, keyword_3) {
    char* line =
        "def test(**keys):\n"
        "    print(keys['a'], keys['b'])\n"
        "test(a=1, b= 2)";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "1 2\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, keyword_4) {
    char* line =
        "def test(a, b, **keys):\n"
        "    print(keys['c'], keys['d'], a, b)\n"
        "test(3, 4, c=1, d= 2)";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "1 2 3 4\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, class_keyword) {
    char* line =
        "class Test:\n"
        "    def __init__(self, a, b = 1, c =2):\n"
        "        print(a, b, c)\n"
        "t = Test(0, b = 3)\n";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "0 3 2\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, class_keyword_mqtt) {
    char* line =
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
        "        print('ip:', ip)\n"
        "        print('port:', port)\n"
        "c = MQTT('test')\n";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "ip: test\r\n");
    EXPECT_STREQ(log_buff[0], "port: 1883\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, vars_keyward) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/function.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "1 2 3 4 5 10\r\n");
    EXPECT_STREQ(log_buff[0], "1 2 3 4 5 12\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, cb_1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "../../examples/Callback/test1.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[3], "__init__\r\n");
    EXPECT_STREQ(log_buff[1], "a\r\n");
    EXPECT_STREQ(log_buff[0], "b\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, cb_2) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "../../examples/Callback/test2.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[7], "__init__\r\n");
    EXPECT_STREQ(log_buff[5], "a\r\n");
    EXPECT_STREQ(log_buff[4], "a\r\n");
    EXPECT_STREQ(log_buff[3], "b\r\n");
    EXPECT_STREQ(log_buff[2], "b\r\n");
    EXPECT_STREQ(log_buff[1], "ppp\r\n");
    EXPECT_STREQ(log_buff[0], "ppp\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, cb_3) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/callback/test3.py");
    /* collect */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif

#if !PIKA_NANO_ENABLE
TEST(vm, default_no_input) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def test(a = 1):\n"
            "    print(a)\n"
            "test()");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "1\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, default_1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def test(a = 1):\n"
            "    print(a)\n"
            "test(a = 2)");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "2\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, default_2) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def test(a, b = 1):\n"
            "    print(a, b)\n"
            "test(1, b = 2)");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "1 2\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, default_3) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def myprint(end='\\n', *var):\n"
            "    for v in var:\n"
            "        print(v, end = end)\n"
            "myprint(1, 2)\n"
            "myprint(1, 2, end=';\\n')\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[3], "1\n");
    EXPECT_STREQ(log_buff[2], "2\n");
    EXPECT_STREQ(log_buff[1], "1;\n");
    EXPECT_STREQ(log_buff[0], "2;\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, default_4) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def test(end=5, *var):\n"
            "    res = end\n"
            "    print('end', end)\n"
            "    for v in var:\n"
            "        res += v\n"
            "    return res\n"
            "test(1, 2, 3)\n"
            "test(1, 2, 3, end=6)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[3], "end 5\r\n");
    EXPECT_STREQ(log_buff[2], "11\r\n");
    EXPECT_STREQ(log_buff[1], "end 6\r\n");
    EXPECT_STREQ(log_buff[0], "12\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, default_no_kw) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def test(a, b=3):\n"
            "    print(a ,b)\n"
            "test(1, 2)\n"
            "test(1, b=2)\n"
            "test(1)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[2], "1 2\r\n");
    EXPECT_STREQ(log_buff[1], "1 2\r\n");
    EXPECT_STREQ(log_buff[0], "1 3\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif

TEST(vm, none) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "GTestTask.test(1, None)");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "GTestTask_test: 1, 0\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(vm, super_) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "class test:\n"
            "    def __init__(self):\n"
            "        print('in test init')\n"
            "class test2(test):\n"
            "    def __init__(self):\n"
            "        super().__init__()\n"
            "        print('in test2 init')\n"
            "t = test2()");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "in test2 init\r\n");
    EXPECT_STREQ(log_buff[1], "in test init\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, super_val) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "class test:\n"
            "    def __init__(self):\n"
            "        self.a = 1\n"
            "        self.b = 2\n"
            "class test2(test):\n"
            "    def __init__(self):\n"
            "        super().__init__()\n"
            "        self.c = 3\n"
            "        self.d = 4\n"
            "t = test2()\n"
            "print(t.a, t.b, t.c, t.d)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "1 2 3 4\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, super_val_) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "class test:\n"
            "    def test(self, a):\n"
            "        self.a = a\n"
            "class test2(test):\n"
            "    def test(self, a):\n"
            "        super().test(str(a))\n"
            "t1 = test()\n"
            "t2 = test2()\n"
            "t1.test(1)\n"
            "t2.test(1)\n");
    /* collect */
    int t1_a = obj_getInt(pikaMain, "t1.a");
    char* t2_a = obj_getStr(pikaMain, "t2.a");
    /* assert */
    EXPECT_EQ(t1_a, 1);
    EXPECT_STREQ(t2_a, "1");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif

#if !PIKA_NANO_ENABLE
TEST(vm, multi_return) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "a,b = (1,2)");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    int b = obj_getInt(pikaMain, "b");
    /* assert */
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 2);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(vm, multi_return_fn) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def test():\n"
            "    return 1,2\n"
            "a,b = test()");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    int b = obj_getInt(pikaMain, "b");
    /* assert */
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 2);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(vm, range_1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "sum = 0\n"
            "for i in range(10):\n"
            "    sum += i\n");
    /* collect */
    int sum = obj_getInt(pikaMain, "sum");
    /* assert */
    EXPECT_EQ(sum, 45);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, rang_3) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "sum = 0\n"
            "for i in range(-5, 20, 2):\n"
            "    sum += i\n");
    /* collect */
    int sum = obj_getInt(pikaMain, "sum");
    /* assert */
    EXPECT_EQ(sum, 91);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, test64) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "import GTestTask\n"
            "res = GTestTask.test64(4294967295, 20)\n");
    /* collect */
    int64_t res = obj_getInt(pikaMain, "res");
    /* assert */
    EXPECT_EQ(res, 4294967295 * 20);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(vm, exit) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "i = 0\n"
            "while True:\n"
            "    i += 1\n"
            "    print(i)\n"
            "    if i == 10:\n"
            "        exit()\n");
    /* collect */
    int i = obj_getInt(pikaMain, "i");
    /* assert */
    EXPECT_EQ(i, 10);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, exit_fn) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "i = 0\n"
            "PikaStdLib.MemChecker.now()\n"
            "def test():\n"
            "    global i\n"
            "    while True:\n"
            "        i += 1\n"
            "        print(i)\n"
            "        if i == 10:\n"
            "            exit()\n"
            "while True:\n"
            "    test()\n");
    obj_run(pikaMain, "PikaStdLib.MemChecker.now()\n");
    /* collect */
    int i = obj_getInt(pikaMain, "i");
    /* assert */
    EXPECT_EQ(i, 10);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif
#if PIKA_INSTRUCT_HOOK_ENABLE
typedef void (*hook_func)(void);
extern volatile int g_hook_cnt;
extern volatile hook_func g_hook_func;
extern "C" {
volatile int g_hook_cnt_triggle;
void hook_func_exit_issue_1(void) {
    if (g_hook_cnt >= g_hook_cnt_triggle) {
        pks_vm_exit();
    }
}
void __gtest_hook_default_(void) {
    return;
}
}

void _vm_exit_fn_issue_1_item(int hook_cnt) {
    g_hook_cnt_triggle = hook_cnt;
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "PikaStdLib.MemChecker.now()\n"
            "def task1():\n"
            "    print('task1 hello')\n"
            "\n"
            "def task2():\n"
            "    print('task2 hello')\n"
            "\n"
            "def fake_runtask():\n"
            "    i = 0\n"
            "    while True:\n"
            "        i += 1\n"
            "        print(i)\n"
            "        task1()\n"
            "        task2()\n"
            "fake_runtask()\n"
            "fake_runtask()\n"
            "\n");
    obj_run(pikaMain, "PikaStdLib.MemChecker.now()\n");
    /* collect */
    /* assert */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, exit_fn_issue_1) {
    g_hook_func = hook_func_exit_issue_1;
    for (int i = 0; i < 50; i++) {
        g_hook_cnt = 0;
        _vm_exit_fn_issue_1_item(i);
    }
    g_hook_func = __gtest_hook_default_;
}
#endif

TEST(vm, pass_) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "pass\n"
            "def testpass():\n"
            "    pass\n"
            "for i in range(10):\n"
            "    pass\n"
            "testpass()\n"
            "print('after pass')\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "after pass\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, test64_hex) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "import GTestTask\n"
            "res = GTestTask.test64(0xffffffff, 20)\n");
    /* collect */
    int64_t res = obj_getInt(pikaMain, "res");
    /* assert */
    EXPECT_EQ(res, 4294967295 * 20);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, test64_hex_print) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "0xffffffff\n"
            "print(0xffffffff)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "4294967295\r\n");
    EXPECT_STREQ(log_buff[1], "4294967295\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(vm, call_dict_err) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "a = {'a': 1}\n"
            "a('a')\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[4], "TypeError: 'a' object is not callable\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, getattribute) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "class test:\n"
            "    def __getattribute__(self, name):\n"
            "        return name\n"
            "t = test()\n"
            "t.a\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "'a'\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, getattr) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "class test:\n"
            "    a =  1\n"
            "    def __getattr__(self, name):\n"
            "        return name\n"
            "t = test()\n"
            "t.a\n"
            "t.b\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "'b'\r\n");
    EXPECT_STREQ(log_buff[1], "1\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, setattr) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "class test:\n"
            "    a =  1\n"
            "    def __setattr__(self, name, val):\n"
            "        print((name, val))\n"
            "t = test()\n"
            "t.a = 1\n"
            "t.b = 'test'\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], "('a', 1)\r\n");
    EXPECT_STREQ(log_buff[0], "('b', 'test')\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, c_module_get_set_attr) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "t = GTestTask.ProxyTest()\n"
            "t.a\n"
            "t.b\n"
            "t.a = 'test1'\n"
            "t.b = 'test2'\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[3], "'a'\r\n");
    EXPECT_STREQ(log_buff[2], "'b'\r\n");
    EXPECT_STREQ(log_buff[1], "GTestTask_ProxyTest___setattr__: a, test1\r\n");
    EXPECT_STREQ(log_buff[0], "GTestTask_ProxyTest___setattr__: b, test2\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, class_attr_ref) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "class test:\n"
            "    a = 1\n"
            "    b = a\n"
            "t = test()\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, getattr_native) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "mem = PikaStdLib.MemChecker()\n"
            "if hasattr(mem, 'max'):\n"
            "    max = getattr(mem, 'max')\n"
            "    max()\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, issue_dict_update) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/issue/issue_dict_update.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(obj_getStr(pikaMain, "inner"), "widget5");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, issue_big_dict_update) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain,
                         "test/python/issue/issue_big_dict_update.py");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif

TEST(vm, i_pp) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    /* clang-format off */
    PIKA_PYTHON(
    i = 0
    while i < 10000:
        i += 1
    )
    /* clang-format on */
    const uint8_t bytes[] = {
        0x30, 0x00, 0x00, 0x00, /* instruct array size */
        0x00, 0x85, 0x01, 0x00, 0x00, 0x04, 0x03, 0x00, 0x10, 0x81, 0x03, 0x00,
        0x10, 0x05, 0x05, 0x00, 0x00, 0x08, 0x0b, 0x00, 0x00, 0x07, 0x0d, 0x00,
        0x11, 0x81, 0x03, 0x00, 0x21, 0x05, 0x0f, 0x00, 0x11, 0x02, 0x00, 0x00,
        0x01, 0x08, 0x11, 0x00, 0x01, 0x04, 0x03, 0x00, 0x00, 0x86, 0x13, 0x00,
        /* instruct array */
        0x16, 0x00, 0x00, 0x00, /* const pool size */
        0x00, 0x30, 0x00, 0x69, 0x00, 0x31, 0x30, 0x30, 0x30, 0x30, 0x00, 0x3c,
        0x00, 0x32, 0x00, 0x31, 0x00, 0x2b, 0x00, 0x2d, 0x31,
        0x00, /* const pool */
    };
    pikaVM_runByteCode(pikaMain, (uint8_t*)bytes);
    EXPECT_EQ(obj_getInt(pikaMain, "i"), 10000);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, benchmark) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    /* clang-format off */
    PIKA_PYTHON(
    num = 0
    i = 2
    while i < 1000:
        is_prime = 1
        j = 2
        while j < i:
            if i%j==0 :
                is_prime = 0
                break
            j += 1 
        if is_prime:
            num = num + i
        i += 1
    )
    /* clang-format on */
    const uint8_t bytes[] = {
        0xac, 0x00, 0x00, 0x00, /* instruct array size */
        0x00, 0x85, 0x01, 0x00, 0x00, 0x04, 0x03, 0x00, 0x00, 0x85, 0x07, 0x00,
        0x00, 0x04, 0x09, 0x00, 0x10, 0x81, 0x09, 0x00, 0x10, 0x05, 0x0b, 0x00,
        0x00, 0x08, 0x10, 0x00, 0x00, 0x07, 0x07, 0x00, 0x01, 0x85, 0x12, 0x00,
        0x01, 0x04, 0x14, 0x00, 0x01, 0x85, 0x07, 0x00, 0x01, 0x04, 0x1d, 0x00,
        0x11, 0x81, 0x1d, 0x00, 0x11, 0x01, 0x09, 0x00, 0x01, 0x08, 0x10, 0x00,
        0x01, 0x07, 0x07, 0x00, 0x22, 0x81, 0x09, 0x00, 0x22, 0x01, 0x1d, 0x00,
        0x12, 0x08, 0x1f, 0x00, 0x12, 0x05, 0x01, 0x00, 0x02, 0x08, 0x21, 0x00,
        0x02, 0x07, 0x12, 0x00, 0x03, 0x85, 0x01, 0x00, 0x03, 0x04, 0x14, 0x00,
        0x03, 0x8e, 0x00, 0x00, 0x12, 0x81, 0x1d, 0x00, 0x22, 0x05, 0x12, 0x00,
        0x12, 0x02, 0x00, 0x00, 0x02, 0x08, 0x24, 0x00, 0x02, 0x04, 0x1d, 0x00,
        0x01, 0x86, 0x26, 0x00, 0x01, 0x81, 0x14, 0x00, 0x01, 0x07, 0x12, 0x00,
        0x12, 0x81, 0x03, 0x00, 0x12, 0x01, 0x09, 0x00, 0x02, 0x08, 0x24, 0x00,
        0x02, 0x04, 0x03, 0x00, 0x11, 0x81, 0x09, 0x00, 0x21, 0x05, 0x12, 0x00,
        0x11, 0x02, 0x00, 0x00, 0x01, 0x08, 0x24, 0x00, 0x01, 0x04, 0x09, 0x00,
        0x00, 0x86, 0x26, 0x00, /* instruct array */
        0x29, 0x00, 0x00, 0x00, /* const pool size */
        0x00, 0x30, 0x00, 0x6e, 0x75, 0x6d, 0x00, 0x32, 0x00, 0x69, 0x00, 0x31,
        0x30, 0x30, 0x30, 0x00, 0x3c, 0x00, 0x31, 0x00, 0x69, 0x73, 0x5f, 0x70,
        0x72, 0x69, 0x6d, 0x65, 0x00, 0x6a, 0x00, 0x25, 0x00, 0x3d, 0x3d, 0x00,
        0x2b, 0x00, 0x2d, 0x31, 0x00, /* const pool */
    };
    pikaVM_runByteCode(pikaMain, (uint8_t*)bytes);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, print_None) {
    char* line = "print(None)\n";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "None\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, bc_fn) {
    PikaObj* self = newRootObj("root", New_PikaMain);
    obj_run(self, "print('hello world')\n");
    /* clang-format off */
    PIKA_PYTHON(
    def test():
        print('test')

    )
    /* clang-format on */
    const uint8_t
        bytes[] =
            {
                0x14, 0x00, 0x00, 0x00, /* instruct array size */
                0x00, 0x89, 0x01, 0x00, 0x00, 0x06, 0x08,
                0x00, 0x11, 0x83, 0x0a, 0x00, 0x01, 0x02,
                0x0f, 0x00, 0x01, 0x8a, 0x00, 0x00, /* instruct array */
                0x15, 0x00, 0x00, 0x00,             /* const pool size */
                0x00, 0x74, 0x65, 0x73, 0x74, 0x28, 0x29,
                0x00, 0x31, 0x00, 0x74, 0x65, 0x73, 0x74,
                0x00, 0x70, 0x72, 0x69, 0x6e, 0x74, 0x00, /* const pool */
            };
    pikaVM_runByteCode(self, (uint8_t*)bytes);
    obj_run(self, "test()\n");
    EXPECT_STREQ(log_buff[1], "hello world\r\n");
    EXPECT_STREQ(log_buff[0], "test\r\n");
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(VM, default_num_err) {
    char* line =
        "def test(a, b=1):\n"
        "    print(a, b)\n"
        "test(1, 2, 3)\n";
    PikaObj* self = newRootObj("root", New_PikaStdLib_SysObj);
    obj_run(self, line);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[8],
                 "TypeError: test() takes from 1 to 2 positional arguments but "
                 "3 were given\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(VM, bc_fn_file_cb0) {
    PikaObj* self = newRootObj("root", New_PikaMain);
    pikaVM_runByteCodeFile(self,
                           "package/pikascript/pikascript-api/cb_test.py.o");
    obj_run(self, "test()\n");
    EXPECT_STREQ(log_buff[0], "test\r\n");
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, bc_fn_file_cb2) {
    PikaObj* self = newRootObj("root", New_PikaMain);
    obj_run(self, "print('hello world')\n");
    pikaVM_runByteCodeFile(self,
                           "package/pikascript/pikascript-api/cb_test.py.o");
    obj_run(self, "test()\n");
    EXPECT_STREQ(log_buff[1], "hello world\r\n");
    EXPECT_STREQ(log_buff[0], "test\r\n");
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(vm, slice_str_end) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "'test'[:-1]");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "'tes'\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, slice_list) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "[1,2,3][1:]");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "[2, 3]\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, slice_list_list) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "l = "
            "[[\"ID\",\"TCP_URL\",\"VERSION\",\"SENSOR_SCAN_s\",\"DATA_UPLOAD_"
            "s\",\"ACT_LOGIC_SCAN_s\",\"NETWORK\",\"AUTO_ACT\",\"START_TIME\"],"
            "[\"ABC123\",\"iot.365sn.cn/operate/h/"
            "1234\",\"V1.1\",60,1800,0.5,\"4G\",\"TRUE\",1669017826]]\n"
            "ll = l[1:]\n"
            "print(len(l), l)\n"
            "print(len(ll),ll)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0],
                 "9 ['ABC123', 'iot.365sn.cn/operate/h/1234', 'V1.1', 60, "
                 "1800, 0.500000, '4G', 'TRUE', 1669017826]\r\n");
    EXPECT_STREQ(log_buff[1],
                 "2 [['ID', 'TCP_URL', 'VERSION', 'SENSOR_SCAN_s', "
                 "'DATA_UPLOAD_s', 'ACT_LOGIC_SCAN_s', 'NETWORK', 'AUTO_ACT', "
                 "'START_TIME'], ['ABC123', 'iot.365sn.cn/operate/h/1234', "
                 "'V1.1', 60, 1800, 0.500000, '4G', 'TRUE', 1669017826]]\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif

#if !PIKA_NANO_ENABLE
TEST(vm, fn_pos_kw) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def test(a, b):\n"
            "    print(a, b)\n"
            "test(b=1,a=2)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "2 1\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, fn_pos_kw2) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def test(a, b):\n"
            "    print(a, b)\n"
            "test(1, b=2)\n"
            "test(1, a=2)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "1 2\r\n");
    EXPECT_STREQ(log_buff[0], "2 1\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, fn_star) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "a = (1,2,3)\n"
            "print(*a)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "1 2 3\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, fn_star_pos) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "a = (1,2,3)\n"
            "print(*a, 4)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "1 2 3 4\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, fn_star_pos_2) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "a = (1,2,3)\n"
            "print(0, *a, 4)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "0 1 2 3 4\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, fn_star_star) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def test(**kw):\n"
            "    print(kw['a'], kw['b'], kw['c'])\n"
            "a = {'a':1,'b':2,'c':3}\n"
            "test(**a)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "1 2 3\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, issue_keyword_mem_leak) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    pikaVM_runSingleFile(pikaMain,
                         "test/python/issue/issue_keyword_mem_leak.py");
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, issue_not_in) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    obj_run(pikaMain,
            "a = [1,2,3]\n"
            "print(1 not in a)\n"
            "print(4 not in a)\n");
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, def_not_in) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    obj_run(pikaMain,
            "t = ['hmac-md5', 'other']\n"
            "def test(a=\"hmac-md5\"):\n"
            "    print(\"input:\", a)\n"
            "    print(\"table:\", t)\n"
            "    if a not in t:\n"
            "        print(\"a not in t\")\n"
            "    else:\n"
            "        print(\"a in t\")\n"
            "test()\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "a in t\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, fn_pos_kw_issue1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def new(a=1, b=2):\n"
            "    print(a, b)\n"
            "new(3, b = 2)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "3 2\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, fn_pos_kw_issue2) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def new(key:any, msg=None, digestmod=\"md5\"):\n"
            "    print(key, msg, digestmod)\n"
            "new('123','456',digestmod=\"md5\")\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "123 456 md5\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, num_issue_lakj) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "a = 1/2*3\n");
    /* collect */
    /* assert */
    EXPECT_DOUBLE_EQ(obj_getFloat(pikaMain, "a"), 1.5);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, dir_issue) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "class test:\n"
            "    pass\n"
            "dir(test)\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, dir_issue1lk) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "import requests\n"
            "dir(requests)\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, type_int) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "i = type(1)\n"
            "ii = i(10)\n");
    /* collect */
    EXPECT_EQ(obj_getInt(pikaMain, "ii"), 10);
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, method_int) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "i = int\n"
            "ii = i(10)\n");
    /* collect */
    /* assert */
    EXPECT_EQ(obj_getInt(pikaMain, "ii"), 10);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, fn_method_int) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def test():\n"
            "    i = int\n"
            "    ii = i(10)\n"
            "    return ii\n"
            "ii = test()\n");
    /* collect */
    /* assert */
    EXPECT_EQ(obj_getInt(pikaMain, "ii"), 10);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, kw_no_empty) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def test(a=None, b=None, c=None):\n"
            "    print(a, b, c)\n"
            "test(1, 2)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "1 2 None\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, tuple_void) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "()\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "()\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, run_file) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    pikaVM_runFile(pikaMain, "package/pikascript/main.py");
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, bool_) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain, "print(True)\n");
    obj_deinit(pikaMain);
    EXPECT_STREQ(log_buff[0], "True\r\n");
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, method_cb) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain,
            "class Test:\n"
            "    _cb = None\n"
            "    def callback(self):\n"
            "        print('test')\n"
            "    def addcb(self, cb):\n"
            "        self._cb = cb\n"
            "    def runcb(self):\n"
            "        self._cb()\n"
            "    def __init__(self):\n"
            "        self.addcb(self.callback)\n"
            "t = Test()\n"
            "t.runcb()");
    obj_deinit(pikaMain);
    EXPECT_STREQ(log_buff[0], "test\r\n");
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, class_getattr) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/callback/class_getattr.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "2\r\n");
    EXPECT_STREQ(log_buff[1], "1\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, type_fullfealure) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/builtin/type.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "PASS\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, dir_print_arg) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "print(dir)\r\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "<built-in function dir>\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, fn_pos_vars) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/builtin/fn_pos_vars.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(
        log_buff[4],
        "TypeError: log() takes 2 positional argument but 1 were given\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, for_return) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/builtin/for_return.py");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(vm, slice_issue) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/builtin/slice.py");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "PASS\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif

TEST_END