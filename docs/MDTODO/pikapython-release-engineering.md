# PikaPython 发布工程

- 发布版本遵循 SemVer，版本元数据、包索引、Git tag 和 GitHub Release 必须相互一致。
- Linux 核心验证默认使用仓库 `pikadev` Docker 镜像并显式启用 `PIKA_CONFIG_ENABLE`。
- runtime/VM 正确性和性能优先于 parser 性能；发布候选不得扩大语法范围或改变大架构。
- LVGL、网络及其他非核心依赖可从核心门禁隔离，但被发布包必须完成独立构建或 smoke。
- 性能结论必须来自同环境重复样本；RAM/Flash 以 Release 或目标固件等价构建核算总体净变化。

## R1 [in_progress]

按已调查的发布链路准备并发布 PikaPython v1.14.0：先建立 RC，修复或裁决语法/runtime/REPL 发布阻断项，改进现有发布工具与版本一致性检查，发布 core、PikaStdLib、threading、requests、mqtt、time 对应版本；在不改变大架构、资源总体不膨胀的前提下完成 Docker 多配置、Valgrind、benchmark、包/BSP smoke，最终创建不可移动的 tag、GitHub Release 并做全新安装验证，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1_Task_Report.md)。

### R1.1 [completed]

冻结 origin/master 发布基线，核对 v1.13.4 至候选提交的版本源、包变更、GitHub Release、Actions 和开放 issue，形成 v1.14.0 范围及 blocker 裁决，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.1_Task_Report.md)。

### R1.2 [completed]

修复 release_helper.py 的非 SemVer 进位和 packages.toml 全表格式抖动，为版本解析、patch 递增和定点包索引更新补自动测试；依赖 R1.1，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.2_Task_Report.md)。

### R1.3 [completed]

将 release_diff.py 改为不 checkout 工作区的只读比较，并增加 release-check 统一校验运行时版本、version_config、CMSIS Pack、包索引 commit 和 tag 一致性；依赖 R1.2，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.3_Task_Report.md)。

### R1.4 [in_progress]

复验并收口 Issue #367、#368、#369：已修复项补证据后关闭，仍复现的现有语法子集误拒绝作为 RC blocker 最小修复并回归；依赖 R1.1，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.4_Task_Report.md)。

#### R1.4.1 [completed]

补齐并修复 Issue #368 剩余的 handler 内再次抛出到外层及父类捕获子类时 alias 保留原始异常实例语义；只在现有 typed except 子集和 VM 错误状态框架内最小处理，不改变异常/字节码大架构，完成 default/minimal 回归与资源核算，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.4.1_Task_Report.md)。
#### R1.4.2 [in_progress]

第二阶段回收 R1.4.1 异常正确性修复的 Flash 增量：以同配置 Release A/B 为准，在不改变 typed except、嵌套传播和 alias 实际类型合同、不降低 VM/runtime 性能且不增加 RAM 的前提下，回收 default 328 B、minimal 16 B text；允许由其他低风险位置补偿，分别记录原增量、补偿收益和最终净变化，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.4.2_Task_Report.md)。
### R1.5

修复 Issue #372 的 optimize_speed runtime 缓存受 GTest 注册和分配布局影响问题，补不依赖偶然布局的回归并保持资源总体不膨胀；依赖 R1.1，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.5_Task_Report.md)。

### R1.6

修复 Issue #371 的 REPL 日志固定索引测试不稳定，恢复默认 TEST 与 VALGRIND 覆盖并验证多配置合同；依赖 R1.1，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.6_Task_Report.md)。

### R1.7

改进 Issue #373 的 benchmark 导出，保留重复样本聚合统计并支持可信的同环境版本 A/B 判定，不增加目标固件资源；依赖 R1.1，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.7_Task_Report.md)。

### R1.8

在 blocker 关闭后同步 v1.14.0 版本元数据和内容提交，定点更新 pikascript-core v1.14.0、PikaStdLib v1.14.0、threading v0.1.0、requests v1.0.4、mqtt v0.1.2、time v0.2.3 包索引；依赖 R1.3-R1.7，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.8_Task_Report.md)。

### R1.9

生成 RC 候选，在 pikadev Docker 中完成 default、minimal、optimize_speed、pool、float、定向与必要全量测试、Valgrind、fatal watchdog、Release 资源和重复 benchmark，并完成被发布包及至少一个 Cortex-M BSP/CMSIS smoke；依赖 R1.8，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.9_Task_Report.md)。

### R1.10

审核 RC 证据与 release-check 输出，创建 annotated v1.14.0 tag 和 GitHub Release，从全新 clone 及包管理器完成发布后验证；失败时不移动 tag，改走后续 patch；依赖 R1.9，完成任务后将详细报告写入[任务报告](./details/pikapython-release-engineering/R1.10_Task_Report.md)。
