#include "test_common.h"
TEST_START
#if PIKA_SYNTAX_LEVEL == PIKA_SYNTAX_LEVEL_MAXIMAL
TEST(PikaMath, test1) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    __platform_printf("BEGIN\r\n");
    pikaVM_runSingleFile(pikaMain, "test/python/PikaMath/Quaternion_test.py");
    /* collect */
    /* assert */
    /* deinit */
    obj_deinit(pikaMain);

    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(PikaMath, quat_add) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "import PikaMath\n"
            "a = PikaMath.Quaternion()\n"
            "b = PikaMath.Quaternion()\n"
            "a.add(b)\n"
            "res1=a.get(0)\n"
            "res2=a.get(1)\n"
            "res3=a.get(2)\n"
            "res4=a.get(3)\n");
    /* collect */
    float res1 = obj_getFloat(pikaMain, "res1");
    float res2 = obj_getFloat(pikaMain, "res2");
    float res3 = obj_getFloat(pikaMain, "res3");
    float res4 = obj_getFloat(pikaMain, "res4");

    /* assert */
    EXPECT_FLOAT_EQ(res1, 0.0);
    EXPECT_FLOAT_EQ(res2, 0.0);
    EXPECT_FLOAT_EQ(res3, 0.0);
    EXPECT_FLOAT_EQ(res4, 2.0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(PikaMath, quat_sub) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "import PikaMath\n"
            "a = PikaMath.Quaternion()\n"
            "b = PikaMath.Quaternion()\n"
            "a.sub(b)\n"
            "res1=a.get(0)\n"
            "res2=a.get(1)\n"
            "res3=a.get(2)\n"
            "res4=a.get(3)\n");
    /* collect */
    float res1 = obj_getFloat(pikaMain, "res1");
    float res2 = obj_getFloat(pikaMain, "res2");
    float res3 = obj_getFloat(pikaMain, "res3");
    float res4 = obj_getFloat(pikaMain, "res4");

    /* assert */
    EXPECT_FLOAT_EQ(res1, 0.0);
    EXPECT_FLOAT_EQ(res2, 0.0);
    EXPECT_FLOAT_EQ(res3, 0.0);
    EXPECT_FLOAT_EQ(res4, 0.0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(PikaMath, quat_mul) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "import PikaMath\n"
            "a = PikaMath.Quaternion()\n"
            "b = PikaMath.Quaternion()\n"
            "a.mul(b)\n"
            "res1=a.get(0)\n"
            "res2=a.get(1)\n"
            "res3=a.get(2)\n"
            "res4=a.get(3)\n");
    /* collect */
    float res1 = obj_getFloat(pikaMain, "res1");
    float res2 = obj_getFloat(pikaMain, "res2");
    float res3 = obj_getFloat(pikaMain, "res3");
    float res4 = obj_getFloat(pikaMain, "res4");

    /* assert */
    EXPECT_FLOAT_EQ(res1, 0.0);
    EXPECT_FLOAT_EQ(res2, 0.0);
    EXPECT_FLOAT_EQ(res3, 0.0);
    EXPECT_FLOAT_EQ(res4, 1.0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(PikaMath, quat_set) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "import PikaMath\n"
            "a = PikaMath.Quaternion()\n"
            "a.set(1,1,1,1)\n"
            "res1=a.get(0)\n"
            "res2=a.get(1)\n"
            "res3=a.get(2)\n"
            "res4=a.get(3)\n");
    /* collect */
    float res1 = obj_getFloat(pikaMain, "res1");
    float res2 = obj_getFloat(pikaMain, "res2");
    float res3 = obj_getFloat(pikaMain, "res3");
    float res4 = obj_getFloat(pikaMain, "res4");

    /* assert */
    EXPECT_FLOAT_EQ(res1, 1);
    EXPECT_FLOAT_EQ(res2, 1);
    EXPECT_FLOAT_EQ(res3, 1);
    EXPECT_FLOAT_EQ(res4, 1);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(PikaMath, quat_dot) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "import PikaMath\n"
            "a = PikaMath.Quaternion()\n"
            "b = PikaMath.Quaternion()\n"
            "a.set(1,2,3,4)\n"
            "b.set(5,6,7,8)\n"
            "res1=a.dot(b)\n");
    /* collect */
    float res1 = obj_getFloat(pikaMain, "res1");


    /* assert */
    EXPECT_FLOAT_EQ(res1, 70.0);


    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(PikaMath, quat_inv) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "import PikaMath\n"
            "a = PikaMath.Quaternion()\n"
            "a.inverse()\n"
            "res1=a.get(0)\n"
            "res2=a.get(1)\n"
            "res3=a.get(2)\n"
            "res4=a.get(3)\n");
    /* collect */
    float res1 = obj_getFloat(pikaMain, "res1");
    float res2 = obj_getFloat(pikaMain, "res2");
    float res3 = obj_getFloat(pikaMain, "res3");
    float res4 = obj_getFloat(pikaMain, "res4");

    /* assert */
    EXPECT_FLOAT_EQ(res1, 0.0);
    EXPECT_FLOAT_EQ(res2, 0.0);
    EXPECT_FLOAT_EQ(res3, 0.0);
    EXPECT_FLOAT_EQ(res4, 1.0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(PikaMath, quat_cross) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "import PikaMath\n"
            "a = PikaMath.Quaternion()\n"
            "b = PikaMath.Quaternion()\n"
            "a.set(2,3,4,1)\n"
            "b.set(6,7,8,5)\n"
            "a.crossproduct(b)\n"
            "res1=a.get(0)\n"
            "res2=a.get(1)\n"
            "res3=a.get(2)\n"
            "res4=a.get(3)\n");
    /* collect */
    float res1 = obj_getFloat(pikaMain, "res1");
    float res2 = obj_getFloat(pikaMain, "res2");
    float res3 = obj_getFloat(pikaMain, "res3");
    float res4 = obj_getFloat(pikaMain, "res4");

    /* assert */
    EXPECT_FLOAT_EQ(res1, -4.0);
    EXPECT_FLOAT_EQ(res2, 8.0);
    EXPECT_FLOAT_EQ(res3, -4.0);
    EXPECT_FLOAT_EQ(res4, 0.0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}


