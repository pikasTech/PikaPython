# Prompt 纯策略增强计划：session_20250924_185457

**关联分析报告**: `20250924_session_185457_analysis.md`
**定位变化**：本版完全移除“新增工具脚本”思路，所有改进均通过 Prompt 内嵌自检、阶段化结构、显式元信息输出与自监控指令实现，不依赖外部新增工具。

---
## 1. 问题 → 纯 Prompt 化应对映射
| 问题编号 | 核心缺陷 | 以前拟用工具 | 现在改为 | 机制简介 |
| -------- | -------- | ------------ | -------- | -------- |
| Q1 | `.pyi` 紧凑语法触发 panic | validate_pyi | 内联 PYI_CHECK 块 | 让模型先生成“候选接口列表 + 规范化版本”，自比对差异再确认提交 |
| Q2 | 冗余 include | scan_includes | INCLUDE_MINIFY 块 | 列出现有 include → 与白名单行内对比 → 生成精简版 diff |
| Q3 | 幻觉符号 | scan_symbols | SYMBOL_POLICY 块 | 先产出所需符号表 + 标记来源（白名单/新增）+ 未知需 `[REQUEST_SYMBOL]` |
| Q4 | 形参与调用不匹配 | classify_failure | SIGNATURE_MATRIX 块 | 生成参数矩阵（函数名/参数/返回/调用样例）并自验一致性 |
| Q5 | None 语义退化 | 语义编码策略工具 | SEMANTIC_GUARD 块 | 列出所有潜在缺失值场景 + 选用编码策略 + 生成断言句式 |
| Q6 | Token 成本高 | trim_prompt_segments | CONTEXT_TRIM 规则 | 根据阶段输出 `[CONTEXT_TRIM] kept=... removed=...` 并后续只引用段 ID |
| Q7 | 错误分类被动 | classify_failure | FAILURE_CLASSIFIER 模式 | 捕获日志片段 → 输出 `[DIAG]` 分类（正则写入说明文字而非执行）|
| Q8 | 固定迭代数 | gen_perf_iter | PERF_ITER_HEURISTIC 块 | 通过暖身测量（脚本内嵌）指令化描述，不调用外部工具 |
| Q9 | 语义退化未标记 | 外部守卫 | DEGRADATION_TAG 规则 | 一旦使用常量占位或跳过断言，必须输出 `[WARN][DEGRADED] <原因>` |

---
## 2. Prompt 顶层结构重构（段 ID）
采用固定段号，调用时允许引用（仅使用 ID + 摘要）以缩短上下文：
1. S1 角色与总目标
2. S2 阶段流水（生成 / 预检 / 构建 / 运行 / 提取 / 总结）
3. S3 命名与路径规范
4. S4 接口与签名（含 PYI_CHECK 规则）
5. S5 C 实现最小化与 INCLUDE_MINIFY 规范
6. S6 测试脚本格式 + SIGNATURE_MATRIX 模板
7. S7 性能测试 & PERF_ITER_HEURISTIC
8. S8 调试与 FAILURE_CLASSIFIER 指南
9. S9 语义编码 & SEMANTIC_GUARD
10. S10 成本与 CONTEXT_TRIM 规则
11. S11 输出格式与 DEGRADATION_TAG

调用策略：首轮提供 S1~S11 全量；之后每轮在首行打印：
`[SEGMENTS] using=S1,S3,S4,S6,S9 delta=reason:"pre-build interface refinement"`

---
## 3. 内嵌自检模块设计（Prompt 片段）
### 3.1 PYI_CHECK 块
模型在真正写 `.pyi` 前输出：
```
[PYI_CHECK]
raw_functions:
- sum_list(self, nums:list)->int
- mean(self, nums:list)->float
normalized:
- def sum_list(self, nums: list) -> int
- def mean(self, nums: list) -> float
issues:
- line1: missing space after comma
decision: USE_NORMALIZED
[/PYI_CHECK]
```
然后才调用写文件工具；若 decision 不是 `USE_NORMALIZED`，禁止写入。

### 3.2 INCLUDE_MINIFY 块
```
[INCLUDE_MINIFY]
original: ["#include \"PikaObj.h\"", "#include \"pika_hal.h\""]
whitelist: ["PikaObj.h"]
remove: ["pika_hal.h"]
final: ["#include \"PikaObj.h\""]
[/INCLUDE_MINIFY]
```

