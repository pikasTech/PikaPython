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
        "values = [1, 2]\n"
        "left, right = values\n"
        "mapping['a'] = left\n"
        "copy = [item for item in values]\n"
        "empty()\n"
        "trailing(1,)\n";
    ByteCodeFrame bytecode_frame = {0};
    byteCodeFrame_init(&bytecode_frame);
    EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source), PIKA_RES_OK);
    byteCodeFrame_deinit(&bytecode_frame);
}
TEST(parser, semantic_syntax_batch_probe) {
    char required_after_default[] = "def f(a=1, b):\n    pass\n";
    char duplicate_parameter[] = "def f(a, a):\n    pass\n";
    char positional_after_keyword[] = "f(a=1, 2)\n";
    char duplicate_keyword[] = "f(a=1, a=2)\n";
    char dict_missing_colon[] = "x = {'a' 1}\n";
    char nested_positional_after_keyword[] = "f(g(a=1, 2))\n";
    char nested_duplicate_keyword[] = "f(g(a=1, a=2))\n";
    char nested_dict_missing_colon[] = "x = [{'a' 1}]\n";
    char* sources[] = {required_after_default, duplicate_parameter,
                       positional_after_keyword, duplicate_keyword,
                       dict_missing_colon, nested_positional_after_keyword,
                       nested_duplicate_keyword, nested_dict_missing_colon};
    for (char* source : sources) {
        SCOPED_TRACE(source);
        ByteCodeFrame bytecode_frame = {0};
        byteCodeFrame_init(&bytecode_frame);
        EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source),
                  PIKA_RES_ERR_SYNTAX_ERROR);
        byteCodeFrame_deinit(&bytecode_frame);
    }
}
TEST(parser, semantic_syntax_batch_valid_neighbors) {
    char source[] =
        "def defaults(a=1, b=2):\n"
        "    return a\n"
        "def distinct(a, b):\n"
        "    return a\n"
        "defaults(1, b=2)\n"
        "defaults(a=1, b=2)\n"
        "mapping = {'a': 1}\n";
    ByteCodeFrame bytecode_frame = {0};
    byteCodeFrame_init(&bytecode_frame);
    EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source), PIKA_RES_OK);
    byteCodeFrame_deinit(&bytecode_frame);
}
TEST(parser, python3_invalid_syntax_batch_r10) {
    char literal_target[] = "1 = x\n";
    char call_target[] = "f() = 1\n";
    char operator_target[] = "a + b = 1\n";
    char none_target[] = "None = 1\n";
    char tuple_multiple_star[] = "(a, *b, *c) = values\n";
    char list_multiple_star[] = "[a, *b, *c] = values\n";
    char for_missing_in[] = "for x []:\n    pass\n";
    char for_missing_iterable[] = "for x in:\n    pass\n";
    char if_duplicate_colon[] = "if True::\n    pass\n";
    char while_duplicate_colon[] = "while True::\n    pass\n";
    char dict_missing_comma[] = "x = {'a': 1 'b': 2}\n";
    char comprehension_missing_target[] = "x = [i for in values]\n";
    char comprehension_missing_expr[] = "x = [for i in values]\n";
    char comprehension_missing_in[] = "x = [i for i values]\n";
    char comprehension_missing_iterable[] = "x = [i for i in]\n";
    char* sources[] = {
        literal_target,
        call_target,
        operator_target,
        none_target,
        tuple_multiple_star,
        list_multiple_star,
        for_missing_in,
        for_missing_iterable,
        if_duplicate_colon,
        while_duplicate_colon,
        dict_missing_comma,
        comprehension_missing_target,
        comprehension_missing_expr,
        comprehension_missing_in,
        comprehension_missing_iterable,
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
TEST(parser, empty_import_target_returns_syntax_error) {
    char empty_import[] = "import \n";
    char empty_from_import[] = "from module import \n";
    char* sources[] = {empty_import, empty_from_import};
    for (char* source : sources) {
        SCOPED_TRACE(source);
        ByteCodeFrame bytecode_frame = {0};
        byteCodeFrame_init(&bytecode_frame);
        EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source),
                  PIKA_RES_ERR_SYNTAX_ERROR);
        byteCodeFrame_deinit(&bytecode_frame);
    }
}
TEST(parser, empty_import_target_valid_neighbors) {
    char source[] =
        "import module\n"
        "import module as alias\n"
        "from module import name\n"
        "from module import name as alias2\n";
    ByteCodeFrame bytecode_frame = {0};
    byteCodeFrame_init(&bytecode_frame);
    EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source), PIKA_RES_OK);
    byteCodeFrame_deinit(&bytecode_frame);
}
TEST(parser, invalid_block_context_returns_syntax_error) {
    char orphan_else[] = "else:\n    pass\n";
    char orphan_elif[] = "elif True:\n    pass\n";
    char orphan_except[] = "except:\n    pass\n";
    char orphan_finally[] = "finally:\n    pass\n";
    char outside_break[] = "break\n";
    char outside_continue[] = "continue\n";
    char outside_return[] = "return 1\n";
    char outside_yield[] = "yield 1\n";
    char list_missing_comma[] = "x = [1 2]\n";
    char tuple_missing_comma[] = "x = (1 2)\n";
    char* sources[] = {
        orphan_else,     orphan_elif,      orphan_except,
        orphan_finally,  outside_break,    outside_continue,
        outside_return,  outside_yield,    list_missing_comma,
        tuple_missing_comma,
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
TEST(parser, unsupported_comparison_chain_and_finally) {
    char less_chain[] = "value = 1 < 2 < 3\n";
    char mixed_chain[] = "value = 1 == 1 in [1]\n";
    char try_finally[] = "try:\n    pass\nfinally:\n    pass\n";
    char except_finally[] =
        "try:\n    pass\nexcept:\n    pass\nfinally:\n    pass\n";
    char* sources[] = {less_chain, mixed_chain, try_finally, except_finally};
    for (char* source : sources) {
        SCOPED_TRACE(source);
        ByteCodeFrame bytecode_frame = {0};
        byteCodeFrame_init(&bytecode_frame);
        EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source),
                  PIKA_RES_ERR_SYNTAX_ERROR);
        byteCodeFrame_deinit(&bytecode_frame);
    }
}
TEST(parser, comparison_and_exception_valid_neighbors) {
    char source[] =
        "less = 1 < 2\n"
        "nested = (1 < 2) == True\n"
        "member = 1 in [1]\n"
        "both = 1 < 2 and 2 < 3\n"
        "either = 3 < 2 or 2 < 3\n"
        "try:\n"
        "    raise\n"
        "except:\n"
        "    handled = True\n";
    ByteCodeFrame bytecode_frame = {0};
    byteCodeFrame_init(&bytecode_frame);
    EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source), PIKA_RES_OK);
    byteCodeFrame_deinit(&bytecode_frame);
}

