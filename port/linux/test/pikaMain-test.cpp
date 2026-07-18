#include "test_common.h"
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
TEST_START

extern "C" uint32_t stack_spSizeFree(Stack* stack);
extern "C" void _stack_overflow_handler(Stack* stack, size_t stack_require);

TEST(stack, size_array_capacity) {
    Stack stack = {0};
    _stack_init(&stack, 64);
    EXPECT_EQ(stack_spSizeFree(&stack), 16);
    stack_pushSize(&stack, 1);
    EXPECT_EQ(stack_spSizeFree(&stack), 12);
    stack_reset(&stack);
    stack_deinit(&stack);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(stack, fatal_overflow_reports_before_stopping) {
    int output_pipe[2] = {-1, -1};
    ASSERT_EQ(pipe(output_pipe), 0);
    fflush(stdout);

    pid_t child = fork();
    ASSERT_NE(child, -1);
    if (child == 0) {
        close(output_pipe[0]);
        if (dup2(output_pipe[1], STDOUT_FILENO) == -1) {
            _exit(100);
        }
        close(output_pipe[1]);

        static char output_buffer[1024];
        setvbuf(stdout, output_buffer, _IOFBF, sizeof(output_buffer));
        Stack stack = {0};
        stack.stack_totle_size = 64;
        _stack_overflow_handler(&stack, 80);
        _exit(101);
    }

    close(output_pipe[1]);
    struct pollfd output = {output_pipe[0], POLLIN, 0};
    char report[1024] = {0};
    size_t report_size = 0;
    for (int retry = 0; retry < 100; retry++) {
        int poll_result = poll(&output, 1, 10);
        if (poll_result == 1) {
            ssize_t size =
                read(output_pipe[0], report + report_size,
                     sizeof(report) - report_size - 1);
            if (size > 0) {
                report_size += (size_t)size;
            }
        }
        if (strstr(report, "execution stopped") != nullptr) {
            break;
        }
    }
    EXPECT_GT(report_size, 0);
    EXPECT_NE(strstr(report, "FatalError: VM stack exhausted"), nullptr);
    EXPECT_NE(strstr(report, "80/64 bytes, PIKA_STACK_BUFF_SIZE"), nullptr);
    EXPECT_NE(strstr(report, "execution stopped"), nullptr);

    int status = 0;
    pid_t wait_result = waitpid(child, &status, WNOHANG);
    EXPECT_EQ(wait_result, 0);
    if (wait_result == 0) {
        EXPECT_EQ(kill(child, SIGKILL), 0);
        EXPECT_EQ(waitpid(child, &status, 0), child);
        EXPECT_TRUE(WIFSIGNALED(status));
        EXPECT_EQ(WTERMSIG(status), SIGKILL);
    }
    close(output_pipe[0]);
}

TEST_RUN_LINES_EXCEPT_OUTPUT(
    issue,
    issue356_top_level_function_refs,
    "def blinkTest():\n"
    "    return 'blink'\n"
    "def statTest():\n"
    "    return 'stat'\n"
    "def initTimer():\n"
    "    return 'timer'\n"
    "blinkFunc = blinkTest\n"
    "statFunc = statTest\n"
    "initTimerFunc = initTimer\n"
    "print(blinkFunc(), statFunc(), initTimerFunc())\n",
    "blink stat timer\r\n")

TEST(pikaMain, init) {
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = pikaScriptInit();
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, new_mem) {
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    VMParameters* globals = obj_run(pikaMain,
                                    "mem = PikaStdLib.MemChecker()\n"
                                    "mem.max()\n");
    PikaObj* mem = (PikaObj*)args_getPtr(globals->list, "mem");
    PikaObj* memClassPtr = (PikaObj*)mem->constructor;
    EXPECT_EQ((void*)memClassPtr, (void*)New_PikaStdLib_MemChecker);
    obj_deinit(pikaMain);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, unitTest) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/UnitTest.py");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);

    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, list_new) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    VMParameters* globals = obj_run(pikaMain,
                                    "list = PikaStdData.List()\n"
                                    "list.append(7)\n"
                                    "list.append('eee')\n"
                                    "len = list.len()\n"
                                    "a1 = list.get(0)\n"
                                    "a2 = list.get(1)\n");
    /* collect */
    int len = obj_getInt(globals, "len");
    int a1 = obj_getInt(globals, "a1");
    char* a2 = obj_getStr(globals, "a2");

    /* assert */
    EXPECT_EQ(len, 2);
    EXPECT_EQ(a1, 7);
    EXPECT_STREQ(a2, "eee");

    /* deinit */
    // obj_deinit(globals);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, int_float_convert) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    VMParameters* globals = obj_run(pikaMain,
                                    "a = 1\n"
                                    "a_f = float(a)\n"
                                    "b = 1.3\n"
                                    "b_i = int(b)\n");
    /* collect */
    int a = obj_getInt(globals, "a");
    float a_f = obj_getFloat(globals, "a_f");
    float b = obj_getFloat(globals, "b");
    int b_i = obj_getInt(globals, "b_i");

    /* assert */
    EXPECT_EQ(a, 1);
    EXPECT_FLOAT_EQ(a_f, 1);
    EXPECT_FLOAT_EQ(b, 1.3);
    EXPECT_FLOAT_EQ(b_i, 1);

    /* deinit */
    // obj_deinit(globals);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, type_) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "type(1)\n"
            "b = 1.4\n"
            "type(b)\n");
    /* collect */

    /* assert */

    /* deinit */
    // obj_deinit(globals);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, a_signed) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "a = -1\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");

    /* assert */
    EXPECT_EQ(-1, a);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, a_mm) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "a = -1.1 ** 3\n");
    /* collect */
    float a = obj_getFloat(pikaMain, "a");

    /* assert */
    EXPECT_FLOAT_EQ(-1.331, a);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, a_n1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "a = -2\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");

    /* assert */
    EXPECT_EQ(-2, a);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, a_l4) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "a = 1<<4\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");

    /* assert */
    EXPECT_EQ(16, a);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, less_equ) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 0\n"
            "while a <= 10:\n"
            "    a += 1\n"
            "\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");

    /* assert */
    EXPECT_EQ(11, a);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(dataString, point_to_last_token) {
    char nested[] = "controller.state.output";
    char plain[] = "output";
    char leading[] = ".output";
    char trailing[] = "controller.";
    char empty[] = "";

    EXPECT_STREQ(strPointToLastToken(nested, '.'), "output");
    EXPECT_EQ(strPointToLastToken(plain, '.'), plain);
    EXPECT_STREQ(strPointToLastToken(leading, '.'), "output");
    EXPECT_STREQ(strPointToLastToken(trailing, '.'), "");
    EXPECT_EQ(strPointToLastToken(empty, '.'), empty);
}

