#include "PikaMain.h"
#include "PikaParser.h"
#include "PikaStdLib_MemChecker.h"
#include "PikaVM.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataStrs.h"
#include "pikaScript.h"
#include <time.h>

#ifndef __platform_printf
void __platform_printf(char* format, ...) {
}
#endif

int main(void) {
    Args* buffs = New_strBuff();
    char* pika_asm = pika_lines2Asm(
        buffs,
        "def fib(n):\n"
        "    if n < 2:\n"
        "        return n\n"
        "    return fib(n - 1) + fib(n - 2)\n"
        "result = fib(20)\n");
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pika_asm);

    clock_t start = clock();
    for (int i = 0; i < 10; i++) {
        PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        if (6765 != obj_getInt(pikaMain, "result")) {
            printf("[ Error]: recursive fibonacci result mismatch\r\n");
            obj_deinit(pikaMain);
            byteCodeFrame_deinit(&bytecode_frame);
            args_deinit(buffs);
            return 1;
        }
        obj_deinit(pikaMain);
    }
    clock_t end = clock();
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffs);
#if PIKA_ARG_CACHE_ENABLE
    extern PikaMemInfo g_PikaMemInfo;
    printf("[ Info]: alloc times: %d, cached times: %d (%0.2f%%)\r\n",
           g_PikaMemInfo.alloc_times, g_PikaMemInfo.alloc_times_cache,
           ((float)g_PikaMemInfo.alloc_times_cache /
            (float)g_PikaMemInfo.alloc_times) *
               100.0);
#endif
    printf("[ Info]: time elapsed: %lfs\r\n",
          ((double)end - (double)start) / (double)CLOCKS_PER_SEC);
    return 0;
}
