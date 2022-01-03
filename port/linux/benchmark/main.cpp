#include <benchmark/benchmark.h>
#include "gtest/gtest.h"
extern "C" {
#include "PikaMain.h"
#include "PikaStdLib_MemChecker.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "pikaScript.h"
}

static void BM_for_app(benchmark::State& state) {
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        /* run */
        obj_run(pikaMain, (char*)
         "a = 0\n"
         "for i in range(0, 10000):\n"
         "    a = a + 1\n"
         "print(a)\n"
         "\n"
        );
    }
}
BENCHMARK(BM_for_app);

BENCHMARK_MAIN();