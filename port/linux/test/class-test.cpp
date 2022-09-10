#include "test_common.h"

#if !PIKA_NANO_ENABLE
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

TEST(class, classpar1) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(self, "../../examples/Class/classpar1.py");
    /* assert */
    EXPECT_STREQ(log_buff[4], "BEGIN\r\n");
    EXPECT_STREQ(log_buff[3], "1\r\n");
    EXPECT_STREQ(log_buff[2], "test\r\n");
    EXPECT_STREQ(log_buff[1], "2\r\n");
    EXPECT_STREQ(log_buff[0], "pewq\r\n");
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(class, static_method) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "import PikaMath\n"
            "a = PikaMath.Operator.plusInt(10, 20)\n");
    /* collect */
    int a = obj_getInt(self, "a");
    /* assert */
    EXPECT_EQ(a, 30);
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}
#endif

TEST(class, dir_) {
    /* init */
    pikaMemInfo.heapUsedMax = 0;
    /* run */
    PikaObj* self = newRootObj("pikaMain", New_PikaMain);
    __platform_printf("BEGIN\r\n");
    obj_run(self,
            "class T:\n"
            "    def a(self):\n"
            "        return 1\n"
            "t = T()\n"
            "l = dir(t)\n"
            "print(l)\n");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}