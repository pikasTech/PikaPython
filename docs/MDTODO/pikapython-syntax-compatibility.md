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

## R11 [completed]

再测试一轮并确认至少 20 个独立语法处理错误，同时覆盖解析阶段和运行阶段：保留 R10 剩余 9 类解析问题，新增运行时崩溃、错误绑定、错误结果或异常不可见问题；每项记录 CPython 3 预期、PikaPython 实际阶段和最小复现，本轮只探测、分类和固化测试，不混入修复，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R11_Task_Report.md)。

### R11.1 [completed]

建立至少 20 项解析/运行时差分候选矩阵和独立问题计数口径，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R11.1_Task_Report.md)。

### R11.2 [completed]

批量验证解析阶段错误接受、错误码丢失和崩溃，确认可重复问题，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R11.2_Task_Report.md)。

### R11.3 [completed]

批量验证已解析语法的运行时错误绑定、错误结果、异常不可见和崩溃，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R11.3_Task_Report.md)。

### R11.4 [completed]

汇总至少 20 个独立问题、最小复现、阶段和后续修复优先级，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R11.4_Task_Report.md)。

## R12 [completed]

修复 R11 确认的 20 个解析期/运行期语法处理错误，优先消除空 import 两种形式的 SIGSEGV，再按共同 parser block/token 根因和 runtime 语义合同分批处理；保持非架构改动，正确性阶段允许量化临时资源增量，资源与性能回收独立执行且 VM/runtime 性能优先，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R12_Task_Report.md)。

### R12.1 [completed]

P0 修复空 import 与 from-import 缺名称在 Suger_import 路径的解析崩溃并补回归，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R12.1_Task_Report.md)。

### R12.2 [completed]

修复孤立 block clause、作用域外控制语句、缺逗号及附加 parser 错误接受，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R12.2_Task_Report.md)。

#### R12.2.1 [completed]

修复孤立 block clause、作用域外控制语句以及 list/tuple 数值项缺逗号，并补合法上下文回归，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R12.2.1_Task_Report.md)。
#### R12.2.2 [completed]

修复附加候选中的非法声明、参数形态、赋值/删除/循环目标和注解目标，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R12.2.2_Task_Report.md)。
#### R12.2.3 [completed]

修复附加候选中的非法 lambda、条件表达式、slice、await 与 async for，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R12.2.3_Task_Report.md)。
### R12.3 [completed]

修复默认参数、短路、运算符结合、推导式作用域、解包和局部变量绑定的运行语义，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R12.3_Task_Report.md)。

#### R12.3.1 [completed]

修复 and/or 短路求值与幂运算右结合语义，并补副作用回归，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R12.3.1_Task_Report.md)。
#### R12.3.2 [completed]

修复默认参数定义时求值和未绑定局部变量错误合同，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R12.3.2_Task_Report.md)。
#### R12.3.3 [completed]

修复推导式作用域、星号解包及定长解包数量/错误类型合同，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R12.3.3_Task_Report.md)。
### R12.4 [completed]

运行 default/minimal 隔离回归并独立量化、回收 RAM/Flash 与性能变化，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R12.4_Task_Report.md)。

## R13 [completed]

继续在 PikaPython 已有语法子集范围内批量探测并修复一批解析期语法问题和运行期语义、崩溃或错误可见性问题；不扩大语法能力、不改变 parser/ASM/VM 大架构，每项修复配单元测试，正确性与资源优化分阶段且 VM/runtime 性能优先，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R13_Task_Report.md)。

### R13.1 [completed]

批量差分探测当前子集的解析期与运行期候选，去重 R10-R12 已覆盖语料并固定最小复现和 CPython 3 预期，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R13.1_Task_Report.md)。

### R13.2 [completed]

按共同根因修复已确认问题并为每项行为补充 Linux 单元测试，不扩大语法范围或引入新 VM 指令，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R13.2_Task_Report.md)。

### R13.3 [completed]

同步 core 后隔离 LVGL、网络和非语法组件，运行 default/minimal 定向与必要全量回归，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R13.3_Task_Report.md)。

### R13.4 [completed]

独立量化并优化 RAM、Flash、分配次数和 runtime 影响，保持语义合同且 runtime 性能优先，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R13.4_Task_Report.md)。

## R14 [completed]

