# Fibonacci 性能测试设计方案

## 1. 项目背景

基于PikaPython现有的性能测试基础设施，为PikaPython VM添加Fibonacci数列计算的性能基准测试。目标是：
- 为VM性能分析提供新的基准测试用例
- 利用现有的gprof热点分析和Google Benchmark基础设施
- 为后续VM优化提供量化指导

## 2. 现有性能测试基础设施分析

### 2.1 核心测试框架

**Google Benchmark集成**：
- 位置：`/home/ubuntu/pikapython/port/linux/benchmark/main.cpp`
- 测试模式：`BENCHMARK(function_name)->Unit(benchmark::kMillisecond)`
- 测试数据：JSON格式输出，支持多轮测试和统计分析

**性能分析工具**：
- gprof集成：`prof.sh`脚本自动生成热点分析报告
- 热点分析：`report.txt`包含函数调用时间、频率和调用图
- 内存分析：支持内存使用统计和泄露检测

**自动化测试流程**：
- `ci_benchmark.sh`：主入口脚本，完整的测试流程
- `ci_benchmark_times.sh`：批量测试(默认20次)和结果收集
- `ci_benchmark_calibration.py`：结果校准和Performance Points计算

### 2.2 现有测试用例模式

**循环结构测试**：
```cpp
static void for_loop_10000(benchmark::State& state) {
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        pikaVM_run(pikaMain, (char *)
            "a = 0\n"
            "for i in range(0, 10000):\n"
            "    a = a + 1\n");
        obj_deinit(pikaMain);
    }
}
```

**数学算法测试**：
```cpp
static void prime_number_100(benchmark::State& state) {
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, python_code);
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        // 结果验证
        obj_deinit(pikaMain);
    }
}
```

**C语言基准对比**：
- 提供相同算法的C语言实现
- 用于计算性能比率和Performance Points

## 3. Fibonacci 性能测试方案

### 3.1 方案一：递归实现测试 (推荐)

**测试目标**：
- 测试递归函数调用性能
- 分析函数调用栈和参数传递开销
- 评估VM在深度递归场景下的性能

**Python测试代码**：
```python
def fib_recursive(n):
    if n <= 1:
        return n
    return fib_recursive(n-1) + fib_recursive(n-2)

result = fib_recursive(30)
```

**C++实现**：
```cpp
static void fib_recursive_30(benchmark::State& state) {
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, (char*)
        "def fib_recursive(n):\n"
        "    if n <= 1:\n"
        "        return n\n"
        "    return fib_recursive(n-1) + fib_recursive(n-2)\n"
        "\n"
        "result = fib_recursive(30)\n");
    
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        
        int result = obj_getInt(pikaMain, (char*)"result");
        if (832040 != result) {  // fib(30) = 832040
            printf("Error: fib_recursive_30, expected 832040, got %d\r\n", result);
        }
        obj_deinit(pikaMain);
    }
    
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffs);
}
BENCHMARK(fib_recursive_30)->Unit(benchmark::kMillisecond);
```

**C语言基准**：
```cpp
static int fib_recursive_c(int n) {
    if (n <= 1) return n;
    return fib_recursive_c(n-1) + fib_recursive_c(n-2);
}

static void fib_recursive_30_c(benchmark::State& state) {
    int result = 0;
    for (auto _ : state) {
        result = fib_recursive_c(30);
        if (832040 != result) {
            printf("Error: fib_recursive_30_c\r\n");
        }
    }
}
BENCHMARK(fib_recursive_30_c)->Unit(benchmark::kMillisecond);
```

### 3.2 方案二：迭代实现测试

**测试目标**：
- 测试循环结构性能
- 分析变量赋值和算术运算开销
- 评估VM在大量循环场景下的性能

**Python测试代码**：
```python
def fib_iterative(n):
    if n <= 1:
        return n
    a, b = 0, 1
    for i in range(2, n+1):
        a, b = b, a + b
    return b

result = fib_iterative(10000)
```

**C++实现**：
```cpp
static void fib_iterative_10000(benchmark::State& state) {
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, (char*)
        "def fib_iterative(n):\n"
        "    if n <= 1:\n"
        "        return n\n"
        "    a, b = 0, 1\n"
        "    for i in range(2, n+1):\n"
        "        a, b = b, a + b\n"
        "    return b\n"
        "\n"
        "result = fib_iterative(10000)\n");
    
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        
        // 验证结果 (fib(10000)的结果非常大，这里验证类型)
        Arg* result_arg = obj_getArg(pikaMain, (char*)"result");
        if (ARG_TYPE_INT != arg_getType(result_arg)) {
            printf("Error: fib_iterative_10000 result type error\r\n");
        }
        obj_deinit(pikaMain);
    }
    
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffs);
}
BENCHMARK(fib_iterative_10000)->Unit(benchmark::kMillisecond);
```

