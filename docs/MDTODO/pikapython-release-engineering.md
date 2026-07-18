# PikaPython 发布工程

- 发布版本遵循 SemVer，版本元数据、包索引、Git tag 和 GitHub Release 必须相互一致。
- Linux 核心验证默认使用仓库 `pikadev` Docker 镜像并显式启用 `PIKA_CONFIG_ENABLE`。
- runtime/VM 正确性和性能优先于 parser 性能；发布候选不得扩大语法范围或改变大架构。
- LVGL、网络及其他非核心依赖可从核心门禁隔离，但被发布包必须完成独立构建或 smoke。
- 性能结论必须来自同环境重复样本；RAM/Flash 以 Release 或目标固件等价构建核算总体净变化。

## R1 [in_progress]

按已调查的发布链路直接发布 PikaPython v1.14.0：修复或裁决语法/runtime 发布问题，改进版本与包索引一致性工具，发布 core、PikaStdLib、threading、requests、mqtt、time 对应版本；按用户裁决跳过 RC，接受已量化的少量 Flash 增量，开发测试工具、环境、LVGL 与网络问题不阻断源码发布，最终创建不可移动 tag、GitHub Release 并做全新安装验证，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1_Task_Report.md)。

### R1.1 [completed]

冻结 origin/master 发布基线，核对 v1.13.4 至候选提交的版本源、包变更、GitHub Release、Actions 和开放 issue，形成 v1.14.0 范围及 blocker 裁决，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.1_Task_Report.md)。

### R1.2 [completed]

修复 release_helper.py 的非 SemVer 进位和 packages.toml 全表格式抖动，为版本解析、patch 递增和定点包索引更新补自动测试；依赖 R1.1，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.2_Task_Report.md)。

### R1.3 [completed]

将 release_diff.py 改为不 checkout 工作区的只读比较，并增加 release-check 统一校验运行时版本、version_config、CMSIS Pack、包索引 commit 和 tag 一致性；依赖 R1.2，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.3_Task_Report.md)。

### R1.4 [completed]

复验并收口 Issue #367、#368、#369：已修复项补证据后关闭，仍复现的现有语法子集误拒绝作为 RC blocker 最小修复并回归；依赖 R1.1，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.4_Task_Report.md)。

#### R1.4.1 [completed]

补齐并修复 Issue #368 剩余的 handler 内再次抛出到外层及父类捕获子类时 alias 保留原始异常实例语义；只在现有 typed except 子集和 VM 错误状态框架内最小处理，不改变异常/字节码大架构，完成 default/minimal 回归与资源核算，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.4.1_Task_Report.md)。
#### R1.4.2 [completed]

裁决 R1.4.1 异常正确性修复的 Flash 增量：default 增加 328 B、minimal 增加 16 B text，data/bss 不变；用户明确接受该少量增量，不再阻断 v1.14.0，后续可由其他低风险优化补偿且不得降低 runtime/VM 性能或增加 RAM，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.4.2_Task_Report.md)。

### R1.5 [completed]

裁决 Issue #372 不阻断本次 v1.14.0 源码发布：保持 Issue 开放并转入发布后修复，不把 optimize_speed 的开发测试布局问题作为当前源码 tag 门禁，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.5_Task_Report.md)。

### R1.6 [completed]

裁决 Issue #371 不阻断本次 v1.14.0 源码发布：保持 Issue 开放并转入发布后修复，不把 REPL 日志固定索引测试不稳定作为当前源码 tag 门禁，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.6_Task_Report.md)。

### R1.7 [completed]

裁决 Issue #373 不阻断本次 v1.14.0 源码发布：保持 Issue 开放并转入发布后改进，当前 Release 不宣称 Pages benchmark 的精确跨版本墙钟收益，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.7_Task_Report.md)。

### R1.8 [completed]

直接同步 v1.14.0 版本元数据和源码快照，定点更新 pikascript-core v1.14.0、PikaStdLib v1.14.0、threading v0.1.0、requests v1.0.4、mqtt v0.1.2、time v0.2.3 包索引；R1.3-R1.7 的发布门禁已按用户裁决收口，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.8_Task_Report.md)。

### R1.9

跳过 RC 阶段；对正式源码候选执行版本一致性、发布工具单测、核心 Linux 编译/定向回归和工作区清洁检查，开发测试工具、环境、LVGL 与网络问题不阻断源码 tag，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.9_Task_Report.md)。

### R1.10

审核直接发布候选与 release-check 输出，创建 annotated v1.14.0 tag 和 GitHub Release，从全新 clone 及包管理器完成发布后验证；失败时不移动 tag，改走后续 patch，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.10_Task_Report.md)。
