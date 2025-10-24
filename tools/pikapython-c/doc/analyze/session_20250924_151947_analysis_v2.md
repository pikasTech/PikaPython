# Session 再分析 (v2 修正版): session_20250924_151947

> 说明: 该版本修正先前分析中“已成功终止”判断的错误。实际情况是：`[EXAMPLE][SELFTEST] matmul2d OK` 文本并非来自真实执行 `run_pika.py` 的运行日志，而是直接由 LLM 写入的测试脚本文本 (通过 write_file) 与后续 read_file 读取内容拼接进入 ToolMessage，从而触发了当前的 `_detect_success` 逻辑，造成 **False Positive**。

## 1. False Positive 触发链路
1. LLM 生成 `test_example.py`，其中手动包含行：`print("[EXAMPLE][SELFTEST] matmul2d OK")` (或等价结构)。
2. 工具 `write_file` 成功写入后，后续 `read_file` 读取该文件内容。
3. `_call_tool` 中收集 ToolMessage 内容拼接 -> `_detect_success` 函数扫描到文本里同时含有 `selftest` 与 `ok` (大小写不敏感)。
4. `_success_detected` 被置 True，随后生成 `[SUMMARY]`，但实际上 `run_pika.py` 构建与运行阶段尚未成功执行；命令多次返回超时或非零退出 (exit 5)。

当前 `_detect_success` 规则：
```python
if 'selftest' in t and 'ok' in t: return True
if '[module]' in t: return True
```
该逻辑未区分“运行输出渠道”与“文件源代码或未执行的文本”，导致任何源文件中预置的成功标记都能提前结束流程。

## 2. 真实困难点 (与之前版本不同的重点)
| 困难 | 说明 | 影响 |
|------|------|------|
| 构建/运行未成功 | `run_shell` 多次返回 `ERROR: 命令执行超时` 与 `returncode=5` 且无 stderr | 功能从未真正验证 |
| 没有运行日志关联 | Agent 未读取 `logs/run/<ts>/run.log` 或 `compile.log` 最新路径；也没有自动指向这些文件 | 无法基于真实错误调整策略 |
| 成功检测被源码内容欺骗 | 成功信号来自文件静态文本而非 runtime 输出 | 过早进入总结，流程偏离真实目标 |
| 缺乏渠道区分 | ToolMessage 没有标记“来源类型”(运行输出 vs 文件读取) | 判定无法加条件过滤 |
| 无最小执行验证 | 未执行“短跑”命令（如 `python run_pika.py --no-run` 或 `--tail-lines 1`）先确认 exit=0 | 一上来就尝试完整构建耗时且超时无法诊断 |

## 3. returncode=5 可能来源分析
依据 `run_pika.py`：
1. 若 `CommandError` 捕获子进程 make/运行失败，返回底层进程 exit code。
2. `return 3` 仅在“可执行文件缺失”路径；`return 2` 用于参数/资源缺失。exit 5 很可能来自 make 或执行阶段的底层命令（例如 `make` 失败 / 构建中断）。
3. 由于未读取 `compile.log` 尾部，Agent 不知失败属于 CMake / make / 运行阶段哪一环。

## 4. 核心根因 (Revised)
| 根因 | 描述 |
|------|------|
| 判定信号来源未加隔离 | 直接扫描所有 ToolMessage 文本，未限制为“运行日志通道”。 |
| 测试脚本内嵌成功标记 | `test_example.py` 自带 `[EXAMPLE][SELFTEST] ... OK` 打印语句，哪怕未执行也会被识别。 |
| 缺少运行阶段原始输出采集 | `run_pika.py` 的 stdout/stderr 汇总到 `run.log`，Agent 未读取。 |
| 超时/失败无结构化分层信息 | `run_shell` 单一字符串返回，不能指导下一步（例如建议读取哪个文件）。 |
| 没有“执行校验 -> 再总结”的二次确认 | 缺少后置“确认执行成功”步骤，比如检测 run.log 中是否真的出现 `[EXAMPLE][SELFTEST]`。 |

## 5. 修复策略 (针对 False Positive)
### 5.1 判定来源过滤
将成功检测输入限定为：
1. 来自 `run_shell` 且命令包含 `run_pika.py` 的 ToolMessage 输出；或
2. 直接读取的 `run.log` 内容（文件路径匹配 `logs/run/` 且文件名为 `run.log`）。

### 5.2 双阶段确认
成功条件 := (A) 工具构建命令 exit=0 & (B) 在 `run.log` 真实出现 `SELFTEST` 且同一行/附近包含 `OK`。
伪代码：
```python
if tool_name=='run_shell' and 'run_pika.py' in cmd and returncode==0:
    mark build_run_ok=True
if tool_name in ('read_file',) and file_path.endswith('/run.log') and 'SELFTEST' in text and 'OK' in text:
    if build_run_ok: success
```

### 5.3 移除源码扫描成功路径
不再允许 `[MODULE]` 或 `SELFTEST` 出现在普通文件读取（非 run.log）时触发成功。

### 5.4 运行输出采样工具
新增工具 `read_last_lines(file_path, n)` 专门读取 run.log/compile.log 尾部，以减少过长文本干扰。

### 5.5 添加执行回显验证
在 `run_pika.py` 成功路径末尾增加显式行：`[RUN_PIKA_SUCCESS] exit=0`。Agent 先匹配此行，再匹配 SELFTEST 行，双信号确认。

### 5.6 防御性测试脚本检查
在写入 `test_example.py` 前做静态扫描：若出现未受保护的 `[EXAMPLE][SELFTEST]` 直接提示模型："请勿预置成功标记，留待真实运行产生"。

## 6. 针对现有代码的最小修补建议
1. 修改 `_detect_success`：加入来源过滤与双标志缓存 (e.g. `self._build_run_ok` + `self._runtime_selftest_ok`)。
2. 在 `_call_tool` 中捕获 `run_shell` 返回字符串时解析 `returncode`（改造 `run_shell` 为 JSON 返回后更易解析）。
3. 在成功前阻止对 `test_example.py` 的再次覆盖（检测写入次数 >1 且尚未构建成功时发出警告）。

## 7. 优先级实施路线
| 优先级 | 行动 | 说明 |
|--------|------|------|
| P0 | 重写成功检测逻辑 | 阻断 false positive 核心源头 |
| P0 | 结构化 `run_shell` 输出 | 解析 returncode, stdout 摘要 |
| P1 | 添加 run.log 专用读取工具 & 尾部采样 | 降低 token 消耗提升置信度 |
| P1 | 写入成功标记检查 | 防止脚本内预置 SELFTEST |
| P2 | run_pika 添加 `[RUN_PIKA_SUCCESS]` 行 | 双信号确认 |
| P2 | 统计与限频策略 | 控制重复构建重试上限 |

## 8. 结论 (v2)
本 session 并未真正完成构建与自测试；早期总结属于误判。根本问题是**信号来源未被隔离**与**判定逻辑过度宽松**。按照上述分级修复，可阻止同类 False Positive，并改进失败诊断路径。

---
（v2 修订完成，用于替换/补充旧版分析）
