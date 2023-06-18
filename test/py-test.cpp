#include "test_common.h"
TEST_START

#if !PIKA_NANO_ENABLE
TEST_RUN_SINGLE_FILE(thread, test2, "test/python/_thread/test2.py")
TEST_RUN_SINGLE_FILE(eventloop, test1, "test/python/eventloop/test1.py")
TEST_RUN_SINGLE_FILE(eventloop, test2, "test/python/eventloop/test2.py")
TEST_RUN_SINGLE_FILE(eventloop, test3, "test/python/eventloop/test3.py")
TEST_RUN_SINGLE_FILE(eventloop, delay1, "test/python/eventloop/delay1.py")
TEST_RUN_SINGLE_FILE(eventloop, once1, "test/python/eventloop/once1.py")
TEST_RUN_SINGLE_FILE(eventloop, once2, "test/python/eventloop/once2.py")
TEST_RUN_SINGLE_FILE(fsm, test1, "test/python/fsm/test1.py")

TEST_RUN_SINGLE_FILE_PASS(builtin,
                          list_tuple_equ,
                          "test/python/builtin/list_tuple_equ.py")
TEST_RUN_SINGLE_FILE_PASS(builtin,
                          fn_default1,
                          "test/python/builtin/fn_default1.py")
TEST_RUN_SINGLE_FILE_PASS(builtin,
                          fn_default_tuple,
                          "test/python/builtin/fn_default_tuple.py")
TEST_RUN_SINGLE_FILE_PASS(builtin, max_min, "test/python/builtin/max_min.py")
TEST_RUN_SINGLE_FILE_PASS(builtin, split, "test/python/builtin/split.py")
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
                                   "test/python/builtin/class_script.py",
                                   "Obj1.test\r\n")
TEST_RUN_SINGLE_FILE_EXCEPT_OUTPUT(builtin,
                                   class_hint,
                                   "test/python/builtin/class_hint.py",
                                   "1\r\n")
TEST_RUN_SINGLE_FILE_PASS(builtin,
                          isinstance,
                          "test/python/builtin/isinstance.py")
TEST_RUN_SINGLE_FILE_PASS(builtin, getitem, "test/python/builtin/getitem.py")
TEST_RUN_SINGLE_FILE_PASS(lua, eval, "test/python/pika_lua/eval.py")
TEST_RUN_SINGLE_FILE_PASS(lua, require, "test/python/pika_lua/require.py")

TEST_RUN_LINES(vm, dot_issue, ".")
TEST_RUN_LINES(vm, char_issue1, "~")
TEST_RUN_LINES(vm, char_issue2, "/")
TEST_RUN_LINES(vm, char_issue3, "%")
TEST_RUN_LINES(vm, char_issue4, "=")
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
TEST_RUN_SINGLE_FILE_PASS(vm, is_not, "test/python/builtin/is_not.py")
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
TEST_RUN_SINGLE_FILE_PASS(vm, fn_fn, "test/python/builtin/fn_fn.py")
TEST_RUN_LINES_EXCEPT_OUTPUT(vm, isinstance, "isinstance(1, int)\n", "True\r\n")
TEST_RUN_SINGLE_FILE_PASS(except,
                          try_while_return,
                          "test/python/except/try_while_return.py");
TEST_RUN_SINGLE_FILE_PASS(except,
                          isinstance,
                          "test/python/except/except_isinstance.py");

#endif

TEST_END