### 3.3 SYMBOL_POLICY 块
```
[SYMBOL_POLICY]
allowed_core: newNormalObj(New_PikaObj), obj_getInt, obj_setFloat
requested: []
forbidden_detected: New_TinyObj
action: REPLACE_WITH newNormalObj(New_PikaObj)
[/SYMBOL_POLICY]
```

### 3.4 SIGNATURE_MATRIX 块
```
[SIGNATURE_MATRIX]
functions:
- name: sum_list  params: (self, nums:list) return:int  call: stats.sum_list(data)
- name: mean      params: (self, nums:list) return:float call: stats.mean(data)
cross_check: OK (all have nums, test script calls match)
[/SIGNATURE_MATRIX]
```

### 3.5 SEMANTIC_GUARD 块
```
[SEMANTIC_GUARD]
potential_missing: mean(empty), minimum(empty), maximum(empty), sliding_average(<window)
encoding_choice: NaN_fallback (no 0.0 placeholder)
test_assertions: mean -> isnan or value; sliding_average -> per-element compare/NaN check
[/SEMANTIC_GUARD]
```

### 3.6 FAILURE_CLASSIFIER 指令格式
出现构建/运行失败时：
```
[DIAG]
category=HEADER_MISSING trigger="fatal error: pika_hal.h" fix="remove unused include" confidence=0.92
root_type=BUILD
next_step=apply_INCLUDE_MINIFY_then_rebuild
[/DIAG]
```

### 3.7 PERF_ITER_HEURISTIC
在写测试脚本前输出：
```
[PERF_ITER_HEURISTIC]
warm_calls=200 target_time_window=0.4s per_call_estimate=3.2e-05s computed_iters=12500 clamp_applied?=no final_iters=12500
[/PERF_ITER_HEURISTIC]
```

### 3.8 CONTEXT_TRIM 规则
每当进入“构建成功后调试细化”阶段：
```
[CONTEXT_TRIM] kept=S1,S4,S6,S8,S9 removed=S2,S3,S5,S7,S10,S11 reason="post-initial-success minimize cost"
```
后续若再次需要被移除段内容，引用方式：`REF:S5.include_rule`。

### 3.9 DEGRADATION_TAG
如果使用硬编码常量或跳过断言：
```
[WARN][DEGRADED] reason="temporary constant return in mean" plan="replace after list access fix" allow_perf?=NO
```

---
## 4. 阶段式执行流程（纯 Prompt 描述）
1. 收到 Python 源后：输出 `[PLAN]` 概要 + `PYI_CHECK` → 写接口
2. 输出 `SYMBOL_POLICY` + `INCLUDE_MINIFY` → 写 C 文件（仅最小 include）
3. 输出 `SIGNATURE_MATRIX` + `SEMANTIC_GUARD` → 写测试脚本（功能断言 + 缺失值逻辑）
4. 构建 & 运行（一次）；失败则 `FAILURE_CLASSIFIER` → 精准单次修复，限制重试 ≤2
5. 成功后：`CONTEXT_TRIM`，进行性能脚本补丁（附 `PERF_ITER_HEURISTIC`）
6. 若出现语义退化：输出 `DEGRADATION_TAG` 并禁止性能段直至修复或显式确认可降级
7. 收尾输出 `[MODULE]` 块 + `[SUMMARY]`：列出 used_segments / trimmed_segments / degradation_flags

---
## 5. 统一输出格式补充
在最终 `[SUMMARY]` 内新增：
```
segments_used=S1,S2,S3,S4,S5,S6,S7,S8,S9,S10,S11
segments_trimmed=S2,S3,S5,S7,S10
degradation_flags=[]
semantic_encoding=NaN_fallback
perf_iters=12500
```

---
## 6. 样例串联（浓缩版）
```
[PYI_CHECK] ... [/PYI_CHECK]
[SYMBOL_POLICY] ... [/SYMBOL_POLICY]
[INCLUDE_MINIFY] ... [/INCLUDE_MINIFY]
[SIGNATURE_MATRIX] ... [/SIGNATURE_MATRIX]
[SEMANTIC_GUARD] ... [/SEMANTIC_GUARD]
(写文件 & 构建)
[DIAG] category=HEADER_MISSING ... [/DIAG]
... 修复后成功 ...
[PERF_ITER_HEURISTIC] ... [/PERF_ITER_HEURISTIC]
[CONTEXT_TRIM] kept=... removed=... [/CONTEXT_TRIM]
[SUMMARY] ... [/SUMMARY]
```