继续在 PikaPython 已有语法子集范围内探测并修复一批 runtime 正确性和 VM 热路径性能问题，优先闭环 [#347](https://github.com/pikasTech/PikaPython/issues/347)、[#348](https://github.com/pikasTech/PikaPython/issues/348)、[#349](https://github.com/pikasTech/PikaPython/issues/349) 的函数默认参数、变长参数和 keyword 绑定问题；不扩大语法、不改变 VM 大架构，runtime 性能优先并控制 RAM/Flash，完成任务后将详细报告写入任务报告，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14_Task_Report.md)。

### R14.1 [completed]

复现 #347-#349 并建立普通调用、默认参数、星号解包、keyword 绑定和循环控制的语义、分配与 runtime 基线，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.1_Task_Report.md)。

### R14.2 [completed]

按共同根因修复已确认的运行期错误、崩溃或错误不可见问题，补充 Linux 单元测试并保持现有语法范围，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.2_Task_Report.md)。

### R14.3 [completed]

在语义合同固定后优化函数调用和参数绑定 VM 热路径，优先消除重复扫描、分配和字典操作，并用同机 A/B 验证，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.3_Task_Report.md)。

### R14.4 [completed]

同步 core 后运行 default/minimal 定向与必要全量回归，量化 text/data/bss、分配次数和 runtime，更新 issue 与总报告，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.4_Task_Report.md)。

### R14.5 [completed]

继续解决 [#348](https://github.com/pikasTech/PikaPython/issues/348) 的 VM 性能债务：优化 kwargs 名称恢复中的常量池重复扫描，在不扩大语法、不改变 VM 大架构且不增加常驻 RAM 的前提下完成同机 A/B、default/minimal 资源和语义回归，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.5_Task_Report.md)。

### R14.6 [completed]

修复 Linux Action 暴露的默认参数名被误记为未绑定局部变量问题，覆盖 False 默认值和方法参数，完成隔离 Linux 回归并确认不扩大语法或 VM 架构，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.6_Task_Report.md)。

### R14.7 [completed]

修复 Linux Action 暴露的逻辑组合比较被链式比较门禁错误拒绝问题，保持非法链式比较稳定拒绝并恢复已有 and/or 条件子集，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.7_Task_Report.md)。

### R14.8 [completed]

修复 Linux Action 的 VALGRIND 未复用网络隔离和超时边界问题，使内存检查与 core TEST 使用同一默认过滤合同并保留显式 filter 覆盖，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.8_Task_Report.md)。

### R14.9 [completed]

修复 benchmark 结果发布因 Linux 构建覆盖 tracked 生成物而无法切换 gh-pages 的 Action 失败，只恢复 tracked 工作树并保留 benchmark 输出，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.9_Task_Report.md)。

### R14.10 [completed]

隔离 Linux core Action 中非语法的 `threading.lock_rlock` 并发竞态崩溃，保持 TEST 与 VALGRIND 默认过滤合同一致并保留显式覆盖，根因由 [Issue #370](https://github.com/pikasTech/PikaPython/issues/370) 跟踪，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.10_Task_Report.md)。

### R14.11 [completed]

隔离 Linux default 配置下依赖 `log_buff` 固定索引的 REPL 成组不稳定测试，保持显式 filter 可复现并由 [Issue #371](https://github.com/pikasTech/PikaPython/issues/371) 跟踪配置感知修复，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.11_Task_Report.md)。

### R14.12 [completed]

修复 [Issue #370](https://github.com/pikasTech/PikaPython/issues/370) 的 Lock 非法双 release 未定义行为和错误不可见问题，拆分 Lock/RLock 单元测试、验证错误后恢复，稳定后恢复 Linux TEST 与 VALGRIND 默认执行，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.12_Task_Report.md)。

### R14.13

跟踪并修复 [Issue #372](https://github.com/pikasTech/PikaPython/issues/372) 的 `optimize_speed` runtime 缓存受 GTest 注册数量和启动分配布局影响问题，定位缓存键、失效与对象生命周期根因，在不改变大架构且资源总体不膨胀的前提下增加稳定回归，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.13_Task_Report.md)。

### R14.14 [completed]

增加代表嵌入式对象、属性、方法、分支和数值状态更新的复合 runtime benchmark；先用多轮统计与 gprof 定位 VM 热点，再实施不改变大架构的性能优化，并以交错 A/B、控制基准、分配次数和 text/data/bss 验证总体资源不膨胀，完成任务后将详细报告写入[任务报告](./details/pikapython-syntax-compatibility/R14.14_Task_Report.md)。