TEST(pikaMain, recursive_global_function_lookup) {
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    pikaVM_run(pikaMain,
               (char*)"def fib(n):\n"
                      "    if n < 2:\n"
                      "        return n\n"
                      "    return fib(n - 1) + fib(n - 2)\n"
                      "result = fib(10)\n");
    EXPECT_EQ(obj_getInt(pikaMain, (char*)"result"), 55);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, recursive_call_error_propagation) {
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    pikaVM_run(pikaMain,
               (char*)"def recurse(n):\n"
                      "    if n == 0:\n"
                      "        return missing_name\n"
                      "    return recurse(n - 1)\n"
                      "result = recurse(3)\n");

    pika_bool found = pika_false;
    for (int i = 0; i < LOG_BUFF_MAX; i++) {
        if (strEqu(log_buff[i],
                   "NameError: name 'missing_name' is not defined\n")) {
            found = pika_true;
            break;
        }
    }
    EXPECT_TRUE(found);
    EXPECT_FALSE(obj_isArgExist(pikaMain, (char*)"result"));

    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, and_or_not) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 0\n"
            "b = 0\n"
            "c = 0\n"
            "d = 0\n"
            "if (1>2) or (2>1):\n"
            "    a = 1\n"
            "\n"
            "if (1>2) and (2>1):\n"
            "    b = 1\n"
            "\n"
            "if not (1>2):\n"
            "    c = 1\n"
            "\n"
            "if not (2>1):\n"
            "    d = 1\n"
            "\n"
            "e = ~1\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    int b = obj_getInt(pikaMain, "b");
    int c = obj_getInt(pikaMain, "c");
    int d = obj_getInt(pikaMain, "d");
    int e = obj_getInt(pikaMain, "e");

    /* assert */
    EXPECT_EQ(1, a);
    EXPECT_EQ(0, b);
    EXPECT_EQ(1, c);
    EXPECT_EQ(0, d);
    EXPECT_EQ(e, -2);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, python3_operator_associativity_and_short_circuit) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    VMParameters* globals = obj_run(
        pikaMain,
        "calls = 0\n"
        "def side_effect():\n"
        "    global calls\n"
        "    calls += 1\n"
        "    return 9\n"
        "pow_right = 2 ** 3 ** 2\n"
        "pow_left = (2 ** 3) ** 2\n"
        "and_skip = False and side_effect()\n"
        "or_skip = True or side_effect()\n"
        "and_value = True and side_effect()\n"
        "or_value = False or side_effect()\n"
        "false_operand = 0 and 5\n"
        "true_operand = 3 or 5\n"
        "string_or = '' or 'fallback'\n"
        "string_and = 'left' and 'right'\n"
        "chain_and = 1 and 2 and 3\n"
        "chain_or = 0 or 0 or 4\n"
        "nested_and = False and (True and side_effect())\n"
        "nested_or = True or (False or side_effect())\n"
        "nested_eval = True and (False or side_effect())\n");

    EXPECT_EQ(obj_getInt(globals, "pow_right"), 512);
    EXPECT_EQ(obj_getInt(globals, "pow_left"), 64);
    EXPECT_FALSE(obj_getBool(globals, "and_skip"));
    EXPECT_TRUE(obj_getBool(globals, "or_skip"));
    EXPECT_EQ(obj_getInt(globals, "and_value"), 9);
    EXPECT_EQ(obj_getInt(globals, "or_value"), 9);
    EXPECT_EQ(obj_getInt(globals, "false_operand"), 0);
    EXPECT_EQ(obj_getInt(globals, "true_operand"), 3);
    EXPECT_STREQ(obj_getStr(globals, "string_or"), "fallback");
    EXPECT_STREQ(obj_getStr(globals, "string_and"), "right");
    EXPECT_EQ(obj_getInt(globals, "chain_and"), 3);
    EXPECT_EQ(obj_getInt(globals, "chain_or"), 4);
    EXPECT_FALSE(obj_getBool(globals, "nested_and"));
    EXPECT_TRUE(obj_getBool(globals, "nested_or"));
    EXPECT_EQ(obj_getInt(globals, "nested_eval"), 9);
    EXPECT_EQ(obj_getInt(globals, "calls"), 3);

    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, python3_floor_division_and_modulo) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    VMParameters* globals = obj_run(
        pikaMain,
        "floor_left = -3 // 2\n"
        "mod_left = -3 % 2\n"
        "floor_right = 3 // -2\n"
        "mod_right = 3 % -2\n");

    EXPECT_EQ(obj_getInt(globals, "floor_left"), -2);
    EXPECT_EQ(obj_getInt(globals, "mod_left"), 1);
    EXPECT_EQ(obj_getInt(globals, "floor_right"), -2);
    EXPECT_EQ(obj_getInt(globals, "mod_right"), -1);

    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(pikaMain, floor_division_and_modulo_zero_errors) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    VMParameters* globals = obj_run(
        pikaMain,
        "floor_zero = False\n"
        "mod_zero = False\n"
        "try:\n"
        "    ignored = 1 // 0\n"
        "except ZeroDivisionError:\n"
        "    floor_zero = True\n"
        "try:\n"
        "    ignored = 1 % 0\n"
        "except ZeroDivisionError:\n"
        "    mod_zero = True\n");

    EXPECT_TRUE(obj_getBool(globals, "floor_zero"));
    EXPECT_TRUE(obj_getBool(globals, "mod_zero"));

    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, python3_typed_except_runtime_matrix) {
    struct ExceptCase {
        const char* source;
        int expected;
    };
    const ExceptCase cases[] = {
        {"result=1\ntry:\n    pass\nexcept:\n    result=2\n", 1},
        {"result=1\ntry:\n    1/0\nexcept:\n    result=2\n", 2},
        {"result=0\ntry:\n    1/0\nexcept ZeroDivisionError:\n    result=2\n",
         2},
        {"result=0\ntry:\n    1/0\nexcept ValueError:\n    result=1\nexcept "
         "ZeroDivisionError:\n    result=2\n",
         2},
        {"result=0\ntry:\n    raise ValueError\nexcept ValueError:\n    "
         "result=1\n",
         1},
        {"result=0\ntry:\n    raise ValueError()\nexcept ValueError:\n    "
         "result=1\n",
         1},
        {"result=0\ntry:\n    raise ValueError\nexcept ValueError:\n    "
         "result=result+1\nexcept ZeroDivisionError:\n    result=result+2\n",
         1},
        {"result=0\ntry:\n    raise ValueError\nexcept ValueError:\n    "
         "result=1\nexcept:\n    result=2\n",
         1},
        {"result=0\ntry:\n    1/0\nexcept ValueError:\n    result=1\nexcept:\n"
         "    result=2\n",
         2},
        {"result=1\ntry:\n    pass\nexcept TypeError:\n    result=2\nexcept "
         "ValueError:\n    result=3\n",
         1},
        {"result=0\ntry:\n    raise ValueError\nexcept ValueError as err:\n"
         "    result=isinstance(err, ValueError)\n",
         1},
        {"result=0\ntry:\n    1/0\nexcept ArithmeticError as err:\n"
         "    result=isinstance(err, ZeroDivisionError)\n",
         1},
        {"result=0\ntry:\n    try:\n        1/0\n    except ValueError:\n"
         "        result=1\nexcept ZeroDivisionError:\n    result=2\n",
         2},
        {"result=0\ntry:\n    try:\n        raise ValueError\n"
         "    except ValueError:\n        raise TypeError\n"
         "except TypeError:\n    result=1\n",
         1},
        {"result=0\ntry:\n    1/0\nexcept Exception:\n    result=1\n", 1},
        {"result=0\ntry:\n    1/0\nexcept ArithmeticError:\n    result=1\n",
         1},
        {"result=0\ntry:\n    [1][3]\nexcept LookupError:\n    result=1\n",
         1},
        {"result=0\ntry:\n    1+'a'\nexcept TypeError:\n    result=1\n", 1},
        {"result=0\ntry:\n    int('x')\nexcept ValueError:\n    result=1\n",
         1},
        {"result=0\ntry:\n    raise 3\nexcept TypeError:\n    result=1\n", 1},
        {"result=0\ntry:\n    raise ValueError\nexcept ValueError:\n    "
         "result=result+1\ntry:\n    1/0\nexcept ValueError:\n    "
         "result=9\nexcept ZeroDivisionError:\n    result=result+2\n",
         3},
        {"result=0\ntry:\n    1/0\nexcept ZeroDivisionError:\n    result=1\n"
         "result=result+2\n",
         3},
        {"def f():\n    try:\n        return 3\n    except:\n        return 4\n"
         "result=f()\n",
         3},
        {"result=0\nfor i in [1,2]:\n    try:\n        break\n    except:\n"
         "        result=9\n    result=8\nresult=result+1\n",
         1},
        {"result=0\nfor i in [1,2]:\n    try:\n        continue\n    except:\n"
         "        result=9\n    result=8\nresult=result+1\n",
         1},
    };

    for (const ExceptCase& test_case : cases) {
        SCOPED_TRACE(test_case.source);
        PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
        VMParameters* globals = obj_run(pikaMain, (char*)test_case.source);
        EXPECT_EQ(obj_getInt(globals, "result"), test_case.expected);
        obj_deinit(pikaMain);
        EXPECT_EQ(pikaMemNow(), 0);
    }
}

