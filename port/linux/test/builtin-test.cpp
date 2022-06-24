#include "test_common.h"

#if PIKA_SYNTEX_ITEM_SLICE_ENABLE
TEST(builtin, bytes) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "../../examples/BuiltIn/bytes.py");
    /* collect */
    uint8_t* res = obj_getBytes(pikaMain, "res");
    /* assert */
    EXPECT_EQ(res[1], 1);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif
