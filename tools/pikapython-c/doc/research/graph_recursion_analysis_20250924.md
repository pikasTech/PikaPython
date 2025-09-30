# GraphRecursionError 与 anyio Cancel Scope 异常分析报告 (2025-09-24)

## 1. 概要
本次故障在一次 `def add(a,b): return a+b` 的转换与构建过程中触发：
- 上层捕获到 `langgraph.errors.GraphRecursionError: Recursion limit of 25 reached without hitting a stop condition.`
- 同期 stdout 之前输出了一个 `RuntimeError: Attempted to exit cancel scope in a different task than it was entered in` (来自 anyio cancel scope) —— 这是 **次生异常**，属于资源清理阶段（会话关闭）在异常路径上被不正确触发的症状，而非根因。

## 2. 执行流程复盘 (session_20250924_112459)
按日志时间序列抽样：
1. LLM1 -> 工具调用 `write_file` 覆盖已存在 pyi 被拒绝 (写前未 read)
2. LLM2 -> 识别错误，调用 `read_file` 补读；随后反复进入 agent/tools 循环。
3. 到 LLM13 时，已经完成：
   - 读取与写入 `add_module.pyi`
   - 创建 `add_module_math.c`
   - 更新测试脚本 `test_example.py`
   - 运行构建 `run_pika.py` 失败（链接阶段 multiple definition）
   - 读取 `compile.log` 发现 `add_module_Math.c` 与 `add_module_math.c` 提供相同符号 `add_module_Math_add`
   - 计划删除冲突文件 `add_module_Math.c`（触发 `run_shell` rm），但后续日志显示又继续尝试读取文件、再进入写/读循环。

### 2.1 状态机 (StateGraph) 结构
```
agent --(AIMessage.tool_calls存在)--> tools --(始终)--> agent
            ^                                 |
            | (无工具调用)---------------------+ (END)
```
终止条件仅依赖：最后一个消息是否为 `AIMessage` 且其 `tool_calls` 为空。

### 2.2 循环未收敛原因
- agent 在处理错误恢复与后续步骤时持续生成新的 `tool_calls`，未在满足“任务完成（已得到最终用户可读结果）”后产出一个 **无工具调用** 的最终 AIMessage。
- 没有显式“完成”检测：例如检测到成功/失败输出结构 `[MODULE]` ... `[END]` 或检测到构建失败后已给出总结即可终止。
- 发生编译错误后逻辑没有一次性生成总结，而是分步探索：列目录 / 读 compile.log / 读冲突文件 / 尝试清理 / 继续读写。期间每步都附带新的工具调用，导致 graph 不断往返。

## 3. 根因分析
| 现象 | 根因分类 | 详细说明 |
|------|----------|----------|
| GraphRecursionError | 业务终止条件缺失 | 仅靠“无 tool_calls”结束，不满足任务完成态判定；LLM策略未被 prompt 强化为在完成后产出纯文本总结；错误处理分步化导致连续工具链条。|
| 重复写/读文件序列 | 策略冗余与 prompt 指令复杂度 | Prompt 强制的写前读检查（安全策略）在第一次写失败后被正确补救，但后续对测试脚本、C 文件再次进行 read -> write 即使非必需，增加工具链长度。|
| multiple definition 链接错误 | 文件命名规范/生成策略不统一 | 既存在 `add_module_Math.c` 又生成 `add_module_math.c`，符号相同；推测：早期自动生成(可能在 pipeline 另一阶段或旧遗留) + 新生成文件命名风格不同；未执行冲突检测。|
| anyio cancel scope 异常 | 资源关闭次生问题 | 在顶层 `asyncio.run` 收到未捕获的 GraphRecursionError 后，`stdio_client`/`session` 异常未按期望顺序退出，触发 anyio 对 cancel scope 使用线程/任务不一致报错。|

## 4. 触发链条 (事件序列)
1. 初次写 pyi 失败 -> 进入恢复分支
2. 反复工具调用 (读 -> 写 -> 再创建 C -> 再读测试 -> 写测试 -> run_shell 构建 -> 构建失败 -> 探测日志目录 -> 读 compile.log -> 读冲突文件) 仍未总结
3. LLM 继续提出清理动作 (rm、再构建) -> 每次都包含 tool_calls
4. 达到默认 recursion_limit=25 (来自配置 RECURSION_LIMIT 或 LangGraph 默认) -> 抛出 GraphRecursionError
5. 上层无 try/except 包装 graph 调用专门处理该错误 -> 冒泡
6. 清理阶段 anyio 资源退出次序异常 -> cancel scope RuntimeError

## 5. 风险评估
| 风险 | 影响 | 优先级 |
|------|------|--------|
| 无结束判定导致无限循环 | 消耗 token / 工具执行 / 构建资源，最终崩溃 | 高 |
| 文件命名冲突未检测 | 构建失败，误导后续决策 | 高 |
| 清理阶段异常 | 潜在句柄/子进程泄漏 | 中 |
| 分步式日志探测策略 | 放大循环深度 | 中 |
| 缺少递归深度保护反馈 | 用户看不到中间进展总结 | 中 |

## 6. 修复建议
### 6.1 终止条件增强
在 `should_continue` 外增设“任务完成/失败判定”，策略：
1. 如果最近 3 条消息存在一条 AIMessage，且其 content 匹配以下任一：
   - 成功模式：包含 `[MODULE]` 与 `[END]`
   - 错误模式：`[BUILD_FAIL]` 或 `[RUN_FAIL]` 或 `[ERROR]`
   则返回 end。
