#include "test_common.h"

TEST(chinese, test1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    pikaVM_runSingleFile(pikaMain, "../../examples/Chinese/test_operator.py");
    obj_run(pikaMain, "test()");
    /* collect */
    /* assert */
    EXPECT_STREQ(log_buff[0],
                 "1 - a \344\270\215\347\255\211\344\272\216 b\r\n");
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
