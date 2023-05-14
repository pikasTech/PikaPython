#include "test_common.h"
TEST_START
#if !PIKA_NANO_ENABLE

TEST_RUN_SINGLE_FILE_PASS(lua, eval, "test/python/pika_lua/eval.py")
TEST_RUN_SINGLE_FILE_PASS(lua, require, "test/python/pika_lua/require.py")

#endif
TEST_END
