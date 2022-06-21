#include "test_common.h"

TEST(class, class_par) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self, "pi = PikaMath.Math.pi\n");
    /* assert */
    double pi = obj_getFloat(self, "pi");
    EXPECT_DOUBLE_EQ(pi, 3.141592653589793115997963468544185161590576171875l);
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
