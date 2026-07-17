# R3 VM/runtime 热点优化报告

## 范围与约束

- 仅优化既有对象写入和函数调用路径，不改变 VM、对象模型或字节码架构。
- 优先使用仓库自带的 `port/linux/benchmark`、`port/linux/performance`、PikaMemInfo、Google Benchmark 和 GTest。
- LVGL 从隔离测量树中移除；网络类结果不作为本轮性能结论。
- 系统未安装 Linux `perf`，且 `perf_event_paranoid=3`，因此采用仓库已有 `-pg` 配置和 gprof 采样。

## 热点证据

将仓库自带 performance 负载调整为 200 万次无输出 Python 函数调用，避免默认 50 万次 `print` 的终端 I/O 污染。基线结果：

- 总耗时：`5.695956s`。
- 分配次数：`12008617`，缓存命中：`6007512`。
- `_obj_getObjWithKeepDeepth`：gprof self `0.98s`，占 `21.21%`，为第一自耗时热点。
- `__pikaVM_runByteCodeFrameWithState`：占 `20.13%`。
- `obj_setArg_noCopy`：占 `3.90%`。

源码检查确认，`_obj_setArg()` 对无点号的普通变量名仍调用 `obj_getHostObj()`，产生路径复制、token 统计和零次对象遍历开销。

## 实现

- default/full 配置下，单段变量名直接以当前对象为 host。
- 仅在路径包含点号时调用原有 `obj_getHostObj()` 和 `strPointToLastToken()`。
- `PIKA_NANO_ENABLE` 下保留原实现，避免 minimal/nano 的 Flash 增长。
- 不增加常驻 RAM、动态分配或运行时状态。
- 新增对象测试，同时覆盖单段快速路径、嵌套通用路径和释放后零堆残留。

## 性能结果

同机、同隔离源码树、同负载的 performance/gprof A/B：

- 基线：`5.695956s`。
- 优化：`4.835299s`。
- 改善：约 `15.1%`。
- 分配次数：两者均为 `12008617`，无新增分配。
- 热点 self：`_obj_getObjWithKeepDeepth` 从 `0.98s` 降到 `0.76s`。

Google Benchmark，`--benchmark_min_time=0.5s`：

| 基准 | 基线 | 优化 | 判定 |
| --- | ---: | ---: | --- |
| `while_loop_10000` | `6.91ms` | `6.96ms` | 无可信变化 |
| `function_call_1000` | `8.60ms` | `8.35ms` | 约 2.9%，低于该组 CV，仅作方向性证据 |
| `function_call_default_1000` | `2.25ms` | `2.07ms` | 约 8%，方向明确但 CV 约 6%-9% |

主要收益结论以 200 万次长采样的 `15.1%` 为准；短 Google Benchmark 不夸大低于噪声的差异。

## 正确性与资源

- default 对象及 runtime/parser 定向回归：`55/55` 通过。
- 新增 `object_test.set_arg_simple_and_nested_path`，对象测试总计 `18/18` 通过。
- minimal 隔离全量：`226/226` 通过。
- default：`text 1081989 / data 125784 / bss 24338`；相对既有基线 text 减少 `64B`，data/bss 不变。
- minimal：`text 1029603 / data 47180 / bss 23074`；与既有基线完全一致。
- 隔离源码树的 `pika_config.h` 已恢复为 default 配置。
- `git diff --check` 通过。

## 结论

本轮先由仓库自带工具确认热点，再消除普通变量写入中的无效路径解析。长负载函数调用改善约 `15.1%`，分配和常驻 RAM 不变，default Flash 略降，minimal 资源完全不变，未改变架构或语义。