TEST(pikaMain, legacy_untyped_exp_bytecode) {
    char legacy_asm[] =
        "B0\n"
        "0 NUM 0\n"
        "0 OUT result\n"
        "B0\n"
        "0 TRY \n"
        "B1\n"
        "1 NUM 1\n"
        "1 NUM 0\n"
        "0 OPT /\n"
        "B0\n"
        "0 NTR \n"
        "0 GER \n"
        "0 JEZ 2\n"
        "B0\n"
        "0 EXP \n"
        "B1\n"
        "0 NUM 7\n"
        "0 OUT result\n"
        "B0\n";
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    VMParameters* globals = pikaVM_runAsm(pikaMain, legacy_asm);
    EXPECT_EQ(obj_getInt(globals, "result"), 7);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, python3_duplicate_position_keyword) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    VMParameters* globals = obj_run(
        pikaMain,
        "def combine(a, b=5):\n"
        "    return a * 10 + b\n"
        "def combine_required(a, b):\n"
        "    return a * 10 + b\n"
        "def combine_three(a, b, c):\n"
        "    return a * 100 + b * 10 + c\n"
        "duplicate_function = False\n"
        "duplicate_required = False\n"
        "duplicate_multi = False\n"
        "duplicate_method = False\n"
        "try:\n"
        "    combine(1, a=2)\n"
        "except TypeError:\n"
        "    duplicate_function = True\n"
        "valid_function = combine(1, b=2)\n"
        "try:\n"
        "    combine_required(1, a=2)\n"
        "except TypeError:\n"
        "    duplicate_required = True\n"
        "valid_required = combine_required(1, b=4)\n"
        "try:\n"
        "    combine_three(1, 2, a=3)\n"
        "except TypeError:\n"
        "    duplicate_multi = True\n"
        "valid_after_multi = combine_three(1, 2, 3)\n"
        "class Combiner:\n"
        "    def combine(self, a, b=5):\n"
        "        return a * 10 + b\n"
        "combiner = Combiner()\n"
        "try:\n"
        "    combiner.combine(1, a=2)\n"
        "except TypeError:\n"
        "    duplicate_method = True\n"
        "valid_method = combiner.combine(1, b=3)\n");

    EXPECT_TRUE(obj_getBool(globals, "duplicate_function"));
    EXPECT_TRUE(obj_getBool(globals, "duplicate_required"));
    EXPECT_TRUE(obj_getBool(globals, "duplicate_multi"));
    EXPECT_TRUE(obj_getBool(globals, "duplicate_method"));
    EXPECT_EQ(obj_getInt(globals, "valid_function"), 12);
    EXPECT_EQ(obj_getInt(globals, "valid_required"), 14);
    EXPECT_EQ(obj_getInt(globals, "valid_after_multi"), 123);
    EXPECT_EQ(obj_getInt(globals, "valid_method"), 13);

    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(pikaMain, python3_default_argument_definition_time) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    VMParameters* globals = obj_run(
        pikaMain,
        "seed = 1\n"
        "default_calls = 0\n"
        "def make_default():\n"
        "    global default_calls\n"
        "    default_calls += 1\n"
        "    return []\n"
        "def use_default(value=seed, items=make_default()):\n"
        "    items.append(1)\n"
        "    return value * 100 + len(items)\n"
        "seed = 2\n"
        "default_first = use_default()\n"
        "default_second = use_default()\n"
        "default_explicit = use_default(3, [])\n"
        "global_value = 7\n"
        "def use_global():\n"
        "    global global_value\n"
        "    before = global_value\n"
        "    global_value = 8\n"
        "    return before\n"
        "global_before = use_global()\n"
        "class DefaultOwner:\n"
        "    def value(self, item=4):\n"
        "        return item\n"
        "    def enabled(self, stop_on_error: bool=False):\n"
        "        return stop_on_error\n"
        "class_default = DefaultOwner().value()\n"
        "method_false_default = DefaultOwner().enabled()\n"
        "def false_default(stop_on_error=False):\n"
        "    return stop_on_error\n"
        "false_default_value = false_default()\n"
        "false_default_override = false_default(True)\n");

    EXPECT_EQ(obj_getInt(globals, "default_first"), 101);
    EXPECT_EQ(obj_getInt(globals, "default_second"), 102);
    EXPECT_EQ(obj_getInt(globals, "default_explicit"), 301);
    EXPECT_EQ(obj_getInt(globals, "default_calls"), 1);
    EXPECT_EQ(obj_getInt(globals, "global_before"), 7);
    EXPECT_EQ(obj_getInt(globals, "global_value"), 8);
    EXPECT_EQ(obj_getInt(globals, "class_default"), 4);
    EXPECT_EQ(obj_getInt(globals, "method_false_default"), 0);
    EXPECT_EQ(obj_getInt(globals, "false_default_value"), 0);
    EXPECT_EQ(obj_getInt(globals, "false_default_override"), 1);

    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, python3_unbound_local_error) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain,
            "value = 7\n"
            "def read_before_assignment():\n"
            "    before = value\n"
            "    value = 8\n"
            "    return before\n"
            "read_before_assignment()\n");

    pika_bool found = pika_false;
    for (int i = 0; i < LOG_BUFF_MAX; i++) {
        if (strEqu(log_buff[i],
                   "UnboundLocalError: local variable 'value' referenced "
                   "before assignment\n")) {
            found = pika_true;
            break;
        }
    }
    EXPECT_TRUE(found);

    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, python3_definition_scope_neighbors) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    VMParameters* globals = obj_run(
        pikaMain,
        "def read_parameter(value):\n"
        "    return value\n"
        "def assigned_before_read():\n"
        "    value = 6\n"
        "    return value\n"
        "outer_value = 9\n"
        "def outer():\n"
        "    def inner():\n"
        "        outer_value = 10\n"
        "        return outer_value\n"
        "    return outer_value\n"
        "parameter_value = read_parameter(5)\n"
        "assigned_value = assigned_before_read()\n"
        "nested_value = outer()\n");

    EXPECT_EQ(obj_getInt(globals, "parameter_value"), 5);
    EXPECT_EQ(obj_getInt(globals, "assigned_value"), 6);
    EXPECT_EQ(obj_getInt(globals, "nested_value"), 9);

    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(pikaMain, python3_definition_metadata_across_runs) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain,
            "def first(value=11):\n"
            "    return value\n");
    obj_run(pikaMain,
            "def second(value=22):\n"
            "    return value\n");
    VMParameters* globals = obj_run(pikaMain,
                                    "first_value = first()\n"
                                    "second_value = second()\n");

    EXPECT_EQ(obj_getInt(globals, "first_value"), 11);
    EXPECT_EQ(obj_getInt(globals, "second_value"), 22);

    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(pikaMain, python3_comprehension_scope) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    VMParameters* globals = obj_run(
        pikaMain,
        "i = 41\n"
        "values = [i * 2 for i in [1, 2, 3]]\n"
        "after_i = i\n"
        "none_target = None\n"
        "none_values = [none_target for none_target in [3, 4]]\n"
        "missing_values = [j + 1 for j in [1, 2]]\n"
        "def build(offset):\n"
        "    j = 50\n"
        "    items = [offset + j for j in [1, 2]]\n"
        "    return items[0] * 100 + items[1] * 10 + j\n"
        "function_value = build(5)\n");

    EXPECT_EQ(obj_getInt(globals, "after_i"), 41);
    EXPECT_TRUE(obj_isArgExist(globals, "none_target"));
    EXPECT_EQ(arg_getType(obj_getArg(globals, "none_target")), ARG_TYPE_NONE);
    EXPECT_FALSE(obj_isArgExist(globals, "j"));
    EXPECT_EQ(obj_getInt(globals, "function_value"), 720);
    PikaObj* values = obj_getObj(globals, "values");
    EXPECT_EQ(pikaList_getInt(values, 0), 2);
    EXPECT_EQ(pikaList_getInt(values, 1), 4);
    EXPECT_EQ(pikaList_getInt(values, 2), 6);

    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, python3_starred_unpack_assignment) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    VMParameters* globals = obj_run(
        pikaMain,
        "first, *middle, last = [1, 2, 3, 4]\n"
        "tuple_first, *tuple_middle = (5, 6, 7)\n");

    EXPECT_EQ(obj_getInt(globals, "first"), 1);
    EXPECT_EQ(obj_getInt(globals, "last"), 4);
    EXPECT_EQ(obj_getInt(globals, "tuple_first"), 5);
    PikaObj* middle = obj_getObj(globals, "middle");
    PikaObj* tuple_middle = obj_getObj(globals, "tuple_middle");
    EXPECT_EQ(pikaList_getInt(middle, 0), 2);
    EXPECT_EQ(pikaList_getInt(middle, 1), 3);
    EXPECT_EQ(pikaList_getInt(tuple_middle, 0), 6);
    EXPECT_EQ(pikaList_getInt(tuple_middle, 1), 7);

    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, python3_unpack_value_errors) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    VMParameters* globals = obj_run(
        pikaMain,
        "too_many = False\n"
        "too_few = False\n"
        "star_too_few = False\n"
        "a = 10\n"
        "b = 20\n"
        "calls = 0\n"
        "def unpack_source():\n"
        "    global calls\n"
        "    calls += 1\n"
        "    return [8, 9]\n"
        "once_a, once_b = unpack_source()\n"
        "try:\n"
        "    a, b = [1, 2, 3]\n"
        "except ValueError:\n"
        "    too_many = True\n"
        "try:\n"
        "    a, b = [1]\n"
        "except ValueError:\n"
        "    too_few = True\n"
        "try:\n"
        "    a, *rest, b = [1]\n"
        "except ValueError:\n"
        "    star_too_few = True\n");

    EXPECT_TRUE(obj_getBool(globals, "too_many"));
    EXPECT_TRUE(obj_getBool(globals, "too_few"));
    EXPECT_TRUE(obj_getBool(globals, "star_too_few"));
    EXPECT_EQ(obj_getInt(globals, "a"), 10);
    EXPECT_EQ(obj_getInt(globals, "b"), 20);
    EXPECT_EQ(obj_getInt(globals, "calls"), 1);
    EXPECT_EQ(obj_getInt(globals, "once_a"), 8);
    EXPECT_EQ(obj_getInt(globals, "once_b"), 9);

    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, python3_iter_error_recovery) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    VMParameters* globals = obj_run(
        pikaMain,
        "iter_error = False\n"
        "try:\n"
        "    for item in 1:\n"
        "        pass\n"
        "except TypeError:\n"
        "    iter_error = True\n"
        "after_errors = 7\n");

    EXPECT_TRUE(obj_getBool(globals, "iter_error"));
    EXPECT_EQ(obj_getInt(globals, "after_errors"), 7);

    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, err_scop) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "print('test'");
    /* collect */

    /* assert */

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, PikaStdData) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "dict = PikaStdData.Dict()\n"
            "dict.set('a', 1)\n"
            "dict.set('b', 2)\n"
            "dict.remove('b')\n"
            "a = dict.get('a')\n"
            "b = dict.get('b')\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    int b = obj_getInt(pikaMain, "b");
    /* assert */
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, _PIKA_INT_ERR);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, list_method) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "list = PikaStdData.List()\n"
            "list.append(7)\n"
            "list.append('eee')\n"
            "len = list.len()\n"
            "a1 = list.get(0)\n"
            "a2 = list.get(1)\n"
            "\n");
    /* collect */
    /* assert */
    /* deinit */
    // obj_deinit(globals);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, list_iter) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "list = PikaStdData.List()\n"
            "res = iter(list)\n"
            "\n");
    /* collect */
    PikaObj* res = (PikaObj*)obj_getPtr(pikaMain, "res");
    PikaObj* list = (PikaObj*)obj_getPtr(pikaMain, "list");
    int ref_cnt = obj_refcntNow(list);
    EXPECT_EQ(ref_cnt, 2);
    /* assert */
    EXPECT_EQ(res, list);
    /* deinit */
    // obj_deinit(globals);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, list_for_loop) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "list = PikaStdData.List()\n"
            "list.append(7)\n"
            "list.append('eee')\n"
            "len = list.len()\n"
            "for item in list:\n"
            "    print(item)\n"
            "    a = item\n"
            "\n");
    /* collect */
    char* a = obj_getStr(pikaMain, "a");
    /* assert */
    EXPECT_STREQ(a, "eee");
    /* deinit */
    // obj_deinit(globals);
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, range) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "r = range(10, 0)\n"
            "r\n"
            "\n");
    /* collect */
    /* assert */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_in_range) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 0\n"
            "for i in range(0, 10):\n"
            "    print(i)\n"
            "    a = a + i\n"
            "\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 45);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_for_in_range) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 0\n"
            "for i in range(0, 10):\n"
            "    for k in range(0, 3):\n"
            "        print(k)\n"
            "        a = a + k\n"
            "\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 30);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_if_break) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 0\n"
            "for i in range(0, 10):\n"
            "    if i == 5:\n"
            "        break\n"
            "    a = a + i\n"
            "\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 10);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_if_continue) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 0\n"
            "for i in range(0, 10):\n"
            "    if i == 5:\n"
            "        continue\n"
            "    a = a + i\n"
            "\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 40);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

