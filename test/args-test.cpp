#include "test_common.h"
TEST_START

static int mem;
TEST(args, test1) {
    mem = pikaMemNow();
    Args* args = New_args(NULL);
    args_setInt(args, "a", 1);
    int a = args_getInt(args, "a");
    EXPECT_EQ(a, 1);
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, test4) {
    int32_t testint = 12333;
    Args* args = New_args(NULL);
    args_setInt(args, "testint", testint);
    EXPECT_EQ(12333, args_getInt(args, "testint"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, test2) {
    Args* args = New_args(NULL);
    int64_t int64Out = 0;
    void* pointer = NULL;
    char* strOut = NULL;
    args_setInt(args, "int64Test", (int64_t)22221);
    args_setPtr(args, "pointerTest", (void*)2222322);
    args_setStr(args, "strTest", "teeeds");

    int64Out = args_getInt(args, "int64Test");
    pointer = args_getPtr(args, "pointerTest");
    strOut = args_getStr(args, "strTest");

    EXPECT_EQ(int64Out, 22221);
    EXPECT_EQ((uint64_t)pointer, 2222322);
    EXPECT_EQ(1, strEqu("teeeds", strOut));
    EXPECT_EQ(args_getType(args, "int64Test"), ARG_TYPE_INT);
    EXPECT_EQ(args_getType(args, "pointerTest"), ARG_TYPE_POINTER);
    EXPECT_EQ(args_getType(args, "strTest"), ARG_TYPE_STRING);
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, test3) {
    Args* args1 = New_args(NULL);
    Args* args2 = New_args(NULL);

    args_setFloat(args1, "argtest1", 2.883);
    args_setStr(args1, "strtest1", "teststr");
    args_copyArgByName(args1, "argtest1", args2);
    args_copyArgByName(args1, "strtest1", args2);

    EXPECT_FLOAT_EQ(2.8830, args_getFloat(args1, "argtest1"));
    args_deinit(args1);
    args_deinit(args2);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, buff) {
    Args* buffs = New_args(NULL);
    args_getBuff(buffs, 256);
    args_deinit(buffs);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, update) {
    Args* args = New_args(NULL);
    args_setInt(args, "testInt", 124);
    args_setInt(args, "testInt", 125);
    EXPECT_EQ(125, args_getInt(args, "testInt"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

// TEST(args, print_int) {
//     Args* args = New_args(NULL);
//     args_setInt(args, "testInt", 124);
//     EXPECT_STREQ("124", args_print(args, "testInt"));
//     args_deinit(args);
//     EXPECT_EQ(pikaMemNow(), 0);
// }

// TEST(args, test5) {
// int32_t testInt = 124;
// Args* args = New_args(NULL);
// args_bind(args, "i", "testInt", &testInt);
// char* type = args_getType(args, "testInt");
// args_print(args, "testInt");
// EXPECT_STREQ("124", args_print(args, "testInt"));
// args_deinit(args);
// EXPECT_EQ(pikaMemNow(), 0);
// }

// TEST(args, test6) {
//     Args* args = New_args(NULL);
//     args_setFloat(args, "testfloat", 1.42);
//     EXPECT_STREQ("1.420000", args_print(args, "testfloat"));
//     args_deinit(args);
//     EXPECT_EQ(pikaMemNow(), 0);
// }

// TEST(args, test7) {
// Args* args = New_args(NULL);
// float floatBindTest = 2.314;
// args_bind(args, "f", "floatBind", &floatBindTest);
// EXPECT_STREQ("2.314000", args_print(args, "floatBind"));
// args_deinit(args);
// EXPECT_EQ(pikaMemNow(), 0);
// }

// TEST(args, test8) {
//     Args* args = New_args(NULL);
//     args_setStr(args, "testString", "test string print");
//     EXPECT_STREQ("test string print", args_print(args, "testString"));
//     args_deinit(args);
//     EXPECT_EQ(pikaMemNow(), 0);
// }

// TEST(args, test9) {
// Args* args = New_args(NULL);
// char strBindTest[] = "test string bind";
// args_bindStr(args, "testStringBind", (char**)&strBindTest);
// EXPECT_STREQ("test string bind",
//  args_print(args, "testStringBind"));
// args_deinit(args);
// EXPECT_EQ(pikaMemNow(), 0);
// }

TEST(args, test12) {
    Args* args = New_args(NULL);
    args_setInt(args, "test", 13);
    args_setInt(args, "kw", 25);
    EXPECT_EQ(2, args_getSize(args));
    EXPECT_EQ(13, (int)args_getInt(args, "test"));
    args_removeArg(args, args_getArg(args, "test"));
    EXPECT_EQ(1, args_getSize(args));
    EXPECT_EQ(-999999999, (int)args_getInt(args, "test"));
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

struct test {
    int a;
    float b;
    int c;
};
TEST(args, struct_) {
    Args* args = New_args(NULL);
    struct test test_struct = {1, 2.2, 4};
    args_setStruct(args, "test", test_struct);
    struct test* out_struct = (struct test*)args_getStruct(args, "test");
    EXPECT_EQ(out_struct->a, 1);
    EXPECT_FLOAT_EQ(out_struct->b, 2.2);
    EXPECT_EQ(out_struct->c, 4);
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, mem) {
    EXPECT_EQ(pikaMemNow(), mem);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, index) {
    Args* args = New_args(NULL);
    args_pushArg(args, arg_newInt(1));
    args_pushArg(args, arg_newFloat(2.4));

    int a = arg_getInt(args_getArgByIndex(args, 1));
    float b = arg_getFloat(args_getArgByIndex(args, 0));
    /* assert */
    EXPECT_EQ(a, 1);
    EXPECT_FLOAT_EQ(b, 2.4);
    /* check memory */
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, int_float_convert) {
    Args* args = New_args(NULL);
    args_setInt(args, "a", 10);
    args_setFloat(args, "b", 2.333);

    float a = args_getFloat(args, "a");
    int b = args_getInt(args, "b");
    /* assert */
    EXPECT_EQ(a, 10);
    EXPECT_FLOAT_EQ(b, 2);
    /* check memory */
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, heap_struct) {
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    bytecode_frame.const_pool.size = 100;
    Args* args = New_args(NULL);
    args_setHeapStruct(args, "a", bytecode_frame, byteCodeFrame_deinit);
    ByteCodeFrame* bf_out = (ByteCodeFrame*)args_getHeapStruct(args, "a");
    /* assert */
    EXPECT_EQ(bf_out->const_pool.size, 100);
    /* check memory */
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, heap_struct_override) {
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    bytecode_frame.const_pool.size = 100;

    ByteCodeFrame bytecode_frame_1;
    byteCodeFrame_init(&bytecode_frame_1);
    bytecode_frame_1.const_pool.size = 100;

    Args* args = New_args(NULL);
    args_setHeapStruct(args, "a", bytecode_frame, byteCodeFrame_deinit);
    args_setHeapStruct(args, "a", bytecode_frame_1, byteCodeFrame_deinit);
    ByteCodeFrame* bf_out = (ByteCodeFrame*)args_getHeapStruct(args, "a");
    /* assert */
    EXPECT_EQ(bf_out->const_pool.size, 100);
    /* check memory */
    args_deinit(args);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, args_move) {
    Args* args = New_args(NULL);
    Args* args2 = New_args(NULL);
    args_setInt(args, "a", 100);
    args_moveArg(args, args2, args_getArg(args, "a"));
    /* assert */
    EXPECT_EQ(-999999999, args_getInt(args, "a"));
    EXPECT_EQ(100, args_getInt(args2, "a"));
    /* deinit */
    args_deinit(args);
    args_deinit(args2);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(args, args_mem) {
    uint8_t mem_test[] = {0x33, 0x55, 0x00, 0x15};
    Args* args = New_args(NULL);
    args_setBytes(args, "mem", mem_test, sizeof(mem_test));
    size_t mem_size = args_getBytesSize(args, "mem");
    char* mem_test_out = (char*)args_getBytes(args, "mem");
    EXPECT_EQ(mem_size, sizeof(mem_test));
    EXPECT_EQ(mem_test_out[0], 0x33);
    EXPECT_EQ(mem_test_out[1], 0x55);
    EXPECT_EQ(mem_test_out[2], 0x00);
    EXPECT_EQ(mem_test_out[3], 0x15);
    args_deinit(args);
}

TEST(args, dict) {
    PikaDict* dict = New_pikaDict();
    int64_t int64Out = 0;
    void* pointer = NULL;
    char* strOut = NULL;
    pikaDict_setInt(dict, "int64Test", (int64_t)22221);
    pikaDict_setPtr(dict, "pointerTest", (void*)2222322);
    pikaDict_setStr(dict, "strTest", "teeeds");

    int64Out = pikaDict_getInt(dict, "int64Test");
    pointer = pikaDict_getPtr(dict, "pointerTest");
    strOut = pikaDict_getStr(dict, "strTest");

    EXPECT_EQ(int64Out, 22221);
    EXPECT_EQ((uint64_t)pointer, 2222322);
    EXPECT_EQ(1, strEqu("teeeds", strOut));
    EXPECT_EQ(pikaDict_getType(dict, "int64Test"), ARG_TYPE_INT);
    EXPECT_EQ(pikaDict_getType(dict, "pointerTest"), ARG_TYPE_POINTER);
    EXPECT_EQ(pikaDict_getType(dict, "strTest"), ARG_TYPE_STRING);
    pikaDict_deinit(dict);
    EXPECT_EQ(pikaMemNow(), 0);
}
TEST_END