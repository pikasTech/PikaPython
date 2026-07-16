#include "test_common.h"
TEST_START

// #define _SKIP_EVENTLOOP_TEST

#if !PIKA_NANO_ENABLE
TEST_RUN_SINGLE_FILE(thread, test2, "test/python/_thread/test2.py")

#ifndef _SKIP_EVENTLOOP_TEST
TEST_RUN_SINGLE_FILE(eventloop, test1, "test/python/eventloop/test1.py")
TEST_RUN_SINGLE_FILE(eventloop, test2, "test/python/eventloop/test2.py")
TEST_RUN_SINGLE_FILE(eventloop, test3, "test/python/eventloop/test3.py")
TEST_RUN_SINGLE_FILE(eventloop, delay1, "test/python/eventloop/delay1.py")
TEST_RUN_SINGLE_FILE(eventloop, once1, "test/python/eventloop/once1.py")
TEST_RUN_SINGLE_FILE(eventloop, once2, "test/python/eventloop/once2.py")
#endif

TEST_RUN_SINGLE_FILE(fsm, test1, "test/python/fsm/test1.py")

TEST_RUN_SINGLE_FILE_PASS(builtin,
                          list_tuple_equ,
                          "test/python/builtins/list_tuple_equ.py")
TEST_RUN_SINGLE_FILE_PASS(builtin,
                          fn_default1,
                          "test/python/builtins/fn_default1.py")
TEST_RUN_SINGLE_FILE_PASS(builtin,
                          fn_default_tuple,
                          "test/python/builtins/fn_default_tuple.py")
TEST_RUN_SINGLE_FILE_PASS(builtin,
                          fn_default_vars,
                          "test/python/builtins/fn_default_vars.py")
TEST_RUN_SINGLE_FILE_PASS(builtin,
                          fn_kwargs_order,
                          "test/python/builtins/fn_kwargs_order.py")
TEST_RUN_SINGLE_FILE_PASS(builtin,
                          fn_kwargs_required,
                          "test/python/builtins/fn_kwargs_required.py")
TEST_RUN_SINGLE_FILE_PASS(builtin,
                          fn_too_many_bound_args,
                          "test/python/builtins/fn_too_many_bound_args.py")
TEST_RUN_SINGLE_FILE_PASS(builtin,
                          fn_many_vars,
                          "test/python/builtins/fn_many_vars.py")
TEST_RUN_SINGLE_FILE_PASS(builtin, max_min, "test/python/builtins/max_min.py")
TEST_RUN_SINGLE_FILE_PASS(builtin, split, "test/python/builtins/split.py")
TEST_RUN_LINES_EXCEPT_OUTPUT(builtin,
                             split_slice,
                             "'test'.split('e')[0]",
                             "'t'\r\n")
TEST_RUN_LINES_EXCEPT_OUTPUT(builtin,
                             split_slice_1,
                             "'test'.split('e')[1]",
                             "'st'\r\n")
TEST_RUN_LINES_EXCEPT_OUTPUT(builtin,
                             replace_split_0,
                             "'a b c d'.replace(' ', ',').split(',')[0]",
                             "'a'\r\n")
TEST_RUN_SINGLE_FILE_EXCEPT_OUTPUT(builtin,
                                   class_script,
                                   "test/python/builtins/class_script.py",
                                   "Obj1.test\r\n")
TEST_RUN_SINGLE_FILE_EXCEPT_OUTPUT(builtin,
                                   class_hint,
                                   "test/python/builtins/class_hint.py",
                                   "1\r\n")
TEST_RUN_SINGLE_FILE_PASS(builtin,
                          isinstance,
                          "test/python/builtins/isinstance.py")
TEST_RUN_SINGLE_FILE_PASS(builtin, getitem, "test/python/builtins/getitem.py")
TEST_RUN_SINGLE_FILE_PASS(lua, eval, "test/python/pika_lua/eval.py")
TEST_RUN_SINGLE_FILE_PASS(lua, require, "test/python/pika_lua/require.py")