TEST(PikaMath, quat_toEuler) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "import PikaMath\n"
            "a = PikaMath.Quaternion()\n"
            "a.set(0,0,1,0)\n"
            "res=a.toEuler()\n"
            "res1=res[0]\n"
            "res2=res[1]\n"
            "res3=res[2]\n");
    /* collect */
    float res1 = obj_getFloat(pikaMain, "res1");
    float res2 = obj_getFloat(pikaMain, "res2");
    float res3 = obj_getFloat(pikaMain, "res3");

    /* assert */
    EXPECT_FLOAT_EQ(res1, 0.0);
    EXPECT_FLOAT_EQ(res2, 0.0);
    EXPECT_FLOAT_EQ(res3, 3.1415927);

    /* deinit */
    obj_deinit(pikaMain);
 
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(PikaMath, quat_fromEuler) {
    /* init */
    g_PikaMemInfo.heapUsedMax = 0;
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain,
            "import PikaMath\n"
            "a = PikaMath.Quaternion()\n"
            "a.fromEuler(0,0.0,0.0,3.1415927)\n"
            "res1=a.get(0)\n"
            "res2=a.get(1)\n"
            "res3=a.get(2)\n"
            "res4=a.get(3)\n");
    /* collect */
    float res1 = obj_getFloat(pikaMain, "res1");
    float res2 = obj_getFloat(pikaMain, "res2");
    float res3 = obj_getFloat(pikaMain, "res3");
    float res4 = obj_getFloat(pikaMain, "res4");

    /* assert */
    EXPECT_FLOAT_EQ(res1, 0.0);
    EXPECT_FLOAT_EQ(res2, 0.0);
    EXPECT_FLOAT_EQ(res3, 0.0);
    EXPECT_FLOAT_EQ(res4, 1.0);

    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

#endif
TEST_END