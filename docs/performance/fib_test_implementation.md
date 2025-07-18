# Fibonacci 性能测试实施方案

## 1. 方案概述

基于现有的PikaPython性能测试基础设施，添加简单的Fibonacci迭代测试。目标是：
- 最小化代码改动
- 充分利用现有的Google Benchmark和gprof基础设施
- 提供独立的测试启动方式
- 支持性能跑分和热点分析两种模式

## 2. 测试内容

**Fibonacci迭代算法测试**：
- 计算fib(10)，预期结果：55
- 使用迭代方式避免递归复杂度
- 测试VM在简单循环和算术运算场景下的性能

**Python测试代码**：
```python
def fib_iterative(n):
    if n <= 1:
        return n
    a, b = 0, 1
    for i in range(2, n+1):
        a, b = b, a + b
    return b

result = fib_iterative(10)
```

## 3. 文件修改清单

### 3.1 需要修改的文件

#### 文件1：`port/linux/benchmark/main.cpp`
**修改点**：添加fib测试函数
**位置**：在`prime_number_100_c`函数之后，`BENCHMARK_MAIN()`之前

#### 文件2：`port/linux/performance/main.c`
**修改点**：替换现有测试代码为fib测试
**位置**：`main`函数中的`pikaVM_run`调用

#### 文件3：`port/linux/fib_test.sh`（新建）
**修改点**：创建独立的测试启动脚本
**位置**：新建文件

## 4. 具体修改内容

### 4.1 benchmark/main.cpp 修改

**在第122行后添加**：
```cpp
static void fib_iterative_10(benchmark::State& state) {
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
        "result = fib_iterative(10)\n");
    
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    byteCodeFrame_appendFromAsm(&bytecode_frame, pikaAsm);
    
    for (auto _ : state) {
        PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        pikaVM_runByteCodeFrame(pikaMain, &bytecode_frame);
        
        int result = obj_getInt(pikaMain, (char*)"result");
        if (55 != result) {  // fib(10) = 55
            printf("Error: fib_iterative_10, expected 55, got %d\r\n", result);
        }
        obj_deinit(pikaMain);
    }
    
    byteCodeFrame_deinit(&bytecode_frame);
    args_deinit(buffs);
}
BENCHMARK(fib_iterative_10)->Unit(benchmark::kMillisecond);

static void fib_iterative_10_c(benchmark::State& state) {
    int result = 0;
    for (auto _ : state) {
        // C语言基准实现
        int n = 10;
        if (n <= 1) {
            result = n;
        } else {
            int a = 0, b = 1;
            for (int i = 2; i <= n; i++) {
                int temp = a + b;
                a = b;
                b = temp;
            }
            result = b;
        }
        if (55 != result) {
            printf("Error: fib_iterative_10_c\r\n");
        }
    }
}
BENCHMARK(fib_iterative_10_c)->Unit(benchmark::kMillisecond);
```

### 4.2 performance/main.c 修改

**替换第24-27行的pikaVM_run调用**：
```c
pikaVM_run(pikaMain, 
    "def fib_iterative(n):\n"
    "    if n <= 1:\n"
    "        return n\n"
    "    a, b = 0, 1\n"
    "    for i in range(2, n+1):\n"
    "        a, b = b, a + b\n"
    "    return b\n"
    "\n"
    "for i in range(1000):\n"
    "    result = fib_iterative(10)\n"
    );
```

### 4.3 fib_test.sh 脚本内容

**创建新文件**：`port/linux/fib_test.sh`
```bash
#!/bin/bash
# Fibonacci Performance Test Script

cd "$(dirname "$0")"

MODE=${1:-benchmark}

case $MODE in
    "benchmark"|"b")
        echo "=== Running Fibonacci Benchmark Test ==="
        echo "Building..."
        cd Release && ninja -j0 && cd ..
        
        echo "Running benchmark..."
        ./Release/benchmark/pikascript_benchmark --benchmark_filter=fib_iterative_10 --benchmark_format=json | tee fib_benchmark_result.json
        
        echo ""
        echo "=== Fibonacci Benchmark Results ==="
        echo "Results saved to: fib_benchmark_result.json"
        echo "To view formatted results:"
        echo "cat fib_benchmark_result.json | python3 -m json.tool"
        ;;
        
    "profile"|"p")
        echo "=== Running Fibonacci Profile Test ==="
        echo "Building with profiling..."
        cd Release && ninja -j0 && cd ..
        
        echo "Running performance test with profiling..."
        ./Release/performance/pikascript_performance
        
        echo "Generating profile report..."
        gprof Release/performance/pikascript_performance gmon.out > fib_profile_report.txt
        
        echo ""
        echo "=== Fibonacci Profile Results ==="
        echo "Profile report saved to: fib_profile_report.txt"
        echo "Top 10 functions by time:"
        head -30 fib_profile_report.txt | tail -20
        ;;
        
    "help"|"h"|*)
        echo "Usage: $0 [MODE]"
        echo ""
        echo "Modes:"
        echo "  benchmark, b    Run Google Benchmark test (default)"
        echo "  profile, p      Run gprof profiling analysis"
        echo "  help, h         Show this help message"
        echo ""
        echo "Examples:"
        echo "  $0              # Run benchmark test"
        echo "  $0 benchmark    # Run benchmark test"
        echo "  $0 profile      # Run profiling analysis"
        ;;
esac
```