### 3.3 方案三：混合测试套件

**测试目标**：
- 全面评估不同计算模式的性能
- 提供多维度的性能分析数据
- 支持不同优化场景的基准测试

**测试组合**：
1. **小数值递归**：`fib_recursive_20` - 快速递归测试
2. **中数值递归**：`fib_recursive_30` - 中等复杂度递归
3. **大数值迭代**：`fib_iterative_10000` - 大规模循环测试
4. **内存优化测试**：`fib_memory_test` - 内存使用分析

**扩展测试**：
```cpp
// 小递归测试
static void fib_recursive_20(benchmark::State& state) {
    // 类似fib_recursive_30，但使用n=20，结果6765
}

// 记忆化递归测试
static void fib_memoized_40(benchmark::State& state) {
    Args* buffs = New_strBuff();
    char* pikaAsm = pika_lines2Asm(buffs, (char*)
        "memo = {}\n"
        "def fib_memo(n):\n"
        "    if n in memo:\n"
        "        return memo[n]\n"
        "    if n <= 1:\n"
        "        result = n\n"
        "    else:\n"
        "        result = fib_memo(n-1) + fib_memo(n-2)\n"
        "    memo[n] = result\n"
        "    return result\n"
        "\n"
        "result = fib_memo(40)\n");
    // 实现细节...
}
```

## 4. 单独运行方案

### 4.1 方案A：Google Benchmark过滤器

**使用现有基础设施**：
```bash
# 运行所有fib测试
./Release/benchmark/pikascript_benchmark --benchmark_filter=fib

# 运行特定测试
./Release/benchmark/pikascript_benchmark --benchmark_filter=fib_recursive_30

# 输出JSON格式
./Release/benchmark/pikascript_benchmark --benchmark_filter=fib --benchmark_format=json
```

**集成到现有脚本**：
```bash
#!/bin/bash
# fib_benchmark.sh
cd Release && ninja -j0 && cd ..

# 运行fib专项测试
echo "Running Fibonacci Performance Tests..."
./Release/benchmark/pikascript_benchmark --benchmark_filter=fib --benchmark_format=json | tee fib_benchmark_result.json

# 运行性能分析
echo "Running profiling analysis..."
./Release/performance/pikascript_performance  # 需要修改为fib测试
gprof Release/performance/pikascript_performance gmon.out > fib_report.txt

echo "Fibonacci benchmark completed."
echo "Results: fib_benchmark_result.json"
echo "Profiling: fib_report.txt"
```

### 4.2 方案B：专用性能分析程序

**创建专用程序**：
```cpp
// performance/fib_main.c
#include "PikaMain.h"
#include "PikaVM.h"
#include <time.h>

int main(void) {
    PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
    
    printf("=== Fibonacci Performance Analysis ===\n");
    
    // 递归测试
    clock_t start = clock();
    pikaVM_run(pikaMain, 
        "def fib_recursive(n):\n"
        "    if n <= 1:\n"
        "        return n\n"
        "    return fib_recursive(n-1) + fib_recursive(n-2)\n"
        "\n"
        "result = fib_recursive(30)\n");
    clock_t end = clock();
    
    int result = obj_getInt(pikaMain, "result");
    printf("Recursive fib(30) = %d, time: %lfs\n", result, 
           ((double)end - (double)start) / (double)CLOCKS_PER_SEC);
    
    // 迭代测试
    start = clock();
    pikaVM_run(pikaMain, 
        "def fib_iterative(n):\n"
        "    if n <= 1:\n"
        "        return n\n"
        "    a, b = 0, 1\n"
        "    for i in range(2, n+1):\n"
        "        a, b = b, a + b\n"
        "    return b\n"
        "\n"
        "result2 = fib_iterative(10000)\n");
    end = clock();
    
    printf("Iterative fib(10000) computed, time: %lfs\n",
           ((double)end - (double)start) / (double)CLOCKS_PER_SEC);
    
    obj_deinit(pikaMain);
    return 0;
}
```

**CMakeLists.txt扩展**：
```cmake
# 添加到performance/CMakeLists.txt
add_executable(pikascript_fib_performance fib_main.c)
target_link_libraries(pikascript_fib_performance PUBLIC 
    ${CMAKE_PROJECT_NAME}-core)
```

