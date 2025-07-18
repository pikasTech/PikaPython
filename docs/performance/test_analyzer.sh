#!/bin/bash
# 测试 profile_analyzer.py 工具的脚本

cd "$(dirname "$0")"

echo "=== 测试 PikaPython 性能分析工具 ==="
echo ""

# 检查是否存在热点数据文件
PROFILE_FILE="../../port/linux/fib_profile_report.txt"

if [ ! -f "$PROFILE_FILE" ]; then
    echo "错误: 找不到热点数据文件 $PROFILE_FILE"
    echo "请先运行: cd ../../port/linux && ./fib_test.sh profile"
    exit 1
fi

echo "✅ 找到热点数据文件: $PROFILE_FILE"
echo ""

# 测试1: 基本功能测试
echo "📊 测试1: 基本分析功能"
echo "----------------------------------------"
python3 profile_analyzer.py "$PROFILE_FILE" | head -30
echo ""

# 测试2: 输出到文件
echo "📊 测试2: 输出分析报告到文件"
echo "----------------------------------------"
python3 profile_analyzer.py "$PROFILE_FILE" -o fib_analysis_report.txt
if [ -f "fib_analysis_report.txt" ]; then
    echo "✅ 报告已生成: fib_analysis_report.txt"
    echo "文件大小: $(wc -l < fib_analysis_report.txt) 行"
    echo ""
    echo "📋 报告预览 (前20行):"
    head -20 fib_analysis_report.txt
    echo ""
    echo "📋 报告结尾 (最后10行):"
    tail -10 fib_analysis_report.txt
else
    echo "❌ 报告生成失败"
fi
echo ""

# 测试3: 详细模式
echo "📊 测试3: 详细模式测试"
echo "----------------------------------------"
python3 profile_analyzer.py "$PROFILE_FILE" -v -o fib_detailed_report.txt
echo ""

# 测试4: 错误处理测试
echo "📊 测试4: 错误处理测试"
echo "----------------------------------------"
echo "测试不存在的文件:"
python3 profile_analyzer.py "nonexistent_file.txt" 2>&1 | head -5
echo ""

# 测试5: 帮助信息
echo "📊 测试5: 帮助信息"
echo "----------------------------------------"
python3 profile_analyzer.py --help
echo ""

echo "=== 测试完成 ==="
echo ""
echo "生成的文件:"
ls -la fib_*_report.txt 2>/dev/null || echo "没有生成报告文件"
echo ""
echo "使用方法:"
echo "python3 profile_analyzer.py <profile_file> [-o output_file] [-v]"