---
## 7. 质量与衡量指标（纯 Prompt 版）
| 指标 | 目标 | 说明 |
| ---- | ---- | ---- |
| 构建前失败触发率 | < 10% | 通过 PYI_CHECK + INCLUDE_MINIFY 降低 panic/缺头文件 |
| 幻觉符号发生率 | 0 | SYMBOL_POLICY 必须列出全部新增符号，否则拒绝写入 |
| 语义退化标记覆盖率 | 100% | 任一硬编码/断言跳过必须出现 DEGRADED 标记 |
| token 成本下降 | -30% | CONTEXT_TRIM 后阶段仅引用 ID |
| 每失败循环修复尝试次数 | ≤1 平均 | FAILURE_CLASSIFIER 精准分类减少盲试 |
| 缺失值错误断判次数 | 0 | SEMANTIC_GUARD 统一编码策略 |

---
## 8. 引导文字（可直接嵌入 core_task 尾部）
```
在任何写文件前，你必须：
1. 生成对应自检块（PYI_CHECK / SYMBOL_POLICY / INCLUDE_MINIFY / SIGNATURE_MATRIX / SEMANTIC_GUARD）。
2. 若自检块未标记 PASS 或 decision=USE_NORMALIZED/OK，禁止继续。
3. 构建失败时先输出 DIAG，不得直接试错修改。
4. 成功一次后立即执行 CONTEXT_TRIM，后续引用段落用 REF:Sx.key。
5. 若出现语义退化，输出 WARN DEGRaded 标签并暂缓性能测试。
```

---
## 9. 渐进引入顺序（Prompt 迭代而非工具开发）
Phase A（快速）：加入段编号 + PYI_CHECK + INCLUDE_MINIFY + SYMBOL_POLICY + SIGNATURE_MATRIX。
Phase B（语义）：SEMANTIC_GUARD + DEGRADATION_TAG + FAILURE_CLASSIFIER（文本规则）。
Phase C（成本）：CONTEXT_TRIM + PERF_ITER_HEURISTIC。
Phase D（优化）：语义完成度度量（在 SUMMARY 中统计 degradation_flags）。

---
## 10. 风险与 Prompt 缓解
| 风险 | 描述 | Prompt 缓解策略 |
| ---- | ---- | --------------- |
| 模型忽略自检块 | 直接写文件 | 强制规则：无自检块判定视为逻辑失败需自我重述 |
| 误判退化 | 正常实现被标记 | 让模型在 DEGRADED 原因后附 “撤销条件” 描述 |
| 过度裁剪导致遗忘 | 引用 ID 缺上下文 | 要求引用时携带 5~10 字摘要（REF:S5.include_rule:"only minimal includes"） |
| 语义编码不一致 | 多策略混用 | SEMANTIC_GUARD 中必须唯一选项 `encoding_choice` |
| PERF 估算失真 | 暖身样本偏小 | 约束 warm_calls ∈ [100,400] 并若 per_call_stddev 高则增加样本 |

---
## 11. 验收与回归核对清单
提交一次成功会话产物时应包含：
- `[PYI_CHECK]`、`[SYMBOL_POLICY]`、`[INCLUDE_MINIFY]`、`[SIGNATURE_MATRIX]`、`[SEMANTIC_GUARD]` 至少各 1 次
- 若出现失败：至少 1 条 `[DIAG]`，且后续修改针对其 fix 字段
- `[CONTEXT_TRIM]` 至少出现 1 次（在首次成功后）
- `[SUMMARY]` 含：segments_used / segments_trimmed / degradation_flags / semantic_encoding / perf_iters

---
## 12. 总结
通过将“工具外部校验”转化为“Prompt 内部显式自检 + 元信息协议”，我们：
1. 保留全部可观测信号（以结构化块形式出现）；
2. 降低外部依赖与开发成本；
3. 为未来的解析/自动质检提供格式稳定的锚点；
4. 形成“声明 → 自检 → 产出 → 诊断 → 精简”统一闭环。

下一步应优先在 `core_task.md` 中植入：段编号、各自检块格式说明、失败到修复的单步映射表。

