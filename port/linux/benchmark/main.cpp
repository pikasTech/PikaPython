#include <benchmark/benchmark.h>
#include <algorithm>
#include <chrono>
#include "test_common.h"

extern "C" {
#include "PikaMain.h"
#include "PikaParser.h"
#include "PikaStdLib_MemChecker.h"
#include "PikaVM.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataStrs.h"
#include "pikaScript.h"
}

static void for_loop_10000(benchmark::State& state) {
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        /* run */
        pikaVM_run(pikaMain, (char *)
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
        pikaVM_run(pikaMain, (char *)
            "i = 0\n"
            "while i < 10000:\n"
            "    i = i + 1\n"
            "\n");
        obj_deinit(pikaMain);
    }
}
BENCHMARK(while_loop_10000)->Unit(benchmark::kMillisecond);

static void function_call_1000(benchmark::State& state) {
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(
        buffs, (char*)
                   "def collect(a, b=2, *args, **kwargs):\n"
                   "    return a + b\n"
                   "i = 0\n"
                   "while i < 1000:\n"
                   "    collect(i, 3, 4, 5, alpha=6, beta=7)\n"
                   "    i = i + 1\n"
                   "\n");
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        obj_deinit(pikaMain);
    }
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffs);
}
BENCHMARK(function_call_1000)->Unit(benchmark::kMillisecond);

static void function_call_default_1000(benchmark::State& state) {
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(
        buffs, (char*)
                   "def add(a, b=2):\n"
                   "    return a + b\n"
                   "i = 0\n"
                   "while i < 1000:\n"
                   "    add(i)\n"
                   "    i = i + 1\n"
                   "\n");
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        obj_deinit(pikaMain);
    }
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffs);
}
BENCHMARK(function_call_default_1000)->Unit(benchmark::kMillisecond);

static void function_call_kwargs_1000(benchmark::State& state) {
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(
        buffs, (char*)
                   "def collect(a, **kwargs):\n"
                   "    return a\n"
                   "i = 0\n"
                   "while i < 1000:\n"
                   "    collect(a=i, alpha=1, beta=2)\n"
                   "    i = i + 1\n"
                   "\n");
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        obj_deinit(pikaMain);
    }
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffs);
}
BENCHMARK(function_call_kwargs_1000)->Unit(benchmark::kMillisecond);

static void function_call_kwargs_large_constpool_1000(
    benchmark::State& state) {
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(
        buffs, (char*)
                   "def collect(a, **kwargs):\n"
                   "    return a\n"
                   "c0 = 'constant_00'\n"
                   "c1 = 'constant_01'\n"
                   "c2 = 'constant_02'\n"
                   "c3 = 'constant_03'\n"
                   "c4 = 'constant_04'\n"
                   "c5 = 'constant_05'\n"
                   "c6 = 'constant_06'\n"
                   "c7 = 'constant_07'\n"
                   "c8 = 'constant_08'\n"
                   "c9 = 'constant_09'\n"
                   "c10 = 'constant_10'\n"
                   "c11 = 'constant_11'\n"
                   "c12 = 'constant_12'\n"
                   "c13 = 'constant_13'\n"
                   "c14 = 'constant_14'\n"
                   "c15 = 'constant_15'\n"
                   "c16 = 'constant_16'\n"
                   "c17 = 'constant_17'\n"
                   "c18 = 'constant_18'\n"
                   "c19 = 'constant_19'\n"
                   "i = 0\n"
                   "while i < 1000:\n"
                   "    collect(a=i, alpha=1, beta=2)\n"
                   "    i = i + 1\n"
                   "\n");
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        obj_deinit(pikaMain);
    }
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffs);
}
BENCHMARK(function_call_kwargs_large_constpool_1000)
    ->Unit(benchmark::kMillisecond);

static void function_call_starred_1000(benchmark::State& state) {
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(
        buffs, (char*)
                   "def collect(a, b=2, *args, **kwargs):\n"
                   "    return a + b\n"
                   "values = (3, 4, 5)\n"
                   "i = 0\n"
                   "while i < 1000:\n"
                   "    collect(i, *values, alpha=6, beta=7)\n"
                   "    i = i + 1\n"
                   "\n");
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        obj_deinit(pikaMain);
    }
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffs);
}
BENCHMARK(function_call_starred_1000)->Unit(benchmark::kMillisecond);

static const char* fibonacci_source =
    "def fib(n):\n"
    "    if n < 2:\n"
    "        return n\n"
    "    return fib(n - 1) + fib(n - 2)\n"
    "result = fib(20)\n"
    "\n";

