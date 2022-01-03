#include <benchmark/benchmark.h>
#include "gtest/gtest.h"
extern "C" {
#include "PikaMain.h"
#include "PikaStdLib_MemChecker.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "pikaScript.h"
}

static void BM_for_continue(benchmark::State& state) {
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        /* run */
        obj_run(pikaMain, (char*)
         "a = 0\n"
         "for i in range(0, 1000):\n"
         "    if i == 5:\n"
         "        continue\n"
         "    a = a + i\n"
         "\n"
        );
    }
}
BENCHMARK(BM_for_continue);

BENCHMARK_MAIN();