### 4.3 方案C：Python脚本集成

**创建Python测试脚本**：
```python
# test/python/performance/fib_performance.py
import time

def fib_recursive(n):
    if n <= 1:
        return n
    return fib_recursive(n-1) + fib_recursive(n-2)

def fib_iterative(n):
    if n <= 1:
        return n
    a, b = 0, 1
    for i in range(2, n+1):
        a, b = b, a + b
    return b

def benchmark_fib():
    print("=== Fibonacci Performance Test ===")
    
    # 递归测试
    start = time.time()
    result1 = fib_recursive(30)
    end = time.time()
    print(f"Recursive fib(30) = {result1}, time: {end-start:.4f}s")
    
    # 迭代测试
    start = time.time()
    result2 = fib_iterative(10000)
    end = time.time()
    print(f"Iterative fib(10000) calculated, time: {end-start:.4f}s")
    
    print("=== Test Completed ===")

if __name__ == "__main__":
    benchmark_fib()
```

**集成到测试框架**：
```cpp
// test/performance-test.cpp
TEST_RUN_SINGLE_FILE(performance, 
                     fib_performance, 
                     "test/python/performance/fib_performance.py");
```

## 5. 性能分析和优化指导

### 5.1 热点分析预期

**基于现有report.txt分析**，Fibonacci测试可能的热点：

1. **递归函数调用**：
   - `VM_instruction_handler_RUN`（函数调用指令）
   - `PikaVMFrame_loadArgsFromMethodArg`（参数传递）
   - `obj_getArg`、`obj_setArg`（变量访问）

2. **算术运算**：
   - 加法运算指令处理
   - 整数类型转换和操作

3. **条件判断**：
   - 比较运算符处理
   - 分支跳转指令

4. **内存管理**：
   - 递归导致的栈帧创建/销毁
   - 局部变量内存分配

### 5.2 性能优化建议

**基于热点分析结果**：

1. **函数调用优化**：
   - 优化参数传递机制
   - 减少函数调用开销
   - 考虑尾递归优化

2. **指令优化**：
   - 优化算术运算指令
   - 减少类型检查开销
   - 指令合并优化

3. **内存优化**：
   - 优化栈帧管理
   - 减少内存分配频率
   - 考虑对象池复用

### 5.3 性能指标

**量化指标**：
- **Performance Points**：基于C语言基准的性能比率
- **执行时间**：绝对执行时间（毫秒）
- **内存使用**：峰值内存和平均内存使用
- **指令数**：执行的VM指令总数

**对比分析**：
- Python vs C性能比率
- 递归 vs 迭代性能差异
- 不同规模问题的性能缩放

## 6. 实施建议

### 6.1 推荐实施顺序

1. **阶段1**：实现方案一（递归实现）
   - 添加`fib_recursive_30`和`fib_recursive_30_c`
   - 集成到现有benchmark框架
   - 验证测试结果正确性

2. **阶段2**：添加迭代实现
   - 实现`fib_iterative_10000`
   - 对比递归和迭代性能差异

3. **阶段3**：创建专用分析工具
   - 实现方案B的专用性能分析程序
   - 集成gprof热点分析

4. **阶段4**：优化和调优
   - 基于热点分析结果进行VM优化
   - 重新测试验证优化效果

### 6.2 文件修改清单

**无需修改的文件**（利用现有基础设施）：
- `ci_benchmark.sh`
- `ci_benchmark_calibration.py`
- `ci_benchmark_select.py`
- `prof.sh`

**需要修改的文件**：
- `benchmark/main.cpp`：添加fib测试函数
- `performance/CMakeLists.txt`：添加专用性能程序（可选）
- `test/python/performance/`：添加Python测试脚本（可选）

### 6.3 预期收益

1. **性能基线**：为VM优化提供量化基线
2. **热点识别**：识别递归和迭代场景的性能瓶颈
3. **优化验证**：验证VM优化措施的有效性
4. **性能回归**：防止未来版本的性能回归

## 7. 总结

本方案充分利用PikaPython现有的性能测试基础设施，通过添加Fibonacci数列测试用例，为VM性能分析和优化提供新的维度。推荐优先实施方案一（递归实现），因为它能够最有效地暴露VM在函数调用和参数传递方面的性能特征，为后续优化工作提供明确的指导方向。

通过Google Benchmark的精确测量、gprof的热点分析以及现有的自动化测试流程，可以获得全面的性能分析数据，为PikaPython VM的持续优化提供坚实的数据基础。