#!/bin/bash
# Fibonacci Performance Test Script - Integrated Analysis

cd "$(dirname "$0")"

MODE=${1:-full}
ANALYZER_PATH="../../docs/performance/profile_analyzer.py"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
BASE_PATH="$(pwd)"

case $MODE in
    "benchmark"|"b")
        echo "=== Fibonacci Benchmark Test ==="
        echo "Building..."
        cd Release && ninja -j0 && cd ..
        
        echo "Running benchmark..."
        BENCHMARK_FILE="fib_benchmark_${TIMESTAMP}.json"
        ./Release/benchmark/pikascript_benchmark --benchmark_filter=fib_recursive_25 --benchmark_format=json > "$BENCHMARK_FILE"
        
        echo ""
        echo "=== Benchmark Results ==="
        echo "📊 Benchmark data: $BASE_PATH/$BENCHMARK_FILE"
        
        # Extract key metrics
        if command -v python3 &> /dev/null; then
            python3 -c "
import json
try:
    with open('$BENCHMARK_FILE', 'r') as f:
        data = json.load(f)
    for benchmark in data.get('benchmarks', []):
        if 'fib_recursive_25' in benchmark['name']:
            print(f\"✅ {benchmark['name']}: {benchmark['real_time']:.3f} ms\")
except:
    print('Raw benchmark data in $BENCHMARK_FILE')
"
        fi
        ;;
        
    "profile"|"p")
        echo "=== Fibonacci Profile Test ==="
        echo "Building with profiling..."
        cd Release && ninja -j0 && cd ..
        
        echo "Running performance test..."
        PROFILE_RAW="fib_profile_raw_${TIMESTAMP}.txt"
        ./Release/performance/pikascript_performance
        
        echo "Generating profile report..."
        gprof Release/performance/pikascript_performance gmon.out > "$PROFILE_RAW"
        
        echo "Analyzing hotspots..."
        ANALYSIS_REPORT="fib_analysis_${TIMESTAMP}.txt"
        if [ -f "$ANALYZER_PATH" ]; then
            python3 "$ANALYZER_PATH" "$PROFILE_RAW" -o "$ANALYSIS_REPORT"
            echo ""
            echo "=== Profile Analysis Results ==="
            echo "📄 Raw profile data: $BASE_PATH/$PROFILE_RAW"
            echo "📊 Analysis report: $BASE_PATH/$ANALYSIS_REPORT"
            echo ""
            cat "$ANALYSIS_REPORT"
        else
            echo "Profile analyzer not found at $ANALYZER_PATH"
            echo "📄 Raw profile data: $BASE_PATH/$PROFILE_RAW"
        fi
        ;;
        
    "full"|"f")
        echo "=== Full Fibonacci Performance Analysis ==="
        echo "Building..."
        cd Release && ninja -j0 && cd ..
        
        # Run benchmark test
        echo "Running benchmark test..."
        BENCHMARK_FILE="fib_benchmark_${TIMESTAMP}.json"
        ./Release/benchmark/pikascript_benchmark --benchmark_filter=fib_recursive_25 --benchmark_format=json > "$BENCHMARK_FILE" 2>/dev/null
        
        # Run profile test
        echo "Running profile test..."
        PROFILE_RAW="fib_profile_raw_${TIMESTAMP}.txt"
        ./Release/performance/pikascript_performance > /dev/null 2>&1
        gprof Release/performance/pikascript_performance gmon.out > "$PROFILE_RAW" 2>/dev/null
        
        # Generate analysis
        echo "Generating analysis report..."
        ANALYSIS_REPORT="fib_analysis_${TIMESTAMP}.txt"
        if [ -f "$ANALYZER_PATH" ]; then
            python3 "$ANALYZER_PATH" "$PROFILE_RAW" -o "$ANALYSIS_REPORT" 2>/dev/null
        fi
        
        echo ""
        echo "=== PikaPython Fibonacci 性能分析简报 ==="
        echo "测试时间: $(date)"
        echo ""
        
        # Extract and display key metrics
        if command -v python3 &> /dev/null && [ -f "$BENCHMARK_FILE" ]; then
            echo "📊 性能基准:"
            python3 -c "
import json
try:
    with open('$BENCHMARK_FILE', 'r') as f:
        data = json.load(f)
    pika_time = None
    cpython_time = None
    for benchmark in data.get('benchmarks', []):
        if 'fib_recursive_25' in benchmark['name'] and 'cpython' not in benchmark['name']:
            pika_time = benchmark['real_time']
            print(f\"  PikaPython fib(25): {pika_time:.3f} ms\")
        elif 'fib_recursive_25_cpython' in benchmark['name']:
            cpython_time = benchmark['real_time']
            print(f\"  CPython基准 fib(25): {cpython_time:.6f} ms\")
    if pika_time and cpython_time:
        ratio = cpython_time / pika_time
        print(f\"  性能比率: PikaPython比CPython快 {ratio:.1f}x\")
except:
    print('  基准测试数据解析失败')
"
        fi
        
        echo ""
        echo "🔥 热点分析 (原始数据):"
        # Extract raw hotspot data directly from profile file
        python3 -c "
try:
    with open('$PROFILE_RAW', 'r') as f:
        lines = f.readlines()
    
    # Find the data section
    data_start = False
    count = 0
    for line in lines:
        line = line.strip()
        if 'time   seconds   seconds' in line and 'name' in line:
            data_start = True
            continue
        
        if data_start and line and count < 8:
            # Parse gprof format: %time cumulative self calls name
            parts = line.split()
            if len(parts) >= 4 and parts[0].replace('.', '').isdigit():
                time_percent = parts[0]
                self_seconds = parts[2]
                func_name = parts[-1]
                print(f\"  {func_name}: {time_percent}% ({self_seconds}s)\")
                count += 1
            elif not line or 'copyright' in line.lower():
                break
except:
    print('  原始热点数据解析失败')
"
        
        echo ""
        echo "📁 详细数据文件:"
        echo "  - 基准测试: $BASE_PATH/$BENCHMARK_FILE"
        echo "  - 原始热点: $BASE_PATH/$PROFILE_RAW"
        if [ -f "$ANALYSIS_REPORT" ]; then
            echo "  - 分析报告: $BASE_PATH/$ANALYSIS_REPORT"
        fi
        
        echo ""
        echo "💡 快速命令:"
        echo "  查看完整分析: cat $ANALYSIS_REPORT"
        echo "  查看基准详情: cat $BENCHMARK_FILE | python3 -m json.tool"
        echo "  查看原始热点: head -50 $PROFILE_RAW"
        ;;
        
    "help"|"h"|*)
        echo "Usage: $0 [MODE]"
        echo ""
        echo "Modes:"
        echo "  full, f         完整分析 (编译+基准+热点+分析报告) [默认]"
        echo "  benchmark, b    仅运行基准测试"
        echo "  profile, p      仅运行热点分析"
        echo "  help, h         显示帮助信息"
        echo ""
        echo "Examples:"
        echo "  $0              # 完整性能分析"
        echo "  $0 full         # 完整性能分析"
        echo "  $0 benchmark    # 仅基准测试"
        echo "  $0 profile      # 仅热点分析"
        ;;
esac

echo ""