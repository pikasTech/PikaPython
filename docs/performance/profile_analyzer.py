#!/usr/bin/env python3
"""
PikaPython Performance Profile Analyzer
分析gprof热点输出数据，生成简明的优化建议报告
"""

import re
import sys
import argparse
from typing import List, Dict, Tuple, Optional
from dataclasses import dataclass
from pathlib import Path

@dataclass
class HotspotFunction:
    """热点函数数据结构"""
    name: str
    time_percent: float
    cumulative_seconds: float
    self_seconds: float
    calls: Optional[int] = None
    self_ms_per_call: Optional[float] = None
    
    def __str__(self):
        return f"{self.name}: {self.time_percent:.2f}% ({self.self_seconds:.3f}s)"

class ProfileAnalyzer:
    """性能分析器"""
    
    def __init__(self):
        self.functions: List[HotspotFunction] = []
        self.total_time: float = 0.0
        self.categories = {
            'vm_execution': ['VM_instruction_', '__pikaVM_', 'pikaVM_'],
            'memory_management': ['pikaMalloc', 'pikaFree', 'arg_deinit', 'arg_init', 'New_', '_deinit'],
            'argument_handling': ['args_', 'arg_', '_updateArg', 'PikaVMFrame_load'],
            'object_operations': ['obj_', '_obj_'],
            'string_operations': ['str', 'String'],
            'hash_operations': ['hash_'],
            'stack_operations': ['stack_'],
            'property_access': ['getProp', 'setProp', 'getArg', 'setArg'],
        }
        
    def parse_gprof_output(self, filepath: str) -> bool:
        """解析gprof输出文件"""
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                content = f.read()
        except FileNotFoundError:
            print(f"Error: 文件 {filepath} 不存在")
            return False
        except Exception as e:
            print(f"Error: 读取文件失败 - {e}")
            return False
            
        # 查找flat profile部分
        flat_profile_start = content.find('Flat profile:')
        if flat_profile_start == -1:
            print("Error: 未找到Flat profile部分")
            return False
            
        # 提取flat profile数据
        lines = content[flat_profile_start:].split('\n')
        
        # 跳过头部直到找到数据行
        data_start = 0
        for i, line in enumerate(lines):
            if 'time' in line and 'seconds' in line and 'name' in line:
                data_start = i + 1
                break
                
        if data_start == 0:
            print("Error: 未找到数据表头")
            return False
            
        # 解析数据行
        for line in lines[data_start:]:
            line = line.strip()
            if not line or line.startswith('%') or line.startswith('---'):
                continue
                
            # 遇到其他section就停止
            if any(keyword in line.lower() for keyword in ['call graph', 'copyright', 'copying']):
                break
                
            # 解析数据行
            func = self._parse_data_line(line)
            if func:
                self.functions.append(func)
                
        if self.functions:
            self.total_time = self.functions[0].cumulative_seconds if self.functions else 0.0
            
        return len(self.functions) > 0
        
    def _parse_data_line(self, line: str) -> Optional[HotspotFunction]:
        """解析单行数据"""
        # 使用正则表达式解析gprof输出格式
        # 格式: time% cumulative_seconds self_seconds calls self_ms/call total_ms/call name
        parts = line.split()
        if len(parts) < 4:
            return None
            
        try:
            time_percent = float(parts[0])
            cumulative_seconds = float(parts[1])
            self_seconds = float(parts[2])
            
            # 函数名通常在最后
            name = parts[-1]
            
            # 过滤掉一些不重要的函数
            if self._should_ignore_function(name):
                return None
                
            return HotspotFunction(
                name=name,
                time_percent=time_percent,
                cumulative_seconds=cumulative_seconds,
                self_seconds=self_seconds
            )
        except (ValueError, IndexError):
            return None
            
    def _should_ignore_function(self, name: str) -> bool:
        """判断是否应该忽略某个函数"""
        ignore_patterns = [
            'frame_dummy', '__libc_', '__gmon_', '_start', 'main',
            'register_tm_clones', 'deregister_tm_clones'
        ]
        return any(pattern in name.lower() for pattern in ignore_patterns)
        
    def categorize_functions(self) -> Dict[str, List[HotspotFunction]]:
        """将函数按类别分组"""
        categorized = {category: [] for category in self.categories.keys()}
        categorized['others'] = []
        
        for func in self.functions:
            category_found = False
            for category, patterns in self.categories.items():
                if any(pattern in func.name for pattern in patterns):
                    categorized[category].append(func)
                    category_found = True
                    break
                    
            if not category_found:
                categorized['others'].append(func)
                
        return categorized
        
    def generate_report(self) -> str:
        """生成分析报告（限制在100行以内）"""
        if not self.functions:
            return "Error: 没有找到性能数据"
            
        report_lines = []
        
        # 报告头部
        report_lines.append("=" * 60)
        report_lines.append("PikaPython 性能热点分析报告")
        report_lines.append("=" * 60)
        
        # 总体统计
        total_functions = len(self.functions)
        top_5_time = sum(f.time_percent for f in self.functions[:5])
        report_lines.append(f"📊 总计: {total_functions}个函数, Top5占用: {top_5_time:.1f}%")
        
        # 按类别分析
        categorized = self.categorize_functions()
        category_stats = []
        
        for category, functions in categorized.items():
            if not functions:
                continue
                
            total_time = sum(f.time_percent for f in functions)
            category_stats.append((category, total_time, functions))
            
        # 按时间占用排序
        category_stats.sort(key=lambda x: x[1], reverse=True)
        
        report_lines.append("")
        report_lines.append("🔥 热点类别分析:")
        
        category_names = {
            'vm_execution': 'VM执行引擎',
            'argument_handling': '参数处理',
            'memory_management': '内存管理',
            'object_operations': '对象操作',
            'string_operations': '字符串操作',
            'hash_operations': '哈希运算',
            'stack_operations': '栈操作',
            'property_access': '属性访问',
            'others': '其他'
        }
        
        priority_levels = []
        
        for i, (category, total_time, functions) in enumerate(category_stats[:6]):  # 只显示前6个
            if total_time < 1.0:
                continue
                
            category_name = category_names.get(category, category)
            
            # 确定优先级
            if total_time > 20:
                priority = "🚨极高"
                priority_levels.append((category_name, total_time))
            elif total_time > 10:
                priority = "⚠️高"
                priority_levels.append((category_name, total_time))
            elif total_time > 5:
                priority = "📋中"
            else:
                priority = "📝低"
                
            report_lines.append(f"{i+1}. {category_name}: {total_time:.1f}% {priority}")
            
            # 显示该类别的top函数
            top_funcs = sorted(functions, key=lambda x: x.time_percent, reverse=True)[:2]
            for func in top_funcs:
                report_lines.append(f"   └─ {func.name}: {func.time_percent:.2f}%")
                
        # 优化建议
        report_lines.append("")
        report_lines.append("🎯 优化建议 (按紧急程度):")
        
        suggestion_count = 0
        for category, time_percent in priority_levels[:3]:  # 只显示前3个最重要的
            if category == 'VM执行引擎':
                report_lines.append(f"🚨 {category} ({time_percent:.1f}%)")
                report_lines.append("   • 优化指令调度器和字节码执行")
                report_lines.append("   • 考虑指令合并和预取优化")
                suggestion_count += 3
                
            elif category == '参数处理':
                report_lines.append(f"⚠️  {category} ({time_percent:.1f}%)")
                report_lines.append("   • 实现参数缓存机制")
                report_lines.append("   • 优化参数传递和更新流程")
                suggestion_count += 3
                
            elif category == '内存管理':
                report_lines.append(f"📋 {category} ({time_percent:.1f}%)")
                report_lines.append("   • 实现对象池复用机制")
                report_lines.append("   • 优化内存分配策略")
                suggestion_count += 3
                
            elif category == '对象操作':
                report_lines.append(f"📋 {category} ({time_percent:.1f}%)")
                report_lines.append("   • 优化对象查找和属性访问")
                suggestion_count += 2
                
            elif category == '哈希运算':
                report_lines.append(f"📝 {category} ({time_percent:.1f}%)")
                report_lines.append("   • 使用更快的哈希算法")
                suggestion_count += 2
                
            if suggestion_count > 15:  # 限制建议数量
                break
                
        # Top 热点函数（只显示前8个）
        report_lines.append("")
        report_lines.append("📈 Top 8 热点函数:")
        
        for i, func in enumerate(self.functions[:8]):
            report_lines.append(f"{i+1}. {func.name}: {func.time_percent:.2f}% ({func.self_seconds:.3f}s)")
            
        # 总结
        report_lines.append("")
        report_lines.append("💡 关键优化点:")
        
        if priority_levels:
            top_priority = priority_levels[0]
            report_lines.append(f"1. 最紧急: {top_priority[0]} ({top_priority[1]:.1f}%)")
            
            if len(priority_levels) > 1:
                second_priority = priority_levels[1]
                report_lines.append(f"2. 次紧急: {second_priority[0]} ({second_priority[1]:.1f}%)")
                
        report_lines.append("")
        report_lines.append("🔧 优化顺序建议:")
        report_lines.append("1. 优化占用时间最多的类别")
        report_lines.append("2. 关注单个函数时间占比>5%的热点")
        report_lines.append("3. 考虑算法级别优化")
        report_lines.append("4. 实施后重新测试验证")
        
        report_lines.append("")
        report_lines.append("=" * 60)
        
        return '\n'.join(report_lines)

def main():
    """主函数"""
    parser = argparse.ArgumentParser(description='分析PikaPython性能热点数据')
    parser.add_argument('profile_file', help='gprof输出文件路径')
    parser.add_argument('-o', '--output', help='输出报告文件路径')
    parser.add_argument('-v', '--verbose', action='store_true', help='详细输出')
    
    args = parser.parse_args()
    
    # 创建分析器
    analyzer = ProfileAnalyzer()
    
    # 解析数据
    if not analyzer.parse_gprof_output(args.profile_file):
        sys.exit(1)
        
    # 生成报告
    report = analyzer.generate_report()
    
    # 输出报告
    if args.output:
        try:
            with open(args.output, 'w', encoding='utf-8') as f:
                f.write(report)
            print(f"报告已保存到: {args.output}")
        except Exception as e:
            print(f"Error: 保存报告失败 - {e}")
            sys.exit(1)
    else:
        print(report)
        
    if args.verbose:
        print(f"\n解析了 {len(analyzer.functions)} 个函数")

if __name__ == '__main__':
    main()