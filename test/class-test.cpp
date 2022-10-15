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
    pika_float pi = obj_getFloat(self, "pi");
#if PIKA_FLOAT_TYPE_DOUBLE
    EXPECT_DOUBLE_EQ(pi, 3.141592653589793115997963468544185161590576171875l);
#else
    EXPECT_FLOAT_EQ(pi, 3.141592653589793115997963468544185161590576171875l);
#endif
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

extern "C" {

void PikaStdLib_SysObj_intMethod(PikaObj* self, Args* args);
void PikaStdLib_SysObj_floatMethod(PikaObj* self, Args* args);
void PikaStdLib_SysObj_printMethod(PikaObj* self, Args* args);

method_typedef(PikaStdLib_SysObj_float, "float", "arg");
method_typedef(PikaStdLib_SysObj_int, "int", "arg");
method_typedef(PikaStdLib_SysObj_print, "print", "*val,**ops");
class_def(PikaStdLib_SysObj){
    method_def(PikaStdLib_SysObj_float, hash_time33("float")),
    method_def(PikaStdLib_SysObj_int, hash_time33("int")),
    method_def(PikaStdLib_SysObj_print, hash_time33("print")),
};
class_inhert(PikaStdLib_SysObj, TinyObj);
PikaObj* New_NativeMethodBase(Args* args) {
    PikaObj* self = New_TinyObj(NULL);
    obj_setClass(self, PikaStdLib_SysObj);
    return self;
}
}

TEST(class, native_class1) {
    PikaObj* native_obj = newNormalObj(New_NativeMethodBase);
    obj_run(native_obj,
            "print('hello', 123)\n"
            "int(123.0000)\n"
            "float(123)\n");
    obj_deinit(native_obj);
    EXPECT_STREQ(log_buff[1], "123\r\n");
    EXPECT_STREQ(log_buff[0], "123.000000\r\n");
    EXPECT_EQ(pikaMemNow(), 0);
}
