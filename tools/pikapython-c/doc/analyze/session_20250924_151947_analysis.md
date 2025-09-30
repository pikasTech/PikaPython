# Session 分析报告: session_20250924_151947

时间窗口: 2025-09-24 15:19~15:28 (从首个 LLM 日志 15:19:57 到最后一个 15:28:51)

## 1. 概览
| 指标 | 数值 |
|------|------|
| LLM 调用次数 (summary_stats) | 8 (中途已达成功判定阈值) |
| 实际产生日志的 LLM 条目 | 35 (后续继续调用) |
| 工具调用次数 (summary_stats) | 8 (成功判定前) |
| 目录中 TOOL 日志总数 | 34 (含后续冗余) |
| 首次成功条件出现 | 第 8 次 LLM 调用后 (包含 SELFTEST OK) |
| 生成文件 | `matmul2d/matmul2d.pyi`, `matmul2d/matmul2d_matmul2d.c`, `test_example.py` |
| 关键失败命令 | 构建/运行命令多次超时或返回码 5 |

> 说明: `summary_stats.log` 在第 8 次 LLM + 工具调用后写出, 标记 `success=true`。然而对话并未自动终止，后续产生了额外的 27 轮 LLM/工具交互 (日志编号继续增长到 LLM35/TOOL34)。

## 2. 主要困难与症状
### 2.1 成功终止条件未被严格执行
虽然在第 8 次调用后已经满足了 `[EXAMPLE][SELFTEST] matmul2d OK` (成功判定逻辑)，系统也写出了 `[SUMMARY]`，但:
1. Agent 仍继续提出对测试脚本“简化”的额外写入 (违反“成功后不再修改”约束)。
2. 继续尝试多种变体的运行命令 (相对路径 / 进入工作目录再调用 / 增加 `2>&1`)。

根因推测:
- 终止逻辑只在工具节点内部设置 `_summary_emitted`，但图调度仍允许继续 (可能 LLM 在总结后又生成了新的 tool_calls)，缺少“全局短路”机制。
- Prompt 虽说明“成功后立即结束”，但模型输出后仍继续；需要硬性封装或在 `_call_model` 层判断 `_summary_emitted` 后直接返回 END。

### 2.2 构建/运行命令超时 / returncode=5 未被语义化
观测到多次 `run_shell` 返回:
```
ERROR: 命令执行超时
ERROR: returncode=5 stderr=
```
问题:
1. 超时未附加命令上下文、未提供建议 (例如“可能在构建依赖下载” / “需要缩短示例” / “应先清理再编译”).
2. returncode=5 没有 stderr 内容，Agent 再次盲目重试路径变化，而非定位真实失败源 (可能是 `run_pika.py` 内对于输入文件或模块加载失败直接 exit 5)。
3. 未提供构建阶段产生的中间日志路径（例如 `logs/run/<timestamp>/run.log`）供 LLM 读取。

### 2.3 日志检索策略混乱
模型在超时后尝试读取 `logs/compile/latest/compile.log`，该路径并不存在 (当前流程似乎没有创建 `logs/compile/latest/` 软链接或目录映射)。
- 缺乏“如何发现最近运行日志”的稳定指导；模型转而使用 `find logs -name "*.log" | head -5`，列出旧 session，无助于当前问题。

### 2.4 C 层接口设计潜在风险
`matmul2d_matmul2d.c` 中返回值类型使用 `PikaObj*` 作为矩阵，未做数值与结构错误提示；兼容性检查失败时直接返回空列表对象。缺乏用户级错误语义 (例如应在不匹配时抛异常或打印 `[RUN_FAIL] incompatible shapes`)。

### 2.5 测试脚本结构在成功后被改写
在成功总结后再次写入精简版 `test_example.py`，破坏最初产生的性能测试逻辑；如果后续再运行，将缺失 PERF 指标输出，导致统计不一致。

### 2.6 资源/时间控制不足
运行命令统一 120s 超时 (服务器工具设定)，但矩阵运算/构建通常较快；若构建脚本内部等待或交互，会消耗整个窗口。缺少渐进式诊断 (先 `make -n` 或 `--help` 验证，再全量 run)。

## 3. 行为模式分析
| 模式 | 描述 | 影响 |
|------|------|------|
| 重复路径试探 | 在不同工作目录前缀下重复执行同一 run 命令 | 增加 token 与工具调用浪费 |
| 缺少错误抽象 | 直接看到 returncode=5 而不去查找对应 run.log | 无法定位根因，陷入盲试 |
| 成功后继续增量“优化” | 违反设计的终止策略 | 生成多余日志，易引入回归 |
| 日志枚举替代精确定位 | 使用 find/head 列举旧 session | 干扰上下文，可能错误读取过期信息 |

