#include "test_common.h"
TEST_START
#if !PIKA_NANO_ENABLE

TEST_RUN_SINGLE_FILE(lua, eval, "test/python/pika_lua/eval.py")

#endif
TEST_END
