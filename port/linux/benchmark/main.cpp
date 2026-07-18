#include <benchmark/benchmark.h>
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
