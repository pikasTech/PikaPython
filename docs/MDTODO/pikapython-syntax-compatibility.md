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

## R4 [completed]

继续优先处理已有 [#356](https://github.com/pikasTech/PikaPython/issues/356)：复现 nano 模式下 main.py 顶层函数定义后引用偶发 NameError，区分当前语法/符号注册问题与旧版本或固件裁剪问题；只做非架构、非扩语法且不增加 RAM/Flash 的修复，并补 Linux 单元测试，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R4_Task_Report.md)。

### R4.1 [completed]

完整复现 #356 的三个顶层函数定义、直接调用和函数对象赋值，在 default/minimal 下确认当前行为与定义顺序影响，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R4.1_Task_Report.md)。

### R4.2 [completed]

定位 nano 模式符号注册、函数对象引用和编译资产路径，若仍存在问题则实现最小修复并补单元测试，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R4.2_Task_Report.md)。

### R4.3 [completed]

运行隔离后的 default/minimal 与相关配置回归，比较 R3 资源门槛，任何 RAM/Flash 增长项退出，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R4.3_Task_Report.md)。

### R4.4 [completed]

审核变更边界、更新 #356 证据并完成报告，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R4.4_Task_Report.md)。

## R5 [completed]

优先处理已有 [#328](https://github.com/pikasTech/PikaPython/issues/328)：调查带 keyword 的 C 模块 API 在调用时不加 keyword 的行为，按 Python 3 边界覆盖原生方法的位置、混合、全关键字及非法位置调用；只做非架构、非扩语法且不增加 RAM/Flash 的修复，并补 Linux 单元测试，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R5_Task_Report.md)。

### R5.1 [completed]

完整读取 #328 并从 C 模块绑定合同复现位置、混合、全关键字及非法调用行为，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R5.1_Task_Report.md)。

### R5.2 [completed]

定位原生 C 方法 keyword 参数绑定与 Python 3 的差异，若可复现则实现不增资源的最小修复并补测试，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R5.2_Task_Report.md)。

### R5.3 [completed]

运行隔离后的 default/minimal 和相关配置回归，比较 R3 资源门槛，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R5.3_Task_Report.md)。

### R5.4 [completed]

审核变更边界、更新 #328 证据并完成任务报告，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R5.4_Task_Report.md)。

## R6 [completed]

批量探测并修复下一轮语法错误：正确性阶段允许量化的临时资源增量，性能与占用回收转入独立阶段，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R6_Task_Report.md)。

### R6.1 [completed]

建立表驱动语法探针，批量比较非法函数声明、调用参数、集合和表达式的编译结果，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R6.1_Task_Report.md)。

### R6.2 [completed]

定位本批错误接受、错误传播或崩溃根因，实现非架构且不扩语法的最小修复并补回归，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R6.2_Task_Report.md)。

### R6.3 [completed]

运行 default/minimal 隔离全量并量化临时资源增量，将资源债务移交独立优化阶段，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R6.3_Task_Report.md)。

### R6.4 [completed]

审核本批边界、登记未解决问题并完成报告，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R6.4_Task_Report.md)。

## R7 [completed]

保持 R6 语义合同和 VM/runtime 性能优先级，独立回收 default/minimal 资源占用；parser 微基准仅作辅助证据，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R7_Task_Report.md)。

### R7.1 [completed]

建立 R6 的 default/minimal 资源、分配次数和辅助 parser 编译基线，不把 parser 微基准置于 VM/runtime 性能之上，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R7.1_Task_Report.md)。

### R7.2 [completed]

在不改变 15 项错误合同的前提下消除重复扫描和错误路径冗余，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R7.2_Task_Report.md)。

### R7.3 [completed]

运行相同语义回归、性能基准和两档资源验收，记录未回收差额，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R7.3_Task_Report.md)。

## R8 [completed]

优先处理已有 [#367](https://github.com/pikasTech/PikaPython/issues/367) 的五类 Python 3 语义语法错误，在不改变架构和 VM/runtime 热路径的前提下批量修复并补单元测试，正确性阶段允许量化的临时资源增量，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R8_Task_Report.md)。

### R8.1 [completed]

建立 #367 五类非法语法和相邻合法语法的表驱动探针，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R8.1_Task_Report.md)。

### R8.2 [completed]

定位函数声明、调用参数和字典 AST 的错误接受路径，实现非架构最小修复，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R8.2_Task_Report.md)。

### R8.3 [completed]

运行 default/minimal 隔离回归并量化资源变化，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R8.3_Task_Report.md)。

### R8.4 [completed]

更新 #367 证据、审核边界并完成本轮报告，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R8.4_Task_Report.md)。

## R9 [completed]

保持 R8 语义与崩溃合同，独立回收 default 1016 字节和 minimal 296 字节临时 text 增量，VM/runtime 性能优先于 parser 性能，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R9_Task_Report.md)。

### R9.1 [completed]

定位 R8 的 default/minimal 对象级尺寸增量和 parser 成功路径额外状态，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R9.1_Task_Report.md)。

### R9.2 [completed]

在不改变错误合同和 VM/runtime 热路径的前提下做局部尺寸与临时 RAM 回收，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R9.2_Task_Report.md)。

### R9.3 [completed]

运行相同定向、default/minimal 全量和资源验收，记录剩余差额，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R9.3_Task_Report.md)。

## R10 [completed]

批量探测下一轮 Python 3 非法赋值目标、控制流头部和容器/推导式分隔符；优先关联可读取的已有 issue，在不扩语法、不改架构的前提下修复错误接受、崩溃和错误不可见，并补表驱动测试，正确性与资源优化继续分阶段，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R10_Task_Report.md)。

### R10.1 [completed]

恢复 GitHub 可见性后筛选已有语法 issue，并建立 CPython 3 差分样例矩阵，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R10.1_Task_Report.md)。

### R10.2 [completed]

批量运行非法赋值目标、控制流头部和容器/推导式分隔符探针，定位错误接受或崩溃，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R10.2_Task_Report.md)。

### R10.3 [completed]

实现不扩语法、不改架构的本批最小正确性修复并补表驱动测试，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R10.3_Task_Report.md)。

### R10.4 [completed]

运行定向及 default/minimal 隔离回归，量化临时资源增量并移交独立优化阶段，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R10.4_Task_Report.md)。

## R11

跟踪 R10 剩余 9 类 Python 3 非法语法：orphan else/elif/except/finally、循环或函数外 break/continue/return、list/tuple 项缺逗号；GitHub 受控入口恢复后先检索并合并已有 issue，没有同类 issue 再创建，按 block 上下文与通用 token 邻接分批修复，不改变架构和 VM/runtime 热路径，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R11_Task_Report.md)。