## 5. 使用方法

### 5.1 准备工作

```bash
# 进入Linux port目录
cd /home/ubuntu/pikapython/port/linux

# 给脚本添加执行权限
chmod +x fib_test.sh
```

### 5.2 运行基准测试

```bash
# 运行性能跑分
./fib_test.sh benchmark
# 或简写
./fib_test.sh b
# 或默认模式
./fib_test.sh
```

**输出示例**：
```json
{
  "benchmarks": [
    {
      "name": "fib_iterative_10",
      "cpu_time": 0.0234,
      "time_unit": "ms"
    },
    {
      "name": "fib_iterative_10_c", 
      "cpu_time": 0.0001,
      "time_unit": "ms"
    }
  ]
}
```

### 5.3 运行热点分析

```bash
# 运行热点分析
./fib_test.sh profile
# 或简写
./fib_test.sh p
```

**输出示例**：
```
=== Fibonacci Profile Results ===
Profile report saved to: fib_profile_report.txt
Top 10 functions by time:
 15.23    0.45    0.15   args_getArg
 12.34    0.60    0.12   VM_instruction_handler_RUN
 10.87    0.71    0.11   pikaVM_runByteCodeFrame
...
```

## 6. 预期性能数据

### 6.1 基准测试结果

**预期性能指标**：
- `fib_iterative_10`: ~0.02-0.05ms (PikaPython)
- `fib_iterative_10_c`: ~0.0001ms (C语言基准)
- 性能比率: ~200-500倍差距

### 6.2 热点分析结果

**预期热点函数**：
1. **args_getArg** - 参数获取
2. **VM_instruction_handler_RUN** - 指令执行
3. **pikaVM_runByteCodeFrame** - 字节码执行
4. **obj_getInt/obj_setInt** - 整数操作
5. **hash_time33** - 哈希计算

**优化建议**：
- 优化参数传递机制
- 减少指令执行开销
- 优化整数运算性能

## 7. 集成到现有流程

### 7.1 集成到CI基准测试

**修改ci_benchmark.sh**（可选）：
```bash
# 在现有测试后添加
echo "Running Fibonacci specific test..."
./fib_test.sh benchmark
```

### 7.2 独立性能监控

**性能回归检测**：
```bash
# 定期运行并比较结果
./fib_test.sh benchmark > fib_current.json
# 与baseline比较
python3 -c "
import json
with open('fib_current.json', 'r') as f:
    current = json.load(f)
fib_time = current['benchmarks'][0]['cpu_time']
print(f'Current fib performance: {fib_time:.4f}ms')
"
```

## 8. 文件清单总结

### 8.1 修改文件

1. **port/linux/benchmark/main.cpp**
   - 添加`fib_iterative_10`和`fib_iterative_10_c`函数
   - 添加对应的BENCHMARK宏

2. **port/linux/performance/main.c**
   - 替换测试代码为fib测试
   - 保持原有的性能分析框架

### 8.2 新增文件

1. **port/linux/fib_test.sh**
   - 独立的测试启动脚本
   - 支持benchmark和profile两种模式

2. **docs/performance/fib_test_implementation.md**
   - 本实施文档

### 8.3 生成文件

1. **port/linux/fib_benchmark_result.json**
   - 基准测试结果
   
2. **port/linux/fib_profile_report.txt**
   - 热点分析报告

## 9. 验证步骤

1. **代码修改验证**：
   ```bash
   # 验证编译通过
   cd Release && ninja -j0
   ```

2. **功能验证**：
   ```bash
   # 验证fib计算正确
   ./fib_test.sh benchmark
   # 检查结果是否为55
   ```

3. **性能验证**：
   ```bash
   # 验证热点分析
   ./fib_test.sh profile
   # 检查profile报告生成
   ```

## 10. 总结

这个简化方案具有以下优势：

1. **最小化改动**：仅修改2个文件，新增1个脚本
2. **充分利用现有基础设施**：Google Benchmark + gprof + 现有构建系统
3. **独立运行**：专用脚本支持两种测试模式
4. **简单验证**：fib(10)=55，易于验证正确性
5. **实用性强**：提供实际的性能优化指导

实施后即可获得PikaPython VM在简单数学计算场景下的性能基线，为后续优化工作提供数据支持。