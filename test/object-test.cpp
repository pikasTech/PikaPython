#include "test_common.h"

void testFloat(PikaObj* obj, Args* args) {
    float val1 = args_getFloat(args, "val1");
    float val2 = args_getFloat(args, "val2");
    int32_t isShow = args_getInt(args, "isShow");
    if (isShow) {
        printf("the float val1 is: %f\r\n", val1);
        printf("the float val2 is: %f\r\n", val2);
    }
    method_returnFloat(args, val1 + val2);
}

void hello2(PikaObj* obj, Args* args) {
    char* name1 = args_getStr(args, "name1");
    char* name2 = args_getStr(args, "name2");
    char* name3 = args_getStr(args, "name3");
    int32_t isShow = args_getInt(args, "isShow");
    if (isShow) {
        printf("hello, %s, %s and %s!\r\n", name1, name2, name3);
    }
}

void hello(PikaObj* obj, Args* args) {
    char* name = args_getStr(args, "name");
    int32_t isShow = args_getInt(args, "isShow");
    if (isShow) {
        printf("hello, %s!\r\n", name);
    }
}

void add(PikaObj* obj, Args* args) {
    int32_t val1 = args_getInt(args, "val1");
    int32_t val2 = args_getInt(args, "val2");
    method_returnInt(args, val1 + val2);
}

PikaObj* New_PikaObj_test(Args* args) {
    PikaObj* self = New_PikaStdLib_SysObj(args);
    class_defineMethod(self, "hello", "name,isShow", hello);
    class_defineMethod(self, "hello2", "name1,name2,name3,isShow", hello2);
    class_defineMethod(self, "testFloat", "val1,val2,isShow", testFloat);
    class_defineMethod(self, "add", "val1,val2", add);
    return self;
}

void sendMethod(PikaObj* self, Args* args) {
    char* data = args_getStr(args, "data");
    /* send to com1 */
    printf("[com1]: %s\r\n", data);
}

PikaObj* New_USART(Args* args) {
    /*  Derive from the tiny object class.
        Tiny object can not import sub object.
        Tiny object is the smallest object. */
    PikaObj* self = New_TinyObj(args);

    /* bind the method */
    class_defineMethod(self, "send", "data:str", sendMethod);

    /* return the object */
    return self;
}

PikaObj* New_MYROOT1(Args* args) {
    /*  Derive from the base object class .
        BaseObj is the smallest object that can
        import sub object.      */
    PikaObj* self = New_BaseObj(args);

    /* new led object bellow root object */
    obj_newObj(self, "usart", "USART", (NewFun)New_USART);

    /* return the object */
    return self;
}

TEST(object_test, test10) {
    PikaObj* root = newRootObj("root", New_MYROOT1);
    obj_run(root, "usart.send('hello world')");
    obj_deinit(root);
    EXPECT_EQ(pikaMemNow(), 0);
}

// TEST(object_test, test1) {
// PikaObj* process = newRootObj("sys", New_PikaStdLib_SysObj);
// float floatTest = 12.231;
// obj_bindFloat(process, "testFloatBind", &floatTest);
// EXPECT_TRUE(
// strEqu("12.231000", obj_print(process, "testFloatBind")));
// obj_deinit(process);
// EXPECT_EQ(pikaMemNow(), 0);
// }

