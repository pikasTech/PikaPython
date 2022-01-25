#include <benchmark/benchmark.h>
#include "gtest/gtest.h"

extern "C" {
#include "PikaMain.h"
#include "PikaStdLib_MemChecker.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "pikaScript.h"
}

static void for_loop_10000(benchmark::State& state) {
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        /* run */
        obj_run(pikaMain, (char *)
            "a = 0\n"
            "for i in range(0, 10000):\n"
            "    a = a + 1\n"
            "\n");
        obj_deinit(pikaMain);
    }
}
BENCHMARK(for_loop_10000)->Unit(benchmark::kMillisecond);

static void while_loop_10000(benchmark::State& state) {
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        /* run */
        obj_run(pikaMain, (char *)
            "i = 0\n"
            "while i < 10000:\n"
            "    i = i + 1\n"
            "\n");
        obj_deinit(pikaMain);
    }
}
BENCHMARK(while_loop_10000)->Unit(benchmark::kMillisecond);

static void prime_number_100(benchmark::State& state) {
    int num = 0;
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        /* run */
        obj_run(pikaMain, (char *)
            "num = 0\n"
            "i = 2\n"
            "for i in range(2,100):\n"
            "    j=2\n"
            "    is_prime = 1\n"
            "    for j in range(2,i):\n"
            "        if i%j==0 :\n"
            "            is_prime = 0\n"
            "            break\n"
            "    if is_prime:\n"
            "        num = num + i\n"
            "\n");
        num = obj_getInt(pikaMain, (char*)"num");
        if (1060 != num) {
            printf("[error]: prime_number_100\r\n");
        }
        obj_deinit(pikaMain);
    }
}
BENCHMARK(prime_number_100)->Unit(benchmark::kMillisecond);


static void prime_number_100_c(benchmark::State& state) {
    int num = 0;
    for (auto _ : state) {
        num = 0;
        /* run */
        for (int i = 2; i < 100; i++) {
            int is_prime = 1;
            for (int j = 2; j < i; j++) {
                if (i % j == 0) {
                    is_prime = 0;
                    break;
                }
            }
            if (is_prime) {
                num = num + i;
            }
        }
        if (1060 != num) {
            printf("[error]: prime_number_100\r\n");
        }
    }
}
BENCHMARK(prime_number_100_c)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();