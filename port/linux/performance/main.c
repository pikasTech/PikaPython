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
        "class Controller:\n"
        "    def __init__(self):\n"
        "        self.integral = 0\n"
        "        self.previous = 0\n"
        "        self.output = 0\n"
        "    def update(self, target, measurement):\n"
        "        error = target - measurement\n"
        "        self.integral = self.integral + error\n"
        "        derivative = error - self.previous\n"
        "        output = error * 3 + self.integral + derivative\n"
        "        if output > 500:\n"
        "            output = 500\n"
        "        if output < -500:\n"
        "            output = -500\n"
        "        self.previous = error\n"
        "        self.output = output\n"
        "        return output\n"
        "controller = Controller()\n"
        "measurement = 0\n"
        "checksum = 0\n"
        "i = 0\n"
        "while i < 1000:\n"
        "    target = 120\n"
        "    if i % 200 >= 100:\n"
        "        target = -80\n"
        "    output = controller.update(target, measurement)\n"
        "    if output > measurement:\n"
        "        measurement = measurement + 2\n"
        "    else:\n"
        "        measurement = measurement - 2\n"
        "    checksum = checksum + output\n"
        "    i = i + 1\n");
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pika_asm);

    clock_t start = clock();
    for (int i = 0; i < 100; i++) {
        PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        if (12 != obj_getInt(pikaMain, "measurement") ||
            4790 != obj_getInt(pikaMain, "checksum")) {
            printf("[ Error]: embedded control loop result mismatch\r\n");
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
