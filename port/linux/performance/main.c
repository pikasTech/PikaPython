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
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(pikaMain, pikaModules_py_a);
    /* run */
    __platform_printf("BEGIN\r\n");
    /* clang-format off */
    clock_t start = clock();
    pikaVM_run(pikaMain, 
    "for i in range(500000):\n"
    "    print(i)"
    );
    clock_t end = clock();
    /* deinit */
    obj_deinit(pikaMain);
#if PIKA_ARG_CACHE_ENABLE
    extern PikaMemInfo pikaMemInfo;
    printf("[ Info]: alloc times: %d, cached times: %d (%0.2f%%)\r\n",
           pikaMemInfo.alloc_times, pikaMemInfo.alloc_times_cache,
           ((float)pikaMemInfo.alloc_times_cache /
            (float)pikaMemInfo.alloc_times) *
               100.0);
#endif
    printf("[ Info]: time elapsed: %lfs\r\n",
          ((double)end - (double)start) / (double)CLOCKS_PER_SEC);
    return 0;
}
