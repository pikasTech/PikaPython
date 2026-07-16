# PikaPython 语法兼容性

- 范围：跟踪 PikaPython 现有语法问题、Python 3 子集兼容修复及 Linux 单元测试回归。
- 本轮约束：优先处理已有 GitHub issue；不改变解释器架构，不主动扩大语法范围，仅接受必要的小范围语法修正。
- 上游仓库：<https://github.com/pikasTech/PikaPython>


## R1 [completed]

拉取 PikaPython 到 `/root/PikaPython` 并加入常用目录，跑通 Linux 端编译测试；优先处理已有 GitHub 语法 issue，找出语法支持问题和与 Python 3 不兼容的语法，在不扩大语法范围（允许必要小变更）、不改变架构的前提下修复一轮，并补齐对应单元测试，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R1_Task_Report.md)。

### R1.1 [completed]

确认仓库、常用目录与 Linux 测试入口，建立干净基线，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R1.1_Task_Report.md)。

### R1.2 [completed]

检索并完整阅读已有语法 issue，复现 `#347`、`#348`，并有界复测 `#322`，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R1.2_Task_Report.md)。

### R1.3 [completed]

定位参数绑定与关键字参数的 Python 3 兼容差异，形成不改变架构的最小修复方案，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R1.3_Task_Report.md)。

### R1.4 [completed]

实现一轮语法兼容修复，并为每个行为补充独立 Linux 单元测试，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R1.4_Task_Report.md)。

### R1.5 [completed]

运行定向测试与 Linux 全量语法配置回归，记录剩余已知问题，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R1.5_Task_Report.md)。

### R1.6 [completed]

审核变更边界、临时产物和最终差异，完成任务报告，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R1.6_Task_Report.md)。

### R1.7 [completed]

评估 PikaPython 内核设计水平与效率，区分 MCU 约束下的合理取舍和确实存在的低效、脆弱实现，并给出源码证据，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R1.7_Task_Report.md)。

## R2 [completed]

继续执行不改变大架构、不增大 RAM/Flash 占用的语法兼容与错误可见性修复，优先处理已有 [#349](https://github.com/pikasTech/PikaPython/issues/349) 和 [#366](https://github.com/pikasTech/PikaPython/issues/366)，补对应单元测试并与运行时效率第二轮交叉验收，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R2_Task_Report.md)。

### R2.1 [completed]

完整阅读并复现 #349 与 #366，区分语法语义、栈边界和错误可见性根因，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R2.1_Task_Report.md)。

### R2.2 [completed]

在不扩大语法范围的前提下修复 #349 关键字参数缺失或混用时的 Python 3 兼容错误，并补单元测试，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R2.2_Task_Report.md)。

### R2.3 [completed]

修复 #366 参数过多时 VM 栈边界检查漏报和越界写前可见性，不改变栈结构或增加缓冲，并补边界测试，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R2.3_Task_Report.md)。

### R2.4 [completed]

运行隔离网络后的 Linux 多配置回归及默认/minimal 资源尺寸对比，资源增长项退出本轮，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R2.4_Task_Report.md)。

### R2.5 [completed]

审核第二轮语法与错误可见性变更并完成报告，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R2.5_Task_Report.md)。

## R3 [completed]

继续跟踪并修复 [#366](https://github.com/pikasTech/PikaPython/issues/366) 的 20+ 参数调用死机及非法语法死机：区分合法变长参数与固定槽位溢出，保证返回可捕获错误，不扩大语法或架构且不增加 RAM/Flash，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R3_Task_Report.md)。

### R3.1 [completed]

复现 20+ 实参的合法 *args 调用、固定参数槽位溢出和多类非法语法，明确各自预期，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R3.1_Task_Report.md)。

### R3.2 [completed]

补齐 20+ 实参与非法语法单元测试，并修复发现的写前检查或错误传播遗漏，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R3.2_Task_Report.md)。

### R3.3 [completed]

在 NC01 隔离网络、LVGL 和非语法依赖，运行 default/minimal 及相关配置回归和资源对比，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R3.3_Task_Report.md)。

### R3.4 [completed]

审核变更、更新 #366 证据并完成任务报告，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R3.4_Task_Report.md)。