/* the log_buff of printf */
extern char log_buff[LOG_BUFF_MAX][LOG_SIZE];
TEST(pikaMain, print_in_def) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    /* the test is used to fix too many print in def */
    /* clear the log_buff */
    memset(log_buff, 0, LOG_BUFF_MAX * LOG_SIZE);
    obj_run(pikaMain,
            "def test_print():\n"
            "    print('test')\n"
            "test_print()\n");
    /* collect */
    /* assert */
    /* should only print once, so the second log (log_buff[1]) shuold be '\0' */
    EXPECT_STREQ(log_buff[0], "test\r\n");
    EXPECT_STREQ(log_buff[1], "");
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, prime_100) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "num = 0\n"
            "i = 2\n"
            "while i < 100:\n"
            "    j=2\n"
            "    is_prime = 1\n"
            "    while j < i:\n"
            "        if i%j==0 :\n"
            "            is_prime = 0\n"
            "            break\n"
            "        j+=1\n"
            "    if is_prime:\n"
            "        num = num + i\n"
            "    i+=1\n"
            "\n");
    /* collect */
    int num = obj_getInt(pikaMain, "num");
    /* assert */
    EXPECT_EQ(num, 1060);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

/* the log_buff of printf */
extern char log_buff[LOG_BUFF_MAX][LOG_SIZE];
TEST(pikaMain, for_in_dict) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "print('__start__')\n"
            "dict = PikaStdData.Dict()\n"
            "dict.set('a', 1)\n"
            "dict.set('b', 2)\n"
            "dict.remove('b')\n"
            "for item in dict:\n"
            "    print(item)\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "a\r\n");
    EXPECT_STREQ(log_buff[1], "__start__\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, str_add) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = 'a'\n"
            "b = 'b'\n"
            "c = a + b\n"
            "d = 1\n"
            "e = 1.2\n"
            "g = c + str(d)\n"
            "h = c + str(d) + str(e)\n"
            "\n");
    /* collect */
    char* c = obj_getStr(pikaMain, "c");
    char* g = obj_getStr(pikaMain, "g");
    char* h = obj_getStr(pikaMain, "h");
    /* assert */
    EXPECT_STREQ(c, "ab");
    EXPECT_STREQ(g, "ab1");
    EXPECT_STREQ(h, "ab11.200000");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, str_add_print) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "print('test: ' + str(2233))\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "test: 2233\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, int_float_to_str) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = str(1)\n"
            "b = str(1.2)\n"
            "\n");
    /* collect */
    char* a = obj_getStr(pikaMain, "a");
    char* b = obj_getStr(pikaMain, "b");
    /* assert */
    EXPECT_STREQ(a, "1");
    EXPECT_STREQ(b, "1.200000");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, str_eq) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = '1ee'\n"
            "b = '1ee'\n"
            "c = '1e'\n"
            "if a==b:\n"
            "    res1 = 1\n"
            "else:\n"
            "    res1 = 0\n"
            "if a==c:\n"
            "    res2 = 1\n"
            "else:\n"
            "    res2 = 0\n"
            "\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");
    /* assert */
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 0);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, print_with_enter) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain, "print('test\\n')\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "test\n\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, print_with_2enter) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain, "print('test\\r\\n\\n')\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "test\r\n\n\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, print_ddd) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain, "print(\"[Info]: in Python config ...\")\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "[Info]: in Python config ...\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_in_string) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "s = PikaStdData.String('test')\n"
            "for c in s:\n"
            "    print(c)\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[4], "BEGIN\n");
    EXPECT_STREQ(log_buff[3], "t\r\n");
    EXPECT_STREQ(log_buff[2], "e\r\n");
    EXPECT_STREQ(log_buff[1], "s\r\n");
    EXPECT_STREQ(log_buff[0], "t\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_no_init_arg) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "s = PikaStdData.String()\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[3], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[2],
                 "TypeError: __init__() takes 1 positional argument but 0 were "
                 "given\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, obj_no_free) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "s = PikaStdData.String()\n"
            "s = PikaStdData.String()\n"
            "\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if 0
TEST(pikaMain, list__setitem__) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "list = PikaStdData.List()\n"
            "list = __setitem__(list, 0, 2)\n"
            "res = __getitem__(list, 0)\n"
            "\n");
    /* collect */
    int res = obj_getInt(pikaMain, "res");
    /* assert */
    EXPECT_EQ(res, 2);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, string__getitem__) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "s = PikaStdData.String('test')\n"
            "res = __getitem__(s, 2)\n"
            "\n");
    /* collect */
    char* res = obj_getStr(pikaMain, "res");
    /* assert */
    EXPECT_STREQ(res, "s");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, dict__setitem__get) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "list = PikaStdData.Dict()\n"
            "list = __setitem__(list, 'a', 2)\n"
            "res = __getitem__(list, 'a')\n"
            "\n");
    /* collect */
    int res = obj_getInt(pikaMain, "res");
    /* assert */
    EXPECT_EQ(res, 2);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, str___getitem____setitem__) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "s = 'test'\n"
            "res = __getitem__(s, 2)\n"
            "s = __setitem__(s, 2, 'q')\n"
            "\n");
    /* collect */
    char* res = obj_getStr(pikaMain, "res");
    char* s = obj_getStr(pikaMain, "s");
    /* assert */
    EXPECT_STREQ(res, "s");
    EXPECT_STREQ(s, "teqt");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, str__index__) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "s = 'test'\n"
            "res = s[2]\n"
            "res2 = 'eqrt'[2]\n"
            "s[2] = 'q'\n"
            "\n");
    /* collect */
    char* res = obj_getStr(pikaMain, "res");
    char* res2 = obj_getStr(pikaMain, "res2");
    char* s = obj_getStr(pikaMain, "s");
    /* assert */
    EXPECT_STREQ(res, "s");
    EXPECT_STREQ(res2, "r");
    EXPECT_STREQ(s, "teqt");
    // EXPECT_STREQ(s, "teqt");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, bytes__index__) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "s = b'test'\n"
            "res = s[2]\n"
            "res2 = b'eqrt'[2]\n"
            "s[2] = b'q'\n"
            "\n");
    /* collect */
    int res = obj_getInt(pikaMain, "res");
    int res2 = obj_getInt(pikaMain, "res2");
    uint8_t* s = obj_getBytes(pikaMain, "s");
    /* assert */
    EXPECT_EQ(res, 115);
    EXPECT_EQ(res2, 114);
    EXPECT_EQ(s[2], 'q');
    // EXPECT_STREQ(s, "teqt");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, list_index) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "list = PikaStdData.List()\n"
            "list.append(2)\n"
            "res = list[0]\n"
            "\n");
    /* collect */
    int res = obj_getInt(pikaMain, "res");
    /* assert */
    EXPECT_EQ(res, 2);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, dict_index) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\n");
    obj_run(pikaMain,
            "dict = PikaStdData.Dict()\n"
            "dict['a'] = 2\n"
            "res = dict['a']\n"
            "\n");
    /* collect */
    int res = obj_getInt(pikaMain, "res");
    /* assert */
    EXPECT_EQ(res, 2);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, task_run_once) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain,
            "def todo1():\n"
            "    print('task 1 running...')\n"
            "def todo2():\n"
            "    print('task 2 running...')\n"
            "task = GTestTask.Task()\n"
            "task.call_always(todo1)\n"
            "task.call_always(todo2)\n"
            "task.run_once()\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "task 2 running...\r\n");
    EXPECT_STREQ(log_buff[1], "task 1 running...\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, task_run_when) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_run(pikaMain,
            "def todo1():\n"
            "    print('task 1 running...')\n"
            "def todo2():\n"
            "    print('task 2 running...')\n"
            "def todo3():\n"
            "    print('task 3 running...')\n"
            "def when3():\n"
            "    return True\n"
            "task = GTestTask.Task()\n"
            "task.call_always(todo1)\n"
            "task.call_always(todo2)\n"
            "task.call_when(todo3, when3)\n"
            "task.run_once()\n"
            "\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "task 3 running...\r\n");
    EXPECT_STREQ(log_buff[1], "task 2 running...\r\n");
    EXPECT_STREQ(log_buff[2], "task 1 running...\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(pikaMain, task_run_period_until) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "import PikaStdLib\n"
            "mem = PikaStdLib.MemChecker()\n"
            "mem_now = 0\n"
            "mem_max = 0\n"
            "mem_lack = False\n"
            "def todo1():\n"
            "    global mem_now\n"
            "    global mem_lack\n"
            "    global mem_max\n"
            "    mem_max = mem.getMax()\n"
            "    print('task 1 running...')\n"
            "    if mem_now == 0:\n"
            "        mem_now = mem.getNow()\n"
            "    else:\n"
            "        if mem.getNow() > mem_now + 1:\n"
            "            mem_lack = True\n"
            "            print(cformat('%0.8f', mem.getNow()))\n"
            "def todo2():\n"
            "    print('task 2 running...')\n"
            "def todo3():\n"
            "    print('task 3 running...')\n"
            "def when3():\n"
            "    return True\n"
            "task = GTestTask.Task()\n"
            "task.call_period_ms(todo1, 200)\n"
            "task.call_period_ms(todo2, 500)\n"
            "# task.call_always(todo3)\n"
            "task.run_until_ms(1000)\n"
            "\n");
    /* collect */
    PIKA_BOOL mem_lack = obj_getBool(pikaMain, "mem_lack");
    /* assert */
    EXPECT_STREQ(log_buff[0], "task 1 running...\r\n");
    EXPECT_STREQ(log_buff[1], "task 1 running...\r\n");
    EXPECT_STREQ(log_buff[2], "task 2 running...\r\n");
    EXPECT_STREQ(log_buff[3], "task 1 running...\r\n");
    EXPECT_STREQ(log_buff[4], "task 1 running...\r\n");
    EXPECT_STREQ(log_buff[5], "task 2 running...\r\n");
    EXPECT_STREQ(log_buff[6], "task 1 running...\r\n");
    EXPECT_STREQ(log_buff[7], "BEGIN\r\n");
    EXPECT_EQ(mem_lack, PIKA_FALSE);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, fun_call) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "def fun(a,b,c,d,e):\n"
            "    print(a)\n"
            "    print(b)\n"
            "    print(c)\n"
            "    print(d)\n"
            "    print(e)\n"
            "fun(10,20,30,40, 'xxx')\n"
            "\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "xxx\r\n");
    EXPECT_STREQ(log_buff[1], "40\r\n");
    EXPECT_STREQ(log_buff[2], "30\r\n");
    EXPECT_STREQ(log_buff[3], "20\r\n");
    EXPECT_STREQ(log_buff[4], "10\r\n");
    EXPECT_STREQ(log_buff[5], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, global) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "x = 0\n"
            "a = 0\n"
            "z = 0\n"
            "def test_global():\n"
            "    global x\n"
            "    global y, z\n"
            "    x = 1\n"
            "    a = 1\n"
            "    z = 1\n"
            "    print('test global')\n"
            "test_global()\n"
            "\n");
    /* assert */
    int x = obj_getInt(pikaMain, "x");
    int a = obj_getInt(pikaMain, "a");
    int z = obj_getInt(pikaMain, "z");
    EXPECT_EQ(x, 1);
    EXPECT_EQ(a, 0);
    EXPECT_EQ(z, 1);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, complex_str) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "print('test,test')\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "test,test\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, syntax_err_1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, "print('testtest)\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], PIKA_ERR_STRING_SYNTAX_ERROR "\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_arg) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs,
                                   "class Test(TinyObj):\n"
                                   "    x = 1\n"
                                   "\n"
                                   "test = Test()\n"
                                   "print(test.x)\n");
    printf("%s", pikaAsm);
    pikaVM_runAsm(pikaMain, pikaAsm);
    /* assert */
    EXPECT_STREQ(log_buff[0], "1\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_def) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class Test():\n"
            "    x = 1\n"
            "    def hello(self, x):\n"
            "        return x + 2\n"
            "test = Test()\n"
            "x = test.hello(test.x)\n"
            "print(x)\n");
    /* collect */
    PikaObj* test = obj_getObj(self, "test");
    EXPECT_EQ(obj_refcntNow(test), 1);
    /* assert */
    EXPECT_STREQ(log_buff[0], "3\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_def_print) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class Test():\n"
            "    x = 1\n"
            "    def hi(self):\n"
            "        print('hi' + str(self.x))\n"
            "test = Test()\n"
            "test.hi()\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "hi1\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_demo_1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class MyClass:\n"
            "    i = 12345\n"
            "    def f(self):\n"
            "        return 'hello world'\n"
            "        \n"
            "x = MyClass()\n"
            " \n"
            "print(x.i)\n"
            "print(x.f())\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "hello world\r\n");
    EXPECT_STREQ(log_buff[1], "12345\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_demo_2) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class Complex:\n"
            "    def __init__(self):\n"
            "        self.r = 3.0\n"
            "        self.i = -4.5\n"
            "x = Complex()\n"
            "print(x.r)\n"
            "print(x.i)\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "-4.500000\r\n");
    EXPECT_STREQ(log_buff[1], "3.000000\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_demo_3) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
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
            "s.speak()\n");
    /* assert */
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], "i am a people\r\n");
    EXPECT_STREQ(log_buff[0], "i am a student\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_demo_save_asm) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class people:\n"
            "    def speak(self):\n"
            "        print('i am a people')\n"
            " \n"
            "class student(people):\n"
            "    def speak(self):\n"
            "        print('i am a student')\n"
            " \n");
    obj_run(self,
            "p = people()\n"
            "s = student()\n"
            "p.speak()\n"
            "s.speak()\n");
    /* assert */
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], "i am a people\r\n");
    EXPECT_STREQ(log_buff[0], "i am a student\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_if_continue_byte_code) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    pikaVM_run(pikaMain,
               "a = 0\n"
               "for i in range(0, 10):\n"
               "    if i == 5:\n"
               "        continue\n"
               "    a = a + i\n"
               "\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 40);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, print_in_def_byte_code) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_run(pikaMain,
               "def test_print():\n"
               "    print('test')\n"
               "test_print()\n");
    /* collect */
    /* assert */
    /* should only print once, so the second log (log_buff[1]) shuold be '\0' */
    EXPECT_STREQ(log_buff[0], "test\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_demo_1_file) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    char lines[] =
        "class MyClass:\n"
        "    i = 12345\n"
        "    def f(self):\n"
        "        return 'hello world'\n"
        "        \n"
        "x = MyClass()\n"
        " \n"
        "print(x.i)\n"
        "print(x.f())\n";
    pikaCompile("pika_bytecode.bin", lines);
    char bytecodebuff[4096] = {0};
    FILE* f = __platform_fopen("pika_bytecode.bin", "rb");
    printf("size: %ld\n", __platform_fread(bytecodebuff, 1, 4096, f));
    __platform_printf("BEGIN\r\n");
    pikaVM_runByteCode(self, (uint8_t*)bytecodebuff);
    /* assert */
    EXPECT_STREQ(log_buff[0], "hello world\r\n");
    EXPECT_STREQ(log_buff[1], "12345\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    __platform_fclose(f);
    EXPECT_EQ(pikaMemNow(), 0);
}

// #if !PIKA_NANO_ENABLE
// TEST(pikaMain, get_native_method) {
//     /* init */
//     g_PikaMemInfo.heapUsedMax = 0;
//     PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
//     __platform_printf("BEGIN\r\n");
//     /* do some thing */
//     Args args = {0};
//     args_setStr(&args, "val", "t");
//     obj_runNativeMethod(pikaMain, "ord", &args);
//     args_deinit_stack(&args);
//     /* assert */
//     EXPECT_STREQ(log_buff[0], "test\r\n");
//     EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
//     /* deinit */
//     obj_deinit(pikaMain);
//     EXPECT_EQ(pikaMemNow(), 0);
// }
// #endif

TEST(pikaMain, hex_list) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_run(pikaMain,
               "bytes = PikaStdData.List()\n"
               "bytes.append(0x0F)\n"
               "bytes.append(0xFF)\n"
               "sum = 0\n"
               "for byte in bytes:\n"
               "    sum = sum + byte\n"
               "\n"
               "print(sum)\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "270\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, bytearray) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_run(pikaMain,
               "bytes = PikaStdData.ByteArray(b'test')\n"
               "sum = 0\n"
               "for byte in bytes:\n"
               "    sum += byte\n"
               "print(bytes)\n"
               "\n");
    /* collect */
    int sum = obj_getInt(pikaMain, "sum");
    /* assert */
    EXPECT_EQ(sum, 448);
    EXPECT_STREQ(log_buff[0], "bytearray(b'\\x74\\x65\\x73\\x74')\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, not_4_space) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_run(pikaMain, "  print('test')\n");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], PIKA_ERR_STRING_SYNTAX_ERROR "\n");
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, self_operator) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_run(pikaMain,
               "a = 1\n"
               "b = 2\n"
               "c = 3\n"
               "d = 4\n"
               "a += 2\n"
               "b -= 1+1-3\n"
               "c *= 4\n"
               "d /= 2\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    int b = obj_getInt(pikaMain, "b");
    int c = obj_getInt(pikaMain, "c");
    int d = obj_getInt(pikaMain, "d");
    /* assert */
    EXPECT_EQ(a, 3);
    EXPECT_EQ(b, 3);
    EXPECT_EQ(c, 12);
    EXPECT_EQ(d, 2);
    /* deinit */
    obj_deinit(pikaMain);
    /* mem check */
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_demo_2_initwitharg) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class Complex:\n"
            "    def __init__(self, r, i):\n"
            "        self.r = r\n"
            "        self.i = i\n"
            "x = Complex(3.0, -4.5)\n"
            "print(x.r)\n"
            "print(x.i)\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "-4.500000\r\n");
    EXPECT_STREQ(log_buff[1], "3.000000\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, def_args_err) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "print()\n");
    // obj_run(pikaMain, "printNoEnd()\n");
    /* collect */

    /* assert */

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

// TEST(pikaMain, class_args_err) {
//     /* init */
//     g_PikaMemInfo.heapUsedMax = 0;
//     PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
//     /* run */
//     obj_run(pikaMain, "mem = PikaStdLib.MemChecker(0)\n");
//     /* collect */

//     /* assert */
//     EXPECT_STREQ(log_buff[4],
//                  "TypeError: PikaStdLib.MemChecker() takes no
//                  arguments\r\n");
//     /* deinit */
//     obj_deinit(pikaMain);
//     EXPECT_EQ(pikaMemNow(), 0);
// }

TEST(pikaMain, int_) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "a = int('3')\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 3);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, len_) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, "a = len('331dd')\n");
    /* collect */
    int a = obj_getInt(pikaMain, "a");
    /* assert */
    EXPECT_EQ(a, 5);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, def_in_shell) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);

    /* skip the first obj_run */
    __platform_printf("BEGIN\r\n");
    /* as run in shell */
    obj_run(pikaMain,
            "def test():\n"
            "    print('test')\n"
            "\n");
    obj_run(pikaMain,
            "def test2():\n"
            "    print('test2')\n"
            "\n");
    obj_run(pikaMain, "test()\n");
    obj_run(pikaMain, "test2()\n");
    /* collect */
    /* assert */
    EXPECT_STREQ("test2\r\n", log_buff[0]);
    EXPECT_STREQ("test\r\n", log_buff[1]);
    EXPECT_STREQ("BEGIN\r\n", log_buff[2]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, def_in_shell_override) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);

    /* skip the first obj_run */
    __platform_printf("BEGIN\r\n");
    /* as run in shell */
    obj_run(pikaMain,
            "def test():\n"
            "    print('test')\n"
            "\n");
    obj_run(pikaMain, "test()\n");
    obj_run(pikaMain,
            "def test():\n"
            "    print('test2')\n"
            "\n");
    obj_run(pikaMain, "test()\n");
    /* collect */
    /* assert */
    EXPECT_STREQ("test2\r\n", log_buff[0]);
    EXPECT_STREQ("test\r\n", log_buff[1]);
    EXPECT_STREQ("BEGIN\r\n", log_buff[2]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_in_shell) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);

    /* skip the first obj_run */
    __platform_printf("BEGIN\r\n");
    /* as run in shell */
    obj_run(pikaMain,
            "class Test():\n"
            "    def hi(self):\n"
            "        print('hi')\n"
            "\n");
    obj_run(pikaMain,
            "t = Test()\n"
            "t.hi()\n");
    /* collect */
    /* assert */
    EXPECT_STREQ("hi\r\n", log_buff[0]);
    EXPECT_STREQ("BEGIN\r\n", log_buff[1]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class_TinyObj) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);

    /* skip the first obj_run */
    __platform_printf("BEGIN\r\n");
    /* as run in shell */
    obj_run(pikaMain,
            "class MyClass( TinyObj ):\n"
            "    def __init__(self):\n"
            "        self.val = 123\n"
            "\n");
    obj_run(pikaMain,
            "x = MyClass()\n"
            "x.val\n");
    /* collect */
    /* assert */
    EXPECT_STREQ("123\r\n", log_buff[0]);
    EXPECT_STREQ("BEGIN\r\n", log_buff[1]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, list_sysobj) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);

    /* skip the first obj_run */
    obj_run(pikaMain,
            "a = list()\n"
            "a.len()\n");
    /* as run in shell */
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_BUILTIN_STRUCT_ENABLE
TEST(pikaMain, list_init) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "a = [1, 2, 3]\n"
            "a.len()\n");
    /* as run in shell */
    /* collect */
    /* assert */
    EXPECT_STREQ("3\r\n", log_buff[0]);
    EXPECT_STREQ("BEGIN\r\n", log_buff[1]);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, ctypes) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    char* lines =
        "t = ctypes.Test()\n"
        "sendbuf = b'\\x03\\x04\\x33\\x00\\x05'\n"
        "rlen = ctypes.c_uint(0)\n"
        "rcvbuf = ctypes.c_char_p('')\n"
        "res = t.dc_cpuapdu_hex(5, sendbuf, rlen, rcvbuf)\n"
        "t.print_rcv(rcvbuf)\n";
    __platform_printf("%s\n", lines);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* as run in shell */
    /* collect */
    Arg* sendbuf = obj_getArg(pikaMain, "sendbuf");
    Arg* rcvbuf = obj_getArg(pikaMain, "rcvbuf.value");
    /* assert */
    EXPECT_EQ(arg_getBytesSize(sendbuf), 5);
    EXPECT_EQ(arg_getBytesSize(rcvbuf), 6);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, import_err) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    char* lines = "import qqpe\n";
    __platform_printf("%s\n", lines);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* as run in shell */
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[2], "ModuleNotFoundError: No module named 'qqpe'\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, cmodule_in_py) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = pikaScriptInit();
    char* lines =
        "import test_cmodule\n"
        "test_cmodule.test_mem()\n";
    __platform_printf("%s\n", lines);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* as run in shell */
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[1], "mem used max:\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, class___str___print) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class Test():\n"
            "    def __str__(self):\n"
            "        return 'test'\n"
            "test = Test()\n"
            "print(test)\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "test\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, CModule__str__) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "op = PikaMath.Operator()\n"
            "print(op)\n"
            "print('obj: ' + str(op))\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "obj: test\r\n");
    EXPECT_STREQ(log_buff[1], "test\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_STREQ(log_buff[0], "del operator...\r\n");
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(pikaMain, builtin_hex) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "hex(3)\n"
            "hex(-3)\n"
            "hex(1234)\n"
            "hex(-12039)\n");
    /* assert */
    EXPECT_STREQ(log_buff[4], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[3], "'0x03'\r\n");
    EXPECT_STREQ(log_buff[2], "'-0x03'\r\n");
    EXPECT_STREQ(log_buff[1], "'0x4d2'\r\n");
    EXPECT_STREQ(log_buff[0], "'-0x2f07'\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(pikaMain, builtin_ord) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "ord('a')\n");
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "97\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(pikaMain, builtin_chr) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "chr(97)\n");
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "'a'\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, iteral_oct) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "0o100\n");
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "64\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, iteral_bin) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "0b100\n");
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "4\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_push_mode) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    char lines[] =
        "print('test')\n"
        "for i in range(0, 10):\n"
        "    i\n"
        "\n";
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(self, lines[i]);
    }
    /* assert */
    // EXPECT_STREQ(log_buff[18], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[14], "test\r\n");
    EXPECT_STREQ(log_buff[11], "... ");
    EXPECT_STREQ(log_buff[1], "9\r\n");
    EXPECT_STREQ(log_buff[0], ">>> ");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_push_mode_err) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    char lines[] =
        "def test:\n"
        "    print('test')\n"
        "\n";
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(self, lines[i]);
    }
    /* assert */
    EXPECT_STREQ(log_buff[1], PIKA_ERR_STRING_SYNTAX_ERROR "\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if 0
TEST(pikaMain, REPL_input) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    char lines[] =
        "res = input('input:\\n')\n"
        "1234\n"
        "print(res)\n";
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(self, lines[i]);
    }
    /* assert */
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, int_from_bytes) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "int(b'test'[0])\n");
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "116\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