TEST(object_test, test2) {
    int isShow = 1;
    PikaObj* obj = newRootObj("test", New_PikaObj_test);
    obj_setInt(obj, "isShow", isShow);
    obj_run(obj, "hello(name = 'world', isShow = isShow)");
    obj_deinit(obj);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(object_test, test3) {
    int isShow = 1;
    PikaObj* obj = newRootObj("test", New_PikaObj_test);
    obj_setInt(obj, "isShow", isShow);
    obj_run(obj,
            "hello2(name2='tom', name1='john', name3='cat', isShow=isShow) ");
    obj_deinit(obj);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(object_test, test6) {
    PikaObj* obj = newRootObj("test", New_PikaObj_test);
    VMParameters* globals = obj_runDirect(obj, "res = add(1, 2)");
    int32_t res = obj_getInt(globals, "res");
    EXPECT_EQ(3, res);
    obj_deinit(obj);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(object_test, test8) {
    PikaObj* sys = newRootObj("sys", New_PikaStdLib_SysObj);
    obj_run(sys, "a=1");
    obj_run(sys, "remove('a')");
    obj_deinit(sys);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(object_test, test9) {
    PikaObj* sys = newRootObj("sys", New_PikaStdLib_SysObj);
    obj_run(sys, "ls()");
    obj_setPtr(sys, "baseClass", (void*)New_TinyObj);
    obj_run(sys, "ls()");
    obj_deinit(sys);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(object_test, noMethod) {
    PikaObj* root = newRootObj("root", New_MYROOT1);
    obj_runNoRes(root, "noDefindMethod()");
    obj_deinit(root);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(object_test, a_b) {
    PikaObj* root = newRootObj("root", New_MYROOT1);
    obj_runNoRes(root, "b=1");
    obj_runNoRes(root, "a=b");
    obj_deinit(root);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(object_test, voidRun) {
    PikaObj* root = newRootObj("root", New_MYROOT1);
    obj_run(root, "");
    obj_deinit(root);
    EXPECT_EQ(pikaMemNow(), 0);
}

/* the log_buff of printf */
extern char log_buff[LOG_BUFF_MAX][LOG_SIZE];
TEST(object_test, printa) {
    PikaObj* root = newRootObj("root", New_PikaStdLib_SysObj);
    obj_runDirect(root,

                  "a = 2\n"
                  "print(a)\n");
    // char* sysOut = obj_getSysOut(globals);
    EXPECT_STREQ(log_buff[0], "2\r\n");
    // ASSERT_STREQ(sysOut, "2");
    // obj_deinit(globals);
    obj_deinit(root);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(object_test, copyArg) {
    PikaObj* root = newRootObj("root", New_BaseObj);
    Arg* arg = New_arg(NULL);
    arg = arg_setInt(arg, "a", 1);
    obj_setArg(root, "a", arg);
    arg_deinit(arg);
    Arg* argOut = obj_getArg(root, "a");
    int argOutInt = arg_getInt(argOut);
    ASSERT_EQ(argOutInt, 1);
    obj_deinit(root);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(object_test, obj_run_while) {
    PikaObj* root = newRootObj("root", New_BaseObj);
    char lines[] =
        "a = 1\n"
        "b = 0\n"
        "while a:\n"
        "    b = 1\n"
        "    a = 0\n"
        "\n";
    VMParameters* globals = obj_runDirect(root, lines);
    EXPECT_EQ(obj_getInt(globals, "a"), 0);
    EXPECT_EQ(obj_getInt(globals, "b"), 1);
    obj_deinit(root);
    // obj_deinit(globals);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(object_test, obj_mem) {
    uint8_t mem_test[] = {0x33, 0x55, 0x00, 0x15};
    uint8_t mem_out_buff[32] = {0};
    PikaObj* self = New_TinyObj(NULL);
    obj_setBytes(self, "mem", mem_test, sizeof(mem_test));
    size_t mem_size = obj_getBytesSize(self, "mem");
    char* mem_test_out = (char*)obj_getBytes(self, "mem");
    ArgType arg_type = arg_getType(obj_getArg(self, "mem"));
    obj_loadBytes(self, "mem", mem_out_buff);
    /* assert */
    EXPECT_EQ(mem_size, sizeof(mem_test));
    EXPECT_EQ(mem_test_out[0], 0x33);
    EXPECT_EQ(mem_test_out[1], 0x55);
    EXPECT_EQ(mem_test_out[2], 0x00);
    EXPECT_EQ(mem_test_out[3], 0x15);
    EXPECT_EQ(arg_type, ARG_TYPE_BYTES);
    /* deinit */
    obj_deinit(self);

    EXPECT_EQ(mem_out_buff[0], 0x33);
    EXPECT_EQ(mem_out_buff[1], 0x55);
    EXPECT_EQ(mem_out_buff[2], 0x00);
    EXPECT_EQ(mem_out_buff[3], 0x15);

    EXPECT_EQ(pikaMemNow(), 0);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(object_test, mem) {
    EXPECT_EQ(pikaMemNow(), 0);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(object_test, bytes) {
    PikaObj* root = newRootObj("root", New_BaseObj);
    uint8_t test_arg[] = {0x00, 0x02, 0x03, 0x05, 0x07};
    obj_setBytes(root, "test", test_arg, sizeof(test_arg));
    uint16_t mem_now_before = pikaMemNow();
    obj_setBytes(root, "test", test_arg, sizeof(test_arg));
    EXPECT_EQ(pikaMemNow(), mem_now_before);
    obj_deinit(root);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(object_test, bytes_0_size) {
    PikaObj* root = newRootObj("root", New_BaseObj);
    uint8_t test[] = {0, 1, 2, 3, 4, 5};
    obj_setBytes(root, "test", test, 0);
    obj_deinit(root);
    EXPECT_EQ(pikaMemNow(), 0);
}
