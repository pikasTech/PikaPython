# PikaPython 运行时效率

- 范围：
  - 跟踪 PikaPython 内核在不改变 VM、对象模型、字节码和解析器总体架构前提下的性能优化。
  - 优先处理已有 GitHub issue 暴露的热路径和资源占用问题。
- 资源约束：
  - 优先采用不增加常驻 RAM、不扩大对象结构的改动。
  - Release 配置的 `text`、`data`、`bss` 原则上不得增长；无法避免时不纳入本轮实现。
  - 不启用现有不安全的链表 move-to-front 缓存，不引入常驻哈希表或大栈数组。
- 上游仓库：<https://github.com/pikasTech/PikaPython>


## R1 [completed]

在不改变 PikaPython 大架构的情况下执行一轮性能优化，优先结合现有 [#322](https://github.com/pikasTech/PikaPython/issues/322)、[#356](https://github.com/pikasTech/PikaPython/issues/356)、[#366](https://github.com/pikasTech/PikaPython/issues/366)，尽量不增加 RAM 和 Flash 占用，并以对应单元测试、Linux 回归、性能及资源数据验收，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R1_Task_Report.md)。

### R1.1 [completed]

建立 Release 性能、text/data/bss、分配次数与峰值内存基线，明确可重复的测量入口，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R1.1_Task_Report.md)。

### R1.2 [completed]

针对 #322 优化 dict/list 链表遍历和重复查找，保持容器结构、顺序语义及对象大小不变，并补定向单元测试，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R1.2_Task_Report.md)。

### R1.3 [completed]

优化函数调用参数检查和临时分配，保持栈格式及参数语义不变，不引入常驻缓存或大栈数组，并补边界测试，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R1.3_Task_Report.md)。

### R1.4 [completed]

在 event 未启用时编译期消除 VM 空 yield 固定开销，保持启用 event 时的调度语义不变，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R1.4_Task_Report.md)。

### R1.5 [completed]

运行 Linux 多配置全量回归、性能基准和资源占用对比，任何 RAM 或 Flash 增长项退出本轮，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R1.5_Task_Report.md)。

### R1.6 [completed]

审核修改边界、剩余风险与收益证据，完成任务报告，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R1.6_Task_Report.md)。

## R2 [completed]

继续执行不改变架构且不增加 RAM/Flash 的性能改进，优先消除函数调用和关键字处理剩余的重复扫描，并与 [语法兼容 R2](../MDTODO/pikapython-syntax-compatibility.md) 共用测试和资源门槛，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R2_Task_Report.md)。

### R2.1 [completed]

基于现有 benchmark 和源码剖析关键字名称解析、参数签名拆分与容器访问的剩余重复工作，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R2.1_Task_Report.md)。

### R2.2 [completed]

实现一项不增加常驻 RAM、不扩大对象结构且 Release text/data/bss 不增长的局部性能优化，并补测试，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R2.2_Task_Report.md)。

### R2.3 [completed]

与语法兼容 R2 联合运行交替 benchmark、多配置回归和资源对比，收益不成立则撤出实现，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R2.3_Task_Report.md)。

### R2.4 [completed]

审核第二轮性能变更并完成报告，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R2.4_Task_Report.md)。

## R3 [completed]

基于现有 benchmark、PikaMemInfo 和 Linux perf 证据定位 VM/runtime CPU 热点后再做一轮性能优化：优先使用仓库自带测量工具，不改变 VM/对象模型/字节码大架构，不增加常驻 RAM；没有可重复热点证据的候选不得修改，完成后记录热点占比、优化前后 A/B、分配与 default/minimal 资源证据，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R3_Task_Report.md)。

## R5 [completed]

继续在现有 PikaPython 语法子集内批量探测并修复语法、runtime 和 VM bug 及 Python 3 不兼容行为：不改变 VM/对象模型/字节码大架构，不扩大语法范围，优先修复可稳定复现的问题；每批加入对应单元测试，完成 default/minimal 回归及 text/data/bss、分配和堆残留对比，确保资源占用总体不膨胀，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R5_Task_Report.md)。

### R5.1 [completed]

语法与 Python 3 兼容性批量探测：对已有子集的表达式、控制流、函数参数和异常语法建立合法/非法邻接样例，记录解析期错误与运行期错误，修复稳定复现且不扩大语法范围的问题并补回归测试，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R5.1_Task_Report.md)。

### R5.2 [completed]

异常可见性与 runtime 状态批量探测：覆盖类型错误、名称错误、除零、不可调用对象、迭代器和错误后的下一次合法调用，确保错误可捕获、不会静默执行或崩溃，并验证堆无残留，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R5.2_Task_Report.md)。

### R5.3 [completed]

VM 参数绑定与栈恢复批量探测：覆盖位置参数、默认参数、关键字参数、重复参数、多余参数、方法调用和较多参数，检查报错后 VM 栈及对象状态，修复可复现问题并补单元测试，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R5.3_Task_Report.md)。

### R5.4 [completed]

多配置验证与资源门禁：运行 default/minimal 定向及全量 Linux 回归，比较 text/data/bss、分配次数和峰值/当前堆；发现局部资源增加时按整体预算评估并记录补偿或退出本批，完成任务后将详细报告写入[任务报告](./details/pikapython-runtime-efficiency/R5.4_Task_Report.md)。