// TEST(pikaMain, big_bytes) {
//     /* init */
//     g_PikaMemInfo.heapUsedMax = 0;
//     /* run */
//     PikaObj* self = newRootObj("pikaMain", New_PikaMain);
//     __platform_printf("BEGIN\r\n");
//     obj_run(self, "b = bytes(8192)\n");
//     /* assert */
//     /* deinit */
//     obj_deinit(self);
//     EXPECT_EQ(pikaMemNow(), 0);
// }

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, utils_int_to_bytes) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "ut = PikaStdData.Utils()\n"
            "b = b'test'\n"
            "b[0] = ut.int_to_bytes(0x35)\n"
            "b\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "b'\\x35\\x65\\x73\\x74'\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, neg_index) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "'test'[-2]\n"
            "b'test'[-2]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "115\r\n");
    EXPECT_STREQ(log_buff[1], "'s'\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, slice1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "'test'[1:3]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "'es'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, slice2) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "b'test'[1:3]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "b'\\x65\\x73'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, c_buffer) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "version = ctypes.c_buffer(b\"\", 128)\n");
    /* assert */
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, str_add1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "i = 32\n"
            "msg = \"device_names[\" + str(i) + \"]:\"\n"
            "msg\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "'device_names[32]:'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, slice_eei) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "'test'[1:-1]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "'es'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, slice_a97) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "'test'[:-2]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "'te'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, slice_a90) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "'test'[1:]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "'est'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, slice_a12) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "b'test'[1:]\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "b'\\x65\\x73\\x74'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, str_string) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "a = PikaStdData.String('test')\n"
            "print(a)\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "test\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, string_index) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "a = PikaStdData.String('test')\n"
            "a[1]\n"
            "a[1] = 'q'\n"
            "print(a)\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "tqst\r\n");
    EXPECT_STREQ(log_buff[1], "'e'\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, list_for_append) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "rcv_buf = PikaStdData.List()\n"
            "for i in range(0, 1024):\n"
            "    rcv_buf.append(0)\n"
            "rcv_buf.len()\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "1024\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_str) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "str(PikaStdData.String('test'))\n");
    /* assert */
    EXPECT_STREQ(log_buff[0], "'test'\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, module_import_from_module) {
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(self, pikaModules_py_a);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "import test_module1\n"
            "test_module1.test_module_import()\n");
    EXPECT_STREQ(log_buff[0], "test_module_2_hello\r\n");
    EXPECT_STREQ(log_buff[1], "in test module 2\r\n");
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, module_1_module2_test) {
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(self, pikaModules_py_a);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "import test_module1\n"
            "test_module1.test_module2.mytest()\n");
    EXPECT_STREQ(log_buff[0], "test_module_2_hello\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_startswith) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test')\n"
            "res1 = a.startswith('te')\n"
            "res2 = a.startswith('st')\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");

    /* assert */
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_endswith) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test')\n"
            "res1 = a.endswith('te')\n"
            "res2 = a.endswith('st')\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");

    /* assert */
    EXPECT_EQ(res1, 0);
    EXPECT_EQ(res2, 1);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_isdigit) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test')\n"
            "b = PikaStdData.String('1234')\n"
            "c = PikaStdData.String('test1234')\n"
            "res1 = a.isdigit()\n"
            "res2 = b.isdigit()\n"
            "res3 = c.isdigit()\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");
    int res3 = obj_getInt(pikaMain, "res3");

    /* assert */
    EXPECT_EQ(res1, 0);
    EXPECT_EQ(res2, 1);
    EXPECT_EQ(res3, 0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_islower) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test')\n"
            "b = PikaStdData.String('1234')\n"
            "c = PikaStdData.String('Test')\n"
            "res1 = a.islower()\n"
            "res2 = b.islower()\n"
            "res3 = c.islower()\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");
    int res3 = obj_getInt(pikaMain, "res3");

    /* assert */
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 0);
    EXPECT_EQ(res3, 0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_isalnum) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test1234')\n"
            "b = PikaStdData.String('1234')\n"
            "c = PikaStdData.String('  ')\n"
            "res1 = a.isalnum()\n"
            "res2 = b.isalnum()\n"
            "res3 = c.isalnum()\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");
    int res3 = obj_getInt(pikaMain, "res3");

    /* assert */
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 1);
    EXPECT_EQ(res3, 0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_isalpha) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test')\n"
            "b = PikaStdData.String('1234')\n"
            "c = PikaStdData.String('  ')\n"
            "res1 = a.isalpha()\n"
            "res2 = b.isalpha()\n"
            "res3 = c.isalpha()\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");
    int res3 = obj_getInt(pikaMain, "res3");

    /* assert */
    EXPECT_EQ(res1, 1);
    EXPECT_EQ(res2, 0);
    EXPECT_EQ(res3, 0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, string_isspace) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "a = PikaStdData.String('test')\n"
            "b = PikaStdData.String('  ')\n"
            "res1 = a.isspace()\n"
            "res2 = b.isspace()\n");
    /* collect */
    int res1 = obj_getInt(pikaMain, "res1");
    int res2 = obj_getInt(pikaMain, "res2");

    /* assert */
    EXPECT_EQ(res1, 0);
    EXPECT_EQ(res2, 1);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, print_obj) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "mem = PikaStdLib.MemChecker()\n"
            "mem\n"
            "print(mem)\n"
            "res = str(mem)\n");
    /* collect */
    char* res = obj_getStr(pikaMain, "res");

    /* assert */
    EXPECT_EQ(strIsStartWith(log_buff[0], "<object at "), true);
    EXPECT_EQ(strIsStartWith(log_buff[1], "<object at "), true);
    EXPECT_STREQ(log_buff[2], "BEGIN\r\n");
    EXPECT_EQ(strIsStartWith(res, "<object at "), true);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, num_issue1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    char* lines = "(((1 + (2 * 3)/(4 + 5))*(6 - 7) + (8 + 9) * 10)/11) - 12\n";
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[0], "3.303030\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, returnNullString) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    char* lines =
        "import GTestTask\n"
        "task = GTestTask.Task()\n"
        "def test_return_none():\n"
        "    s = task.returnNullString()\n"
        "    return s\n"
        "s = test_return_none()\n"
        "print(s)\n"
        "if None == s:\n"
        "    print('s is None')\n";
    obj_run(pikaMain, lines);
    /* collect */
    Arg* s = obj_getArg(pikaMain, "s");
    /* assert */
    EXPECT_EQ((uintptr_t)s, NULL);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, for_loop_issue_1b2a3f1bdf) {
    char* lines =
        "res = 0\n"
        "for i in range(0, 10):\n"
        "    if i < 3:\n"
        "        continue\n"
        "    if i > 7:\n"
        "        continue\n"
        "    for i in range(i, i+3):\n"
        "        res += i\n"
        "\n";

    Args* buffs = New_strBuff();
    __platform_printf("%s\n", lines);
    char* pikaAsm = pika_lines2Asm(buffs, lines);
    __platform_printf("%s", pikaAsm);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);

    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");

    obj_run(pikaMain, lines);
    /* collect */
    int res = obj_getInt(pikaMain, "res");
    /* assert */
    EXPECT_EQ(res, 90);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#define pika_floatYPE_DOUBLE 0
