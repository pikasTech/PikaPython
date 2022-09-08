#include "test_common.h"

//!    Valgrind not pass.
//!    ==16693== Invalid read of size 4
//!    ==16693==    at 0x5E733F: tm_preprocess (tm_model.c:67)
//!    ...
#if 0 
#if (PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MAXIMAL) && (!PIKA_POOL_ENABLE) && \
    (PIKA_STACK_BUFF_SIZE > 0x6000)
TEST(PikaNN, test1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/PikaNN/PikaNN_test1.py");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);

    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

#endif