static void fibonacci_recursive_20(benchmark::State& state) {
    using clock = std::chrono::steady_clock;
    auto nanoseconds = [](clock::duration duration) {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration)
            .count();
    };
    auto compile_start = clock::now();
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, (char*)fibonacci_source);
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    auto compile_end = clock::now();

    pikaMemAllocationCountReset();
    auto root_create_start = clock::now();
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    auto root_create_end = clock::now();
    uint64_t root_create_allocations = pikaMemAllocationCount();
    uint32_t warm_heap_baseline_bytes = pikaMemNow();

    uint64_t execute_ns = 0;
    uint64_t execute_allocations = 0;
    pikaMemMaxReset();
    pikaMemAllocationCountReset();
    for (auto _ : state) {
        uint32_t before_execute_allocations = pikaMemAllocationCount();
        auto execute_start = clock::now();
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        auto execute_end = clock::now();
        execute_allocations +=
            pikaMemAllocationCount() - before_execute_allocations;
        if (6765 != obj_getInt(pikaMain, (char*)"result")) {
            state.SkipWithError("recursive fibonacci result mismatch");
        }
        execute_ns += nanoseconds(execute_end - execute_start);
    }
    uint32_t heap_peak_bytes = pikaMemMax();
    uint32_t before_destroy_allocations = pikaMemAllocationCount();
    auto root_destroy_start = clock::now();
    obj_deinit(pikaMain);
    auto root_destroy_end = clock::now();
    uint64_t root_destroy_allocations =
        pikaMemAllocationCount() - before_destroy_allocations;
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffs);

    state.counters["compile_once_ns"] =
        nanoseconds(compile_end - compile_start);
    state.counters["setup_root_create_ns"] =
        nanoseconds(root_create_end - root_create_start);
    state.counters["warm_execute_ns"] = benchmark::Counter(
        execute_ns, benchmark::Counter::kAvgIterations);
    state.counters["teardown_root_destroy_ns"] =
        nanoseconds(root_destroy_end - root_destroy_start);
    state.counters["setup_root_create_allocations"] =
        root_create_allocations;
    state.counters["execute_allocations"] = benchmark::Counter(
        execute_allocations, benchmark::Counter::kAvgIterations);
    state.counters["teardown_root_destroy_allocations"] =
        root_destroy_allocations;
    state.counters["heap_allocations"] = benchmark::Counter(
        execute_allocations, benchmark::Counter::kAvgIterations);
    state.counters["heap_peak_bytes"] = heap_peak_bytes;
    state.counters["warm_heap_baseline_bytes"] =
        warm_heap_baseline_bytes;
    state.counters["heap_residual_bytes"] = pikaMemNow();
    state.counters["root_destroy_count"] = 1;
}
BENCHMARK(fibonacci_recursive_20)->Unit(benchmark::kMillisecond);