TEST(parser, typed_except_invalid_forms) {
    char invalid_try[] = "try value:\n    pass\nexcept:\n    pass\n";
    char missing_type[] = "try:\n    pass\nexcept as err:\n    pass\n";
    char missing_alias[] =
        "try:\n    pass\nexcept ValueError as:\n    pass\n";
    char tuple_type[] =
        "try:\n    pass\nexcept (TypeError, ValueError):\n    pass\n";
    char except_after_bare[] =
        "try:\n    pass\nexcept:\n    pass\nexcept ValueError:\n    pass\n";
    char duplicate_as[] =
        "try:\n    pass\nexcept ValueError as first as second:\n    pass\n";
    char* sources[] = {invalid_try,       missing_type, missing_alias,
                       tuple_type,        except_after_bare,
                       duplicate_as};
    for (char* source : sources) {
        SCOPED_TRACE(source);
        ByteCodeFrame bytecode_frame = {0};
        byteCodeFrame_init(&bytecode_frame);
        EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source),
                  PIKA_RES_ERR_SYNTAX_ERROR);
        byteCodeFrame_deinit(&bytecode_frame);
    }
}

TEST(parser, typed_except_valid_neighbors) {
    char source[] =
        "try:\n"
        "    raise ValueError\n"
        "except TypeError:\n"
        "    result = 1\n"
        "except ValueError as err:\n"
        "    result = isinstance(err, ValueError)\n"
        "except:\n"
        "    result = 3\n";
    ByteCodeFrame bytecode_frame = {0};
    byteCodeFrame_init(&bytecode_frame);
    EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source), PIKA_RES_OK);
    byteCodeFrame_deinit(&bytecode_frame);
}
TEST(parser, valid_block_context_neighbors) {
    char source[] =
        "def f(value):\n"
        "    while value:\n"
        "        if value == 1:\n"
        "            break\n"
        "        elif value == 2:\n"
        "            continue\n"
        "        else:\n"
        "            return value\n"
        "    try:\n"
        "        raise\n"
        "    except:\n"
        "        return 0\n"
        "for item in [1, 2]:\n"
        "    pass\n"
        "else:\n"
        "    pass\n";
    ByteCodeFrame bytecode_frame = {0};
    byteCodeFrame_init(&bytecode_frame);
    EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source), PIKA_RES_OK);
    byteCodeFrame_deinit(&bytecode_frame);
}
TEST(parser, invalid_declaration_and_target_returns_syntax_error) {
    char module_nonlocal[] = "nonlocal value\n";
    char empty_assert[] = "assert\n";
    char literal_del[] = "del 1\n";
    char invalid_def_name[] = "def 1():\n    pass\n";
    char invalid_class_name[] = "class 1:\n    pass\n";
    char invalid_for_target[] = "for 1 in [1]:\n    pass\n";
    char param_after_kwargs[] = "def f(**kwargs, value):\n    pass\n";
    char duplicate_varargs[] = "def f(*args, *more):\n    pass\n";
    char tuple_augmented[] = "a, b += (1, 2)\n";
    char literal_annotation[] = "1: int\n";
    char* sources[] = {
        module_nonlocal,   empty_assert,       literal_del,
        invalid_def_name,  invalid_class_name, invalid_for_target,
        param_after_kwargs, duplicate_varargs, tuple_augmented,
        literal_annotation,
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
TEST(parser, valid_declaration_and_target_neighbors) {
    char source[] =
        "class Device:\n"
        "    pass\n"
        "def f(value, *args, **kwargs):\n"
        "    assert value\n"
        "    local: int = value\n"
        "    del local\n"
        "for item in [1, 2]:\n"
        "    pass\n"
        "value += 1\n";
    ByteCodeFrame bytecode_frame = {0};
    byteCodeFrame_init(&bytecode_frame);
    EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source), PIKA_RES_OK);
    byteCodeFrame_deinit(&bytecode_frame);
}
TEST(parser, invalid_expression_form_returns_syntax_error) {
    char invalid_lambda[] = "x = lambda:\n";
    char invalid_conditional[] = "x = 1 if else 2\n";
    char unsupported_slice_step[] = "x = values[0:2:1]\n";
    char unsupported_negative_slice_step[] = "x = values[::-1]\n";
    char excessive_slice[] = "x = values[1:2:3:4]\n";
    char outside_await[] = "await f()\n";
    char async_for[] = "async for item in []:\n    pass\n";
    char* sources[] = {invalid_lambda,
                       invalid_conditional,
                       unsupported_slice_step,
                       unsupported_negative_slice_step,
                       excessive_slice,
                       outside_await,
                       async_for};
    for (char* source : sources) {
        SCOPED_TRACE(source);
        ByteCodeFrame bytecode_frame = {0};
        byteCodeFrame_init(&bytecode_frame);
        EXPECT_EQ(pika_lines2Bytes(&bytecode_frame, source),
                  PIKA_RES_ERR_SYNTAX_ERROR);
        byteCodeFrame_deinit(&bytecode_frame);
    }
}
TEST(parser, invalid_expression_form_valid_neighbors) {
    char source[] =
        "text = 'lambda value if else await async for'\n"
        "await_result = 1\n"
        "values = [1, 2, 3]\n"
        "part = values[0:2]\n"
        "for item in values:\n"
        "    pass\n";
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