2. 若最近 N (配置) 轮内重复同一个 tool call 模式 (比如连续多次 run_shell 查看目录但无新信息)，终止并输出 `[ERROR] 循环检测触发`。

可实现：在 `_call_model` 后对 response 分析（或者自定义结束边条件函数扩展 state 持久化统计）。

### 6.2 循环保护 / 去重
在 state 中维护：
```
state['tool_history']: List[ {name, args_hash} ]
```
- 若最近 K 次完全相同 -> 阻断继续，返回建议总结。
- 增加 `max_tool_calls_per_input` (比如 15)。超过则强制结束并提示用户人工介入。

### 6.3 文件生成策略统一
- 约定：所有自动生成的 C 文件名称统一小写：`<module_name>_<class_lower>.c`  OR 统一首字母大写，不混用。
- 在写入新 C 文件前扫描目录内现有同功能符号 (可正则 grep `_Class_add(`)；若存在则改为覆盖同一文件而非新增不同大小写版本。
- 在构建失败后如检测到 `multiple definition of`，解析出重复符号，列出涉及对象文件路径；自动选择保留最新生成的一个，其余 rename 或删除（谨慎：先 read 备份内容写入 `.bak`）。

### 6.4 构建后处理原子化
引导 LLM 一次性：构建失败 -> 读取 compile.log -> 输出 `[BUILD_FAIL]` 摘要 并终止，而不是逐步交互多个工具；这可通过 prompt 加入“出现构建错误后立即总结，不再进行目录列举”。

### 6.5 执行阶段异常捕获
在 `process_input` 调用 graph 前后包裹 try/except：
```
try:
    result = await self.graph.ainvoke(state, config={"recursion_limit": RECURSION_LIMIT})
except GraphRecursionError as e:
    # 记录日志并创建 AIMessage 总结，附带最近工具调用摘要
```
这样用户仍能看到结构化错误反馈，避免直接冒泡破坏会话清理。

### 6.6 anyio cancel scope 异常修正
- 确保 `initialize` 时的 `stdio_client.__aenter__` 与退出在同一 task：当前使用 `asyncio.run(main())` 下是单任务，但当异常发生在 await 树内部时可能破坏顺序。保证只在 `main()` 内用 `async with PythonCTranspiler()` 包裹，自动调用 `__aexit__`，减少手动 close 顺序错误风险。
- 使用 `contextlib.AsyncExitStack` 集中管理 session 与 stdio_client，确保异常时也按栈顺序退出。

### 6.7 配置项增强
在 `MCP_config.py` 中加入：
```
MAX_TOOL_CALLS_PER_INPUT = 20
MAX_DUP_TOOL_REPEAT = 3
```
并在 graph 运行前注入 config：`self.graph = graph_builder.compile(checkpointer=None)` 之后调用 `ainvoke(state, config={"recursion_limit": RECURSION_LIMIT})`。

### 6.8 监控/日志改进
- 在每轮 LLM/Tool 日志加入 `loop_index` 与 `dup_tool_count` 字段。
- 最终生成总结日志 `final_summary.log`。

## 7. 代码修改建议 (示例片段)
### 7.1 自定义终止函数替换 `should_continue`
```python
def should_continue(state: State) -> str:
    msgs = state['messages']
    last = msgs[-1]
    content_join = '\n'.join([m.content for m in msgs if hasattr(m, 'content')][-5:])
    # 成功/失败模式
    if isinstance(last, AIMessage):
        if any(tag in last.content for tag in ['[MODULE]', '[BUILD_FAIL]', '[RUN_FAIL]', '[ERROR]', '[END]']):
            return 'end'
        if not last.tool_calls:
            return 'end'
    return 'tools'
```
(需同时在状态中增加重复检测逻辑，可封装一个 pre-check 函数在 `_call_model` 返回后立即插入人工 ToolMessage 用于强制结束。)

### 7.2 Graph 调用包裹
```python
from langgraph.errors import GraphRecursionError

async def process_input(...):
    try:
        result = await self.graph.ainvoke(state, config={'recursion_limit': RECURSION_LIMIT})
    except GraphRecursionError as e:
        summary = AIMessage(content=f"[ERROR] 工具循环终止: {e}\n请精简步骤或检查终止条件。")
        state['messages'].append(summary)
        return state
```

## 8. 验证策略
| 阶段 | 用例 | 预期 |
|------|------|------|
| 正常生成 | 简单 add 函数 | <= 10 轮完成，输出 `[MODULE]` 块并结束 |
| 强制构建失败 | 刻意制造重复符号 | 输出 `[BUILD_FAIL]` 摘要并结束，不进入 >15 轮 |
| 工具重复 | 模拟重复 read_file 4 次 | 第 3 次后终止并总结重复循环 |
| RecursionLimit | 人为设置 limit=5 | 第 5 轮抛异常但被捕获并有总结 AIMessage |

## 9. 后续优化方向
- 引入简易任务阶段状态机：`INIT -> GEN_FILES -> BUILD -> SUMMARIZE`；阶段外的工具请求拒绝。
- 使用 embedding 对最近消息进行压缩，减少 token 使用。
- 给 LLM 提供结构化 scratchpad（JSON）减少重复描述 prompt。

## 10. 结论
根因是**终止判定与循环控制缺失**导致的无界 agent-tool 往返，以及文件命名策略不一致引发的构建错误延长了循环链条。通过增强结束条件、引入重复检测、捕获 GraphRecursionError 并结构化总结、统一文件命名与构建失败快速总结策略，可在较小改动下显著提升稳定性与资源利用效率。

---
(本报告可提交版本控制，供后续迭代引用。)
