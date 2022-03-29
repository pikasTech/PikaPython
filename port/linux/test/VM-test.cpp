#include "gtest/gtest.h"
extern "C" {
#include "BaseObj.h"
#include "PikaMain.h"
#include "PikaMath_Operator.h"
#include "PikaParser.h"
#include "PikaStdLib_SysObj.h"
#include "PikaVM.h"
#include "dataMemory.h"
#include "dataQueue.h"
#include "dataStrs.h"
#include "pika_config_gtest.h"
extern char log_buff[LOG_BUFF_MAX][LOG_SIZE];
/* test head */
VMParameters* pikaVM_runAsmWithPars(PikaObj* self,
                                    VMParameters* locals,
                                    VMParameters* globals,
                                    char* pikaAsm);
char* Parser_LineToAsm(Args* buffs, char* line, Stack* blockStack);
char* Parser_multiLineToAsm(Args* outBuffs, char* multiLine);
int32_t __clearInvokeQueues(VMParameters* locals);
int32_t pikaVM_runAsmLine(PikaObj* self,
                          VMParameters* locals,
                          VMParameters* globals,
                          char* pikaAsm,
                          int32_t lineAddr);
}

TEST(VM, num1) {
    char* line = (char*)"1";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    pikaVM_runAsm(self, pikaAsm);
    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_1) {
    char* line = (char*)"a = 1";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    pikaVM_runAsm(self, pikaAsm);
    ASSERT_EQ(args_getInt(self->list, (char*)"a"), 1);

    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_1d1) {
    char* line = (char*)"a = 1.1";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);
    ASSERT_FLOAT_EQ(args_getFloat(globals->list, (char*)"a"), 1.1);

    obj_deinit(self);
    // obj_deinit(globals);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, str_xy) {
    char* line = (char*)"a = 'xy'";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    ASSERT_STREQ(args_getStr(globals->list, (char*)"a"), (char*)"xy");

    obj_deinit(self);
    // obj_deinit(globals);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, str_xy_space) {
    char* line = (char*)"a = 'xy '";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    ASSERT_STREQ(args_getStr(globals->list, (char*)"a"), (char*)"xy ");

    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, ref_a_b) {
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    Args* buffs = New_strBuff();

    pikaVM_runAsm(self, Parser_LineToAsm(buffs, (char*)"a = 'xy '", NULL));

    args_deinit(buffs);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, Run_add) {
    PikaObj* self = newRootObj((char*)"root", New_PikaMath_Operator);
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, (char*)"a = plusInt(1,2)", NULL);
    __platform_printf((char*)"%s", pikaAsm);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    args_deinit(buffs);
    int a = args_getInt(globals->list, (char*)"a");
    ASSERT_EQ(a, 3);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, Run_add_multy) {
    PikaObj* self = newRootObj((char*)"root", New_PikaMath_Operator);
    Args* buffs = New_strBuff();

    pikaVM_runAsm(self, Parser_LineToAsm(buffs, (char*)"b = 2", NULL));

    args_deinit(buffs);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, Run_add_1_2_3) {
    PikaObj* self = newRootObj((char*)"root", New_PikaMath_Operator);
    Args* buffs = New_strBuff();
    char* pikaAsm =
        Parser_LineToAsm(buffs, (char*)"a = plusInt(1, plusInt(2,3) )", NULL);
    __platform_printf((char*)"%s", pikaAsm);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    args_deinit(buffs);
    int a = args_getInt(globals->list, (char*)"a");
    ASSERT_EQ(a, 6);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

extern PikaMemInfo pikaMemInfo;
TEST(VM, WHILE) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =(char *)
        "a = 1\n"
        "b = 0\n"
        "while a:\n"
        "    b = 1\n"
        "    a = 0\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* self = New_TinyObj(NULL);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);
    EXPECT_EQ(args_getInt(globals->list, (char*)"a"), 0);
    EXPECT_EQ(args_getInt(globals->list, (char*)"b"), 1);
    // obj_deinit(globals);
    args_deinit(buffs);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_1_1) {
    char* line = (char*)"a = 1 + 1";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int res = args_getInt(globals->list, (char*)"a");
    obj_deinit(self);
    // obj_deinit(globals);
    args_deinit(buffs);
    ASSERT_EQ(res, 2);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_1_1d1) {
    char* line = (char*)"a = 1 + 1.1";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    float res = args_getFloat(globals->list, (char*)"a");
    obj_deinit(self);
    // obj_deinit(globals);
    args_deinit(buffs);
    ASSERT_FLOAT_EQ(res, 2.1);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, a_jjcc) {
    char* line = (char*)"a = (1 + 1.1) * 3 - 2 /4.0";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    float res = args_getFloat(globals->list, (char*)"a");
    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    ASSERT_FLOAT_EQ(res, 5.8);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, while_a_1to10) {
    char* line = (char*)
    "a = 0\n"
    "while a < 10:\n"
    "    a = a + 1\n"
    "    print(a)\n"
    "\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_multiLineToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int res = args_getInt(globals->list, (char*)"a");
    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    ASSERT_FLOAT_EQ(res, 10);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, mem_x) {
    char* line = (char*)
    "mem = PikaStdLib.MemChecker()\n"
    "mem.x = 1\n"
    "print(mem.x)\n"
    "mem.x = 2\n"
    "print(mem.x)\n"
    "\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_multiLineToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"", New_PikaMain);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int res = obj_getInt(globals, (char*)"mem.x");
    EXPECT_EQ(2, res);
    obj_deinit(self);
    // obj_deinit(globals);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RET_instruct) {
    char* pikaAsm = (char*)
    "B1\n"
    "0 NUM 13\n"
    "0 RET \n"
    "0 NUM 2\n"
    "0 RET \n"
    "B0\n";
    PikaObj* self = New_TinyObj(NULL);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);
    Arg* returnArg = args_getArg(globals->list, (char*)"return");
    int num = arg_getInt(returnArg);
    EXPECT_EQ(num, 13);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_DEF) {
    char* pikaAsm = (char*)
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
    int num = obj_getInt(globals, (char*)"a");
    EXPECT_EQ(num, 1);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_global) {
    char* pikaAsm = (char*)
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
    int a = obj_getInt(self, (char*)"a");
    int b = obj_getInt(self, (char*)"b");
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 1);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_local_b) {
    char* pikaAsm = (char*)
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
    PikaObj* self = newRootObj((char*)"", New_BaseObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);
    int a = obj_getInt(globals, (char*)"a");
    int b = obj_getInt(globals, (char*)"b");
    int c = obj_getInt(globals, (char*)"c");
    EXPECT_EQ(a, 1);
    /* b is local, should not be exist in globals */
    EXPECT_EQ(b, -999999999);
    EXPECT_EQ(c, 2);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_DEF_add) {
    char* pikaAsm = (char*)
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
    PikaObj* self = newRootObj((char*)"", New_BaseObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);
    int a = obj_getInt(globals, (char*)"a");
    int b = obj_getInt(globals, (char*)"b");
    int c = obj_getInt(globals, (char*)"c");
    /* a is local, should not be exist in globals */
    EXPECT_EQ(a, -999999999);
    /* b is local, should not be exist in globals */
    EXPECT_EQ(b, -999999999);
    EXPECT_EQ(c, 3);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, run_def_add) {
    char* line = (char*)
    "def add(a, b):\n"
    "    return a + b\n"
    "c = add(1,3)\n"
    "\n";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_multiLineToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int c = args_getInt(globals->list, (char*)"c");
    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    ASSERT_FLOAT_EQ(c, 4);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, equ) {
    char* line = (char*)
    "if -1 == -1:\n"
    "    a = 1\n"
    "\n"
    ;
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_multiLineToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int c = args_getInt(globals->list, (char*)"a");
    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    ASSERT_FLOAT_EQ(c, 1);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, if_elif) {
    char* line = (char*)
    "a = 2\n"
    "b = 0\n"
    "if a > 1:\n"
    "    b = 1\n"
    "elif a > 0:\n"
    "    b = 2\n"
    "\n"
    ;
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_multiLineToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int b = args_getInt(globals->list, (char*)"b");
    obj_deinit(self);
    args_deinit(buffs);
    ASSERT_FLOAT_EQ(b, 1);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, if_else) {
    char* line = (char*)
    "a = 0\n"
    "b = 0\n"
    "if a > 1:\n"
    "    b = 1\n"
    "else:\n"
    "    b = 2\n"
    "\n"
    ;
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_multiLineToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int b = args_getInt(globals->list, (char*)"b");
    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    ASSERT_FLOAT_EQ(b, 2);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, if_else_) {
    char* line = (char*)
    "a = 2\n"
    "b = 0\n"
    "if a > 1:\n"
    "    b = 1\n"
    "else:\n"
    "    b = 2\n"
    "\n"
    ;
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_multiLineToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    VMParameters* globals = pikaVM_runAsm(self, pikaAsm);

    int b = args_getInt(globals->list, (char*)"b");
    obj_deinit(self);
    args_deinit(buffs);
    // obj_deinit(globals);
    ASSERT_FLOAT_EQ(b, 1);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, EST) {
    char* pikaAsm = (char*)
    "B0\n"
    "0 EST a\n"
    "0 OUT b\n"
    ;
    PikaObj* self = newRootObj((char*)"", New_BaseObj);
    pikaVM_runAsm(self, pikaAsm);
    int a = obj_getInt(self, (char*)"a");
    int b = obj_getInt(self, (char*)"b");
    /* a is local, should not be exist in globals */
    EXPECT_EQ(a, -999999999);
    /* b is local, should not be exist in globals */
    EXPECT_EQ(b, 0);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_AS) {
    char* pikaAsm = (char*)
        "B0\n"
        "0 RUN PikaStdLib.PikaObj\n"
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
        "0 OUT x\n"
    ;
    PikaObj* self = newRootObj((char*)"", New_PikaMain);
    pikaVM_runAsm(self, pikaAsm);
    PikaObj* as = obj_getObj(self, (char*)"as", 0);
    int x_as_ = obj_getInt(as, (char*)"x");
    int x_as = obj_getInt(self, (char*)"as.x");
    int x_origin = obj_getInt(self, (char*)"x");
    EXPECT_EQ(x_as_, 1);
    EXPECT_EQ(x_as, 1);
    EXPECT_EQ(x_origin, 2);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_NEW) {
    char* pikaAsm = (char*)
        "B0\n"
        "0 RUN PikaStdLib.PikaObj\n"
        "0 OUT newObj\n"
        "0 NUM 1\n"
        "0 OUT newObj.x\n"
        "B0\n"
        "0 NEW newObj\n"
        "0 OUT outObj\n"
    ;
    PikaObj* self = newRootObj((char*)"", New_PikaMain);
    pikaVM_runAsm(self, pikaAsm);
    Arg* newObj = obj_getArg(self, (char*)"newObj");
    Arg* outObj = obj_getArg(self, (char*)"outObj");
    void* newObj_ptr = arg_getPtr(newObj);
    void* outObj_ptr = arg_getPtr(outObj);
    EXPECT_EQ(newObj_ptr, outObj_ptr);
    int x = obj_getInt(self, (char*)"outObj.x");
    ArgType newObj_type = arg_getType(newObj);
    ArgType outObj_type = arg_getType(outObj);
    EXPECT_EQ(x, 1);
    EXPECT_EQ(newObj_type, ARG_TYPE_POINTER);
    EXPECT_EQ(outObj_type, ARG_TYPE_OBJECT);
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, RUN_DEF_NEW) {
    char* pikaAsm = (char*)
        "B0\n"
        "0 DEF testnew()\n"
        "0 JMP 1\n"
        "B1\n"
        "0 RUN PikaStdLib.PikaObj\n"
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
        "0 OUT outobj\n"
    ;
    PikaObj* self = newRootObj((char*)"", New_PikaMain);
    pikaVM_runAsm(self, pikaAsm);
    /* assert */
    PikaObj* outobj = obj_getObj(self, (char*)"outobj", 0);
    int x = obj_getInt(outobj, (char*)"x");
    EXPECT_EQ(x, 1);
    /* deinit */
    obj_deinit(self);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, class_x_1) {
    char* line = (char*)
        "class Test(PikaStdLib.PikaObj):\n"
        "    x = 1\n"
        "test = Test()\n"
        "print('test.x: ' + str(test.x))\n"
        "\n"
        ;
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_multiLineToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"", New_PikaMain);
    pikaVM_runAsm(self, pikaAsm);

    PikaObj* test = obj_getObj(self, (char*)"test", 0);
    Arg* test_arg = obj_getArg(self, (char*)"test");
    ArgType test_arg_type = arg_getType(test_arg);
    EXPECT_EQ(test_arg_type, ARG_TYPE_OBJECT);
    int x = obj_getInt(test, (char*)"x");
    EXPECT_EQ(x, 1);
    obj_deinit(self);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, nag_a) {
    char* line = (char*)
            "a = 1\n"
            "print(-a)\n"
            "b = 0.5\n"
            "print(-b)\n"
        ;
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_multiLineToAsm(buffs, line);
    printf("%s", pikaAsm);
    PikaObj* self = newRootObj((char*)"", New_PikaMain);
    __platform_printf((char*)"BEGIN\r\n");
    pikaVM_runAsm(self, pikaAsm);
    /* assert */
    EXPECT_STREQ(log_buff[2], (char*)"BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], (char*)"-1\r\n");
    EXPECT_STREQ(log_buff[0], (char*)"-0.500000\r\n");
    /* deinit */
    obj_deinit(self);
    args_deinit(buffs);
    /* check mem */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(InstructUnit, base) {
    __platform_printf((char*)"BEGIN\r\n");
    InstructUnit ins_unit;
    instructUnit_init(&ins_unit);
    instructUnit_setBlockDeepth(&ins_unit, 2);
    instructUnit_setIsNewLine(&ins_unit, 1);
    instructUnit_setInvokeDeepth(&ins_unit, 3);
    instructUnit_setInstruct(&ins_unit, (Instruct)4);
    instructUnit_setConstPoolIndex(&ins_unit, 12);

    EXPECT_EQ(instructUnit_getBlockDeepth(&ins_unit), 2);
    EXPECT_EQ(instructUnit_getIsNewLine(&ins_unit), 1);
    EXPECT_EQ(instructUnit_getInvokeDeepth(&ins_unit), 3);
    EXPECT_EQ(instructUnit_getInstruct(&ins_unit), 4);
    EXPECT_EQ(instructUnit_getConstPoolIndex(&ins_unit), 12);

    instructUnit_print(&ins_unit);
    EXPECT_STREQ(log_buff[2], (char*)"BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], (char*)"B2\r\n");
    EXPECT_STREQ(log_buff[0], (char*)"3 OUT #12\r\n");
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(ConstPool, get) {
    __platform_printf((char*)"BEGIN\r\n");
    ConstPool cp;
    constPool_init(&cp);
    char* next_content;
    uint16_t next_offset;
    next_offset = constPool_getLastOffset(&cp);
    constPool_append(&cp, (char*)"hello");
    next_content = constPool_getByOffset(&cp, next_offset);
    EXPECT_STREQ(next_content, (char*)"hello");
    next_offset = constPool_getLastOffset(&cp);
    constPool_append(&cp, (char*)"world");
    next_content = constPool_getByOffset(&cp, next_offset);
    EXPECT_STREQ(next_content, (char*)"world");
    char* first = constPool_getNow(&cp);
    char* second = constPool_getNext(&cp);
    char* third = constPool_getNext(&cp);
    char* forth = constPool_getNext(&cp);
    EXPECT_STREQ(first, (char*)"");
    EXPECT_STREQ(second, (char*)"hello");
    EXPECT_STREQ(third, (char*)"world");
    EXPECT_EQ((uintptr_t)forth, (uintptr_t)NULL);
    EXPECT_STREQ(constPool_getByIndex(&cp, 0), (char*)"");
    EXPECT_STREQ(constPool_getByIndex(&cp, 1), (char*)"hello");
    EXPECT_STREQ(constPool_getByIndex(&cp, 2), (char*)"world");
    EXPECT_EQ((uintptr_t)constPool_getByIndex(&cp, 3), (uintptr_t)NULL);
    constPool_print(&cp);
    EXPECT_STREQ(log_buff[2], (char*)"BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], (char*)"1: hello\r\n");
    EXPECT_STREQ(log_buff[0], (char*)"7: world\r\n");
    constPool_deinit(&cp);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(InstructArray, set) {
    __platform_printf((char*)"BEGIN\r\n");
    InstructUnit ins_unit;
    instructUnit_init(&ins_unit);
    instructUnit_setBlockDeepth(&ins_unit, 2);
    instructUnit_setIsNewLine(&ins_unit, 1);
    instructUnit_setInvokeDeepth(&ins_unit, 3);
    instructUnit_setInstruct(&ins_unit, (Instruct)4);
    instructUnit_setConstPoolIndex(&ins_unit, 12);

    InstructArray ins_array;
    instructArray_init(&ins_array);
    instructArray_append(&ins_array, &ins_unit);
    instructArray_print(&ins_array);
    instructArray_deinit(&ins_array);
    EXPECT_STREQ(log_buff[2], (char*)"BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], (char*)"B2\r\n");
    EXPECT_STREQ(log_buff[0], (char*)"3 OUT #12\r\n");
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, bytecode_jjcc) {
    char* line = (char*)"a = (1 + 1.1) * 3 - 2 /4.0";
    Args* buffs = New_strBuff();
    char* pikaAsm = Parser_LineToAsm(buffs, line, NULL);
    printf("%s", pikaAsm);
    ByteCodeFrame byte_frame;
    byteCodeFrame_init(&byte_frame);
    byteCodeFrame_appendFromAsm(&byte_frame, pikaAsm);
    byteCodeFrame_print(&byte_frame);

    PikaObj* self = newRootObj((char*)"root", New_PikaStdLib_SysObj);
    // pikaVM_runAsm(self, pikaAsm);
    pikaVM_runByteCodeFrame(self, &byte_frame);

    float res = obj_getFloat(self, (char*)"a");

    obj_deinit(self);
    args_deinit(buffs);
    byteCodeFrame_deinit(&byte_frame);

    ASSERT_FLOAT_EQ(res, 5.8);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, WHILE_byte) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =(char *)
        "a = 1\n"
        "b = 0\n"
        "while a:\n"
        "    b = 1\n"
        "    a = 0\n"
        "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* self = New_TinyObj(NULL);
    pikaVM_run(self, lines);
    EXPECT_EQ(obj_getInt(self, (char*)"a"), 0);
    EXPECT_EQ(obj_getInt(self, (char*)"b"), 1);
    args_deinit(buffs);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, for_break_byte) {
    pikaMemInfo.heapUsedMax = 0;
    Args* buffs = New_strBuff();
    char* lines =(char *)
         "a = 0\n"
         "for i in range(0, 10):\n"
         "    if i == 5:\n"
         "        break\n"
         "    a = a + i\n"
         "\n";
    printf("%s", lines);
    char* pikaAsm = Parser_multiLineToAsm(buffs, (char*)lines);
    printf("%s", pikaAsm);
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* self = newRootObj((char*)"pikaMain", New_PikaMain);
    pikaVM_run(self, lines);
    /* assert */
    int a = obj_getInt(self, (char*)"a");
    EXPECT_EQ(a, 10);
    /* deinit */
    args_deinit(buffs);
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(VM, load_static_bytes) {
    const uint8_t bytes[] = {
        0x08, 0x02, /* instruct array size */
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
        0xb1, 0x00,             /* const pool size */
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

    byteCodeFrame_loadBytes(&bytecode_frame, (uint8_t*)bytes);
    byteCodeFrame_print(&bytecode_frame);

    EXPECT_EQ(instructArray_getSize(&(bytecode_frame.instruct_array)), 520);
    EXPECT_EQ(bytecode_frame.const_pool.size, 177);
    EXPECT_EQ(pikaMemNow(), 0);
}