static void fibonacci_recursive_20_cold_start(benchmark::State& state) {
    using clock = std::chrono::steady_clock;
    auto nanoseconds = [](clock::duration duration) {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration)
            .count();
    };
    uint64_t root_create_ns = 0;
    uint64_t compile_ns = 0;
    uint64_t first_execute_ns = 0;
    uint64_t root_destroy_ns = 0;
    uint64_t cleanup_ns = 0;
    uint64_t root_create_allocations = 0;
    uint64_t compile_allocations = 0;
    uint64_t execute_allocations = 0;
    uint64_t destroy_allocations = 0;
    uint64_t cleanup_allocations = 0;
    uint32_t heap_peak_bytes = 0;
    uint32_t heap_residual_bytes = 0;

    for (auto _ : state) {
        pikaMemMaxReset();
        pikaMemAllocationCountReset();

        auto root_create_start = clock::now();
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        auto root_create_end = clock::now();
        uint32_t after_root_allocations = pikaMemAllocationCount();

        auto compile_start = clock::now();
        Args* buffs = New_strBuff();
        char* pikaAsm = pika_lines2Asm(buffs, (char*)fibonacci_source);
        ByteCodeFrame bytecode_frame;
        byteCodeFrame_init(&bytecode_frame);
        byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
        auto compile_end = clock::now();
        uint32_t after_compile_allocations = pikaMemAllocationCount();

        auto execute_start = clock::now();
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        auto execute_end = clock::now();
        uint32_t after_execute_allocations = pikaMemAllocationCount();
        if (6765 != obj_getInt(pikaMain, (char*)"result")) {
            state.SkipWithError("cold fibonacci result mismatch");
        }

        auto root_destroy_start = clock::now();
        obj_deinit(pikaMain);
        auto root_destroy_end = clock::now();
        uint32_t after_destroy_allocations = pikaMemAllocationCount();

        auto cleanup_start = clock::now();
        byteCodeFrame_deinit(&bytecode_frame);
        args_deinit(buffs);
        auto cleanup_end = clock::now();
        uint32_t after_cleanup_allocations = pikaMemAllocationCount();

        root_create_ns +=
            nanoseconds(root_create_end - root_create_start);
        compile_ns += nanoseconds(compile_end - compile_start);
        first_execute_ns += nanoseconds(execute_end - execute_start);
        root_destroy_ns +=
            nanoseconds(root_destroy_end - root_destroy_start);
        cleanup_ns += nanoseconds(cleanup_end - cleanup_start);
        root_create_allocations += after_root_allocations;
        compile_allocations +=
            after_compile_allocations - after_root_allocations;
        execute_allocations +=
            after_execute_allocations - after_compile_allocations;
        destroy_allocations +=
            after_destroy_allocations - after_execute_allocations;
        cleanup_allocations +=
            after_cleanup_allocations - after_destroy_allocations;
        heap_peak_bytes = std::max(heap_peak_bytes, pikaMemMax());
        heap_residual_bytes = std::max(heap_residual_bytes, pikaMemNow());
    }

    state.counters["root_create_ns"] = benchmark::Counter(
        root_create_ns, benchmark::Counter::kAvgIterations);
    state.counters["compile_ns"] = benchmark::Counter(
        compile_ns, benchmark::Counter::kAvgIterations);
    state.counters["first_execute_ns"] = benchmark::Counter(
        first_execute_ns, benchmark::Counter::kAvgIterations);
    state.counters["root_destroy_ns"] = benchmark::Counter(
        root_destroy_ns, benchmark::Counter::kAvgIterations);
    state.counters["cleanup_ns"] = benchmark::Counter(
        cleanup_ns, benchmark::Counter::kAvgIterations);
    state.counters["root_create_allocations"] = benchmark::Counter(
        root_create_allocations, benchmark::Counter::kAvgIterations);
    state.counters["compile_allocations"] = benchmark::Counter(
        compile_allocations, benchmark::Counter::kAvgIterations);
    state.counters["execute_allocations"] = benchmark::Counter(
        execute_allocations, benchmark::Counter::kAvgIterations);
    state.counters["root_destroy_allocations"] = benchmark::Counter(
        destroy_allocations, benchmark::Counter::kAvgIterations);
    state.counters["cleanup_allocations"] = benchmark::Counter(
        cleanup_allocations, benchmark::Counter::kAvgIterations);
    state.counters["heap_allocations"] = benchmark::Counter(
        root_create_allocations + compile_allocations +
            execute_allocations + destroy_allocations +
            cleanup_allocations,
        benchmark::Counter::kAvgIterations);
    state.counters["heap_peak_bytes"] = heap_peak_bytes;
    state.counters["heap_residual_bytes"] = heap_residual_bytes;
}
BENCHMARK(fibonacci_recursive_20_cold_start)
    ->Unit(benchmark::kMillisecond);

static void embedded_control_loop_1000(benchmark::State& state) {
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(
        buffs, (char*)
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
                   "    i = i + 1\n"
                   "\n");
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        if (12 != obj_getInt(pikaMain, (char*)"measurement") ||
            4790 != obj_getInt(pikaMain, (char*)"checksum")) {
            state.SkipWithError("embedded control loop result mismatch");
        }
        obj_deinit(pikaMain);
    }
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffs);
}
BENCHMARK(embedded_control_loop_1000)->Unit(benchmark::kMillisecond);

void __platform_printf(char* fmt, ...) {}
static void for_print_1000(benchmark::State& state) {
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, (char*)
            "for i in range(1000):\n"
            "    print(i)\n"
            "\n");
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        /* run */
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        obj_deinit(pikaMain);
    }
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffs);
}
BENCHMARK(for_print_1000)->Unit(benchmark::kMillisecond);

static void prime_number_100(benchmark::State& state) {
    int num = 0;
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, (char*)
            "num = 0\n"
	        "i = 2\n"
            "while i < 100:\n"
            "    is_prime = 1\n"
	        "    j = 2\n"
            "    while j < i:\n"
            "        if i%j==0 :\n"
            "            is_prime = 0\n"
            "            break\n"
	        "        j += 1 \n"
            "    if is_prime:\n"
            "        num = num + i\n"
	        "    i += 1\n"
            "\n");
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        /* run */
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        num = obj_getInt(pikaMain, (char*)"num");
        if (1060 != num) {
            printf("Error: prime_number_100\r\n");
        }
        obj_deinit(pikaMain);
    }
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffs);
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
            printf("Error: prime_number_100\r\n");
        }
    }
}
BENCHMARK(prime_number_100_c)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