TEST(pikaMain, syantex_issue123lkjxi) {
    char* lines = "if i < 3\n";
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], PIKA_ERR_STRING_SYNTAX_ERROR "\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, syantex_issue_fae13) {
    char* lines = "for i < 3\n";
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], PIKA_ERR_STRING_SYNTAX_ERROR "\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, syantex_issue_1289341) {
    char* lines = "class lkj\n";
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], PIKA_ERR_STRING_SYNTAX_ERROR "\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, syantex_issue_183571) {
    char* lines = "def test\n";
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], PIKA_ERR_STRING_SYNTAX_ERROR "\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, dump_issue_12l3kjioa) {
    char* lines =
        "if i = 1:\n"
        "    print('test')\n"
        "\n";
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(pikaMain, issue_fa13f4) {
    char* lines =
        "d = {'language':['c','python']}\n"
        "d['language'].append('Java')\n"
        "print(d)\n";
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "{'language': ['c', 'python', 'Java']}\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, callback_issue_I5L1MI) {
    char* lines =
        "def callback(func):\n"
        "    func()\n"
        "\n"
        "def test():\n"
        "    print('hello')\n"
        "\n"
        "f = test\n"
        "callback(f)\n";
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain, lines);
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "hello\r\n");
    EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, runchar_multiline) {
    char* lines =
        "def test():\r\n"
        "    print('hello')\r\n"
        "\r\n"
        "test()\r\n";
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "hello\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, callback_run_char) {
    char* lines =
        "def test2():\r\n"
        "    print('hello')\r\n"
        "\r\n"
        "def test():\r\n"
        "    print('hello')\r\n"
        "    print('in test')\r\n"
        "\r\n"
        "def test3():\r\n"
        "    print('hello3')\r\n"
        "\r\n"
        "test()\r\n";
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "in test\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_backspace) {
    char* lines = "print('test'\b\b')\r\n";
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "tes\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_TAB_ENABLE
TEST(pikaMain, REPL_tab_completion) {
    char lines[] = {'p', 'r', 0x09, '(', '\"', 't',  'e',
                    's', 't', '\"', ')', '\r', '\n', 0x00};
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "test\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(pikaMain, REPL_backspace_issue_1) {
    char* lines = "print('test'\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b'a'\r\n";
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "'a'\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_backspace_issue_2) {
    char* lines = "loop\bp\n";
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_key_left) {
    char lines[] = {'1',  '2',           '+', '3',  '4', 0x1b,
                    0x5b, PIKA_KEY_LEFT, '5', '\n', 0x00};
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "366\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_key_left_del) {
    char lines[] = {'1',  '2',           '+',  '3',  '4', 0x1b,
                    0x5b, PIKA_KEY_LEFT, '\b', '\n', 0x00};
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "16\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SHELL_HISTORY_ENABLE
TEST(pikaMain, REPL_key_up) {
    char lines[] = {'1',  '2',  '+',         '3', '4',  '5', '\n',
                    0x1b, 0x5b, PIKA_KEY_UP, '2', '\n', 0x00};
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "3464\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_key_up_void) {
    char lines[] = {0x1b, 0x5b, PIKA_KEY_UP, '2', '\n', 0x00};
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "2\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_key_up_2) {
    char lines[] = {'1',         '2',  '+',  '3',         '4',  '5',  '\n',
                    '1',         '3',  '*',  '2',         '\n', 0x1b, 0x5b,
                    PIKA_KEY_UP, 0x1b, 0x5b, PIKA_KEY_UP, '2',  '\n', 0x00};
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "3464\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_key_down) {
    char lines[] = {'1',  '2',  '+',           '3',  '4',  '5',
                    '\n', '1',  '3',           '*',  '2',  '\n',
                    0x1b, 0x5b, PIKA_KEY_UP,   0x1b, 0x5b, PIKA_KEY_UP,
                    0x1b, 0x5b, PIKA_KEY_DOWN, '2',  '\n', 0x00};
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "286\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_key_down_over) {
    char lines[] = {'1',  '2',  '+',           '3',  '4',  '5',
                    '\n', '1',  '3',           '*',  '2',  '\n',
                    0x1b, 0x5b, PIKA_KEY_UP,   0x1b, 0x5b, PIKA_KEY_UP,
                    0x1b, 0x5b, PIKA_KEY_DOWN, 0x1b, 0x5b, PIKA_KEY_DOWN,
                    0x1b, 0x5b, PIKA_KEY_DOWN, '2',  '\n', 0x00};
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "2\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, REPL_pdb_set_break) {
    char lines[] = "pdb.set_break('pdb_set_break', 36)\nn\nn\nn\n";
    write_to_getchar_buffer(lines, strGetSize(lines));
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    pikaVM_runSingleFile(pikaMain, "test/python/builtins/pdb_set_break.py");
    /* run */
    __platform_printf("BEGIN\r\n");
    /* collect */
    /* assert */
    // EXPECT_STREQ(log_buff[1], "2\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif

#if PIKA_SHELL_FILTER_ENABLE

TEST(pikaMain, SHELL_filter_hi_pika) {
    char lines[] = {"###Hi Pika"};
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "Yes, I am here\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, SHELL_filter_hi_pika_sence) {
    char lines[] = {"###hi pika"};
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], ">>> ");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, SHELL_filter_bye_pika_sence) {
    char lines[] = {"###Bye piKa"};
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0], "OK, see you\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, SHELL_filter_bye_pika_nomache) {
    char lines[] = {"print('###Hi Pika')\nprint('###Hi_Pika')\n"};
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[1], "###Hi_Pika\r\n");
    EXPECT_STREQ(log_buff[4], "Yes, I am here\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, mem_now) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(pikaMain,
            "from PikaStdLib import MemChecker as mem\n"
            "mem.now()\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif

TEST(pikaMain, obj_setNone) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_setNone(pikaMain, "n");
    obj_run(pikaMain, "print(n)");
    EXPECT_STREQ(log_buff[0], "None\r\n");
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, obj_setStr_NULL) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    obj_setStr(pikaMain, "n", NULL);
    obj_run(pikaMain, "print(n)");
    EXPECT_STREQ(log_buff[2], "NameError: name 'n' is not defined\n");
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(pikaMain, _obj_set_str_null) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    obj_run(pikaMain,
            "class _test(GTestTask._test):\n"
            "    def __init__(self):\n"
            "        super().__init__()\n"
            "t = _test()\n");
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#if !PIKA_NANO_ENABLE
TEST(pikaMain, shell_nolib_print_help) {
    char lines[] = {"help(\"modules\")\n"};
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    for (size_t i = 0; i < strGetSize(lines); i++) {
        obj_runChar(pikaMain, lines[i]);
    }
    /* assert */
    EXPECT_STREQ(
        log_buff[1],
        "Error: Not found LibObj, please execute obj_linkLibrary()\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST_END
