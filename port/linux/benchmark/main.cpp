#include <benchmark/benchmark.h>
#include "gtest/gtest.h"
extern "C" {
#include "PikaMain.h"
#include "PikaStdLib_MemChecker.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "pikaScript.h"
}

char* get_py(void);
static void BM_for_app(benchmark::State& state) {
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        /* run */
        obj_run(pikaMain, get_py());
    }
}
BENCHMARK(BM_for_app);

BENCHMARK_MAIN();