TEST_RUN_LINES(vm, dot_issue, ".")
TEST_RUN_LINES(vm, char_issue1, "~")
TEST_RUN_LINES(vm, char_issue2, "/")
TEST_RUN_LINES(vm, char_issue3, "%")
TEST_RUN_LINES(vm, char_issue4, "=")
TEST(parser, issue322_unsupported_nested_unpack) {
    char source[] =
        "dict = {\n"
        "    'a': {1, 2, 3},\n"
        "    'b': {4, 5, 6},\n"
        "    'c': {7, 8, 9},\n"
        "}\n"
        "for k, (a, b, c) in dict.items():\n"
        "    print(k, ':', a, b, c)\n";
    ByteCodeFrame bytecode_frame = {0};
    byteCodeFrame_init(&bytecode_frame);
    EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source),
              PIKA_RES_ERR_SYNTAX_ERROR);
    byteCodeFrame_deinit(&bytecode_frame);
}
TEST(parser, malformed_syntax_returns_error) {
    char invalid_def[] = "def broken(:\n    pass\n";
    char missing_colon[] = "if True\n    pass\n";
    char missing_bracket[] = "value = [1, 2\n";
    char leading_comma[] = "def f(,a):\n    pass\n";
    char duplicate_comma[] = "def f(a,,b):\n    pass\n";
    char* sources[] = {invalid_def,   missing_colon,   missing_bracket,
                       leading_comma, duplicate_comma};
    for (char* source : sources) {
        ByteCodeFrame bytecode_frame = {0};
        byteCodeFrame_init(&bytecode_frame);
        EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source),
                  PIKA_RES_ERR_SYNTAX_ERROR);
        byteCodeFrame_deinit(&bytecode_frame);
    }
}
TEST(parser, malformed_syntax_batch_probe) {
    char default_missing[] = "def f(a=):\n    pass\n";
    char bare_star[] = "def f(*):\n    pass\n";
    char bare_double_star[] = "def f(**):\n    pass\n";
    char call_leading_comma[] = "f(,1)\n";
    char call_duplicate_comma[] = "f(1,,2)\n";
    char call_keyword_comma[] = "f(a=1,,b=2)\n";
    char list_duplicate_comma[] = "x = [1,,2]\n";
    char tuple_duplicate_comma[] = "x = (1,,2)\n";
    char dict_missing_value[] = "x = {'a':}\n";
    char operator_missing_rhs[] = "x = 1 +\n";
    char assignment_missing_rhs[] = "x =\n";
    char parenthesized_missing_rhs[] = "x = (1 + )\n";
    char if_missing_condition[] = "if :\n    pass\n";
    char while_missing_condition[] = "while :\n    pass\n";
    char for_missing_target[] = "for in []:\n    pass\n";
    char* sources[] = {
        default_missing,          bare_star,
        bare_double_star,         call_leading_comma,
        call_duplicate_comma,     call_keyword_comma,
        list_duplicate_comma,     tuple_duplicate_comma,
        dict_missing_value,       operator_missing_rhs,
        assignment_missing_rhs,   parenthesized_missing_rhs,
        if_missing_condition,     while_missing_condition,
        for_missing_target,
    };
    for (char* source : sources) {
        SCOPED_TRACE(source);
        ByteCodeFrame bytecode_frame = {0};
        byteCodeFrame_init(&bytecode_frame);
        EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source),
                  PIKA_RES_ERR_SYNTAX_ERROR);
        byteCodeFrame_deinit(&bytecode_frame);
    }
}
TEST(parser, malformed_syntax_batch_valid_neighbors) {
    char source[] =
        "def empty():\n"
        "    pass\n"
        "def trailing(a,):\n"
        "    return a\n"
        "def nested(a=(1, 2), b=',,'):\n"
        "    return a\n"
        "values = [1,]\n"
        "pair = (1,)\n"
        "mapping = {'a': 1}\n"
        "text = ',, + }'\n"
        "empty()\n"
        "trailing(1,)\n";
    ByteCodeFrame bytecode_frame = {0};
    byteCodeFrame_init(&bytecode_frame);
    EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source), PIKA_RES_OK);
    byteCodeFrame_deinit(&bytecode_frame);
}
TEST_RUN_SINGLE_FILE(vm,
                     issue_star_dict,
                     "test/python/issue/issue_star_dict.py")
TEST_RUN_SINGLE_FILE_PASS(vm, proxy2, "test/python/proxy/proxy2.py")
TEST_RUN_LINES(vm, abs_none, "abs(None)")
TEST_RUN_LINES(vm, abs_str, "abs('test')")
TEST_RUN_SINGLE_FILE_PASS(datastruct,
                          circlequeue,
                          "test/python/datastruct/circlequeue.py")

TEST_RUN_LINES_EXCEPT_OUTPUT(vm, single_tuple, "(1,)", "(1,)\r\n")
TEST_RUN_LINES_EXCEPT_OUTPUT(vm, single_tuple_str, "('test',)", "('test',)\r\n")
TEST_RUN_SINGLE_FILE_PASS(vm, is_not, "test/python/builtins/is_not.py")
TEST_RUN_LINES(vm,
               var_global,
               "import PikaStdLib\n"
               "mem = PikaStdLib.MemChecker()\n"
               "mem.clear\n")
TEST_RUN_LINES(vm,
               var_global_run,
               "import PikaStdLib\n"
               "mem = PikaStdLib.MemChecker()\n"
               "mem.clear()\n")
TEST_RUN_LINES(vm,
               var_global_module,
               "import configparser\n"
               "configparser.clear\n")
TEST_RUN_LINES(vm, import_void, "import \n")
TEST_RUN_SINGLE_FILE_PASS(vm, fn_fn, "test/python/builtins/fn_fn.py")
TEST_RUN_LINES_EXCEPT_OUTPUT(vm, isinstance, "isinstance(1, int)\n", "True\r\n")
TEST_RUN_SINGLE_FILE_PASS(except,
                          try_while_return,
                          "test/python/except/try_while_return.py");
TEST_RUN_SINGLE_FILE_PASS(except,
                          isinstance,
                          "test/python/except/except_isinstance.py");

TEST_RUN_SINGLE_FILE_PASS(zlib, zlib1, "test/python/zlib/zlib1.py")
TEST_RUN_SINGLE_FILE(zlib, zlib_err, "test/python/zlib/zlib_err.py")
TEST_RUN_SINGLE_FILE(modbus, rtu_request, "test/python/modbus/rtu_request.py")

TEST_RUN_SINGLE_FILE(PikaStdDevice,
                     inhert,
                     "test/python/PikaStdDevice/inhert.py")
TEST_RUN_SINGLE_FILE_EXCEPT_OUTPUT(socket,
                                   socket_download,
                                   "test/python/socket/socket_download.py",
                                   "PASS\r\n")

TEST_RUN_SINGLE_FILE_PASS(builtin,
                          issue_isinstance,
                          "test/python/builtins/issue_isinstance.py");
#endif

TEST_END