## 4. 根因归类
1. 终止控制缺陷: 成功标记未上升为图全局停止条件。
2. 工具反馈粒度不足: `run_shell` 对超时 / 非零退出缺少结构化诊断字段（命令 / 用时 / 建议）。
3. Prompt 约束软性: 没有“出现 [SUMMARY] 时禁止继续生成 tool_calls”的强制守卫 token（可在系统层过滤）。
4. 日志命名/索引缺少“最新运行别名”: 使模型用猜测路径或 fallback 搜索。
5. 缺少构建阶段拆解: 直接全量 run，失败后无中间产物指导。

## 5. 改进建议
### 5.1 终止机制强化
1. 在 `_call_model` 前置检查: 若 `_summary_emitted` 为 True，直接返回 END，不再调用 LLM。
2. 在 `_call_tool` 生成 SUMMARY 后，将 state 注入一个终止标志 message，如 `SystemMessage("__SESSION_SUCCESS_TERMINATED__")`，供条件边判断。

### 5.2 工具输出结构化
为 `run_shell` 返回 JSON 字符串 (成功/失败)，字段示例：
```json
{"status":"ok","returncode":0,"cmd":"...","stdout_head":"...","duration_ms":1234}
{"status":"timeout","cmd":"...","timeout_s":120}
{"status":"error","returncode":5,"stderr_head":"...","hint":"check run_pika.py arguments"}
```
LLM 可据此分支策略，不再盲试。

### 5.3 构建分阶段命令化
拆分:
1) 生成模块 -> 验证文件存在 (ls)
2) dry-run（如支持）/ 预检命令 (`python run_pika.py --check ...` 可添加)  
3) 真正构建 & 运行
在 prompt 中显式列举步骤。

### 5.4 日志索引工具
新增 MCP 工具: `list_recent_run_logs(limit:int=1)` 返回：
```json
{"compile_log":"logs/run/20250924_152500/compile.log","run_log":".../run.log"}
```
避免使用 `find`。

### 5.5 成功后写保护
在 `_success_detected` 后：
1. 拦截 `write_file` 调用并返回 `ERROR: session finalized`。
2. 或在服务器层维护会话状态，拒绝进一步写操作。

### 5.6 增量诊断提示模块
失败连续 2 次以上的 `run_shell` 调用 (同一命令前缀)，在第三次前注入指导: “请先读取 <run_log> 或 <compile_log> 尾部 40 行再重试”。

### 5.7 统一错误标签
将超时与非零退出映射到 `[RUN_FAIL]` / `[BUILD_FAIL]`，符合 prompt 中的分类；这样成功检测逻辑更清晰。

### 5.8 适度 Token 限流策略
在第 N (如 12) 次 LLM 调用仍未产生新关键阶段进展时，注入系统消息要求模型输出诊断总结而不是继续试探命令。

### 5.9 C 接口错误语义
在矩阵维度不匹配时返回一个明确的对象/打印日志，例如：
```
obj_setErrorCode(self, 1); // 或设置特殊字段
```
并在 Python 包装层转换为异常，便于测试脚本捕获并给出 `[RUN_FAIL] incompatible shapes`。

## 6. 可实施的快速修复清单 (优先级)
| 优先级 | 行动 | 预期收益 |
|--------|------|----------|
| P0 | 在 `_call_model` 开头如 `_summary_emitted` 直接返回空消息并终止图 | 立即阻止成功后继续改写 |
| P0 | `run_shell` 增加结构化 JSON 输出 | 提升 LLM 决策稳定性 |
| P1 | 新增 `list_run_logs` 工具 | 减少无效日志搜索 |
| P1 | 写保护：成功后拒绝 `write_file` | 防回归 |
| P2 | 失败次数阈值注入诊断系统消息 | 降低 token 消耗 |
| P2 | C 层错误语义改造 | 改善调试体验 |

## 7. 结论
本次 session 的核心问题不是功能生成失败，而是“成功后的不收敛”和“缺乏精确诊断数据导致的反复试探”。通过强化终止控制、结构化工具输出与日志索引、以及构建阶段拆分，可显著降低冗余循环和 token 浪费，并提升自动化稳定性。

---
（生成时间: 自动分析，供后续迭代参考）
