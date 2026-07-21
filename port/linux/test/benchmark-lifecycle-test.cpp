#include <gtest/gtest.h>

extern "C" {
#include "PikaMain.h"
#include "PikaParser.h"
#include "PikaVM.h"
#include "dataArgs.h"
#include "dataMemory.h"
}

TEST(benchmark_lifecycle, root_survives_repeated_execution_and_one_teardown) {
    ASSERT_EQ(0u, pikaMemNow());
    Args* buffers = New_strBuff();
    char* assembly = pika_lines2Asm(
        buffers, (char*)
                     "def fib(n):\n"
                     "    if n < 2:\n"
                     "        return n\n"
                     "    return fib(n - 1) + fib(n - 2)\n"
                     "result = fib(15)\n");
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, assembly);
    PikaObj* root = newRootObj((char*)"pikaMain", New_PikaMain);

    for (int iteration = 0; iteration < 500; ++iteration) {
        ASSERT_NE(nullptr, pikaVM_runByteCodeFrame(root, &bytecode_frame));
        ASSERT_EQ(610, obj_getInt(root, (char*)"result")) << iteration;
    }

    ASSERT_NE(nullptr, pikaVM_run(root, (char*)"missing_name()\n"));
    ASSERT_NE(nullptr, pikaVM_runByteCodeFrame(root, &bytecode_frame));
    EXPECT_EQ(610, obj_getInt(root, (char*)"result"));

    obj_deinit(root);
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffers);
    EXPECT_EQ(0u, pikaMemNow());
}
