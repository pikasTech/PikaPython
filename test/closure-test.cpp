#include "test_common.h"
TEST_START
#if !PIKA_NANO_ENABLE

TEST(closure, defdef){
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    pikaVM_runSingleFile(pikaMain, "test/python/closure/defdef.py");
    obj_deinit(pikaMain);
    EXPECT_STREQ(log_buff[0], "in test2\r\n");
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(closure, defvardef){
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    pikaVM_runSingleFile(pikaMain, "test/python/closure/defvardef.py");
    obj_deinit(pikaMain);
    EXPECT_STREQ(log_buff[0], "in test2 var: 1\r\n");
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif

TEST_END