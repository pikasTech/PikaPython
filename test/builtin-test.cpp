#include "test_common.h"
TEST_START

#if PIKA_SYNTAX_SLICE_ENABLE
TEST(builtin, bytes) {
  /* init */
  g_PikaMemInfo.heapUsedMax = 0;
  PikaObj *pikaMain = newRootObj("pikaMain", New_PikaMain);
  /* run */
  __platform_printf("BEGIN\r\n");
  pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/bytes.py");
  /* collect */
  uint8_t *res = obj_getBytes(pikaMain, "res");
  /* assert */
  EXPECT_EQ(res[1], 1);
  /* deinit */
  obj_deinit(pikaMain);
  EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_BUILTIN_STRUCT_ENABLE
TEST(builtin, type1) {
  /* init */
  g_PikaMemInfo.heapUsedMax = 0;
  PikaObj *pikaMain = newRootObj("pikaMain", New_PikaMain);
  /* run */
  __platform_printf("BEGIN\r\n");
  pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/type.py");
  /* collect */
  /* assert */
  EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
  EXPECT_STREQ(log_buff[0], "OK\r\n");
  /* deinit */
  obj_deinit(pikaMain);
  EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MAXIMAL
TEST(builtin, seek) {
  /* init */
  g_PikaMemInfo.heapUsedMax = 0;
  PikaObj *pikaMain = newRootObj("pikaMain", New_PikaMain);
  /* run */
  __platform_printf("BEGIN\r\n");
  pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/seek.py");
  /* collect */
  int len = obj_getInt(pikaMain, "len");
  /* assert */
  EXPECT_EQ(len, 3576);
  /* deinit */
  obj_deinit(pikaMain);
  EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if (PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MAXIMAL)
TEST(builtin, file2) {
  /* init */
  g_PikaMemInfo.heapUsedMax = 0;
  PikaObj *pikaMain = newRootObj("pikaMain", New_PikaMain);
  /* run */
  __platform_printf("BEGIN\r\n");
  pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/file2.py");
  /* collect */
  char *s = obj_getStr(pikaMain, "s");
  /* assert */
  EXPECT_STREQ(s, "import PikaStdLib\n");
  /* deinit */
  obj_deinit(pikaMain);
  EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MAXIMAL
TEST(builtin, file3) {
  /* init */
  g_PikaMemInfo.heapUsedMax = 0;
  PikaObj *pikaMain = newRootObj("pikaMain", New_PikaMain);
  /* run */
  __platform_printf("BEGIN\r\n");
  pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/file3.py");
  /* collect */
  /* assert */
  /* deinit */
  obj_deinit(pikaMain);
  EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if (PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MAXIMAL) && !PIKA_POOL_ENABLE
TEST(builtin, bigfile) {
  /* init */
  g_PikaMemInfo.heapUsedMax = 0;
  PikaObj *pikaMain = newRootObj("pikaMain", New_PikaMain);
  /* run */
  __platform_printf("BEGIN\r\n");
  pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/bigfile.py");
  /* collect */
  // uint8_t* b = obj_getBytes(pikaMain, "b");
  /* assert */
  /* deinit */
  obj_deinit(pikaMain);
  EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(builtin, nofound) {
  /* init */
  g_PikaMemInfo.heapUsedMax = 0;
  PikaObj *pikaMain = newRootObj("pikaMain", New_PikaMain);
  /* run */
  __platform_printf("BEGIN\r\n");
  pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/file_nofound.py");
  /* collect */
  /* assert */
  /* deinit */
  obj_deinit(pikaMain);
  EXPECT_EQ(pikaMemNow(), 0);
}

#if PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MAXIMAL
TEST(builtin, callback_1) {
  /* init */
  g_PikaMemInfo.heapUsedMax = 0;
  PikaObj *pikaMain = newRootObj("pikaMain", New_PikaMain);
  /* run */
  __platform_printf("BEGIN\r\n");
  pikaVM_runSingleFile(pikaMain, "test/python/callback/test1.py");
  /* collect */
  /* assert */
  EXPECT_STREQ(log_buff[0], "b\r\n");
  EXPECT_STREQ(log_buff[1], "a\r\n");
  /* deinit */
  obj_deinit(pikaMain);
  EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#if !PIKA_NANO_ENABLE
TEST(builtin, utf8) {
  /* init */
  g_PikaMemInfo.heapUsedMax = 0;
  PikaObj *pikaMain = newRootObj("pikaMain", New_PikaMain);
  /* run */
  __platform_printf("BEGIN\r\n");
  pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/utf8.py");
  /* collect */
  /* assert */
  EXPECT_STREQ(log_buff[2], "你好,Hello, Bonjour.\r\n");
  EXPECT_STREQ(log_buff[1], "你好，Hello, Bonjour.\r\n");
  EXPECT_STREQ(log_buff[0], "Hi, Hello, Bonjour.\r\n");
  /* deinit */
  obj_deinit(pikaMain);
  EXPECT_EQ(pikaMemNow(), 0);
}

TEST(builtin, str_join) {
  /* init */
  g_PikaMemInfo.heapUsedMax = 0;
  PikaObj *pikaMain = newRootObj("pikaMain", New_PikaMain);
  /* run */
  __platform_printf("BEGIN\r\n");
  pikaVM_runSingleFile(pikaMain, "test/python/builtin/str_join.py");
  /* collect */
  /* assert */
  EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
  EXPECT_STREQ(log_buff[0], "PASS\r\n");
  /* deinit */
  obj_deinit(pikaMain);
  EXPECT_EQ(pikaMemNow(), 0);
}

TEST(builtin, int_extern) {
  /* init */
  g_PikaMemInfo.heapUsedMax = 0;
  PikaObj *pikaMain = newRootObj("pikaMain", New_PikaMain);
  /* run */
  __platform_printf("BEGIN\r\n");
  pikaVM_runSingleFile(pikaMain, "test/python/builtin/int_extern.py");
  /* collect */
  /* assert */
  EXPECT_STREQ(log_buff[1], "BEGIN\r\n");
  EXPECT_STREQ(log_buff[0], "PASS\r\n");
  /* deinit */
  obj_deinit(pikaMain);
  EXPECT_EQ(pikaMemNow(), 0);
}

TEST_RUN_SINGLE_FILE_PASS(builtin, list_tuple_equ,
                          "test/python/builtin/list_tuple_equ.py")

TEST_RUN_SINGLE_FILE_PASS(builtin, fn_default1,
                          "test/python/builtin/fn_default1.py")

TEST_RUN_SINGLE_FILE_PASS(builtin, fn_default_tuple,
                          "test/python/builtin/fn_default_tuple.py")

TEST_RUN_SINGLE_FILE_PASS(builtin, max_min, "test/python/builtin/max_min.py")

TEST_RUN_SINGLE_FILE_PASS(builtin, split, "test/python/builtin/split.py")

TEST_RUN_LINES_EXCEPT_OUTPUT(builtin, split_slice, "'test'.split('e')[0]",
                             "'t'\r\n")

TEST_RUN_LINES_EXCEPT_OUTPUT(builtin, split_slice_1, "'test'.split('e')[1]",
                             "'st'\r\n")

TEST_RUN_LINES_EXCEPT_OUTPUT(builtin, replace_split_0,
                             "'a b c d'.replace(' ', ',').split(',')[0]",
                             "'a'\r\n")

TEST_RUN_SINGLE_FILE_EXCEPT_OUTPUT(builtin, class_script,
                                   "test/python/builtin/class_script.py",
                                   "Obj1.test\r\n")

#endif

TEST_END