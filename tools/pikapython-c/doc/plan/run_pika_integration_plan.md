# run_pika.py 集成与 Prompt 外置规划

## 目标
- 用 `run_pika.py` 替换现有 `Client.py` 中基于 ctypes 动态库 (C->Python->测试) 的验证/测试执行流程。
- 将所有硬编码的大型 prompt 文本迁移到 `./prompt/` 目录下的独立文件，通过文件名动态加载，避免修改代码即改 prompt。
- 维持单一路径：Python -> 生成/已有模块 -> 通过 `run_pika.py` 注入并运行 PikaPython，收集输出，再反馈用户。

## 当前问题
1. `Prompt.py` 内部硬编码多行模板与主控制 prompt，修改不透明、不可热更新。
2. 现流程：生成 C 源 + gcc 构建动态库 + ctypes 加载 + Python 测试脚本运行，与 PikaPython 最终形态割裂。
3. 运行路径与仓库中已经建立的 `--module` 注入体系不统一。
4. 未来想扩展多种 prompt / 任务类型不便。

## 新架构概述
```
User Input
   ↓
LLM (加载 prompt/<X>.md + 任务指令)
   ↓ 工具调用
[文件写入] 生成: <module>/.pyi  与  <module>_*.c
   ↓
调用 run_pika.py --module <module> test_example.py (或动态生成的执行脚本)
   ↓
收集 logs/run/<ts>/run.log 输出
   ↓
解析执行结果 & 返回
```

## 关键改动清单
1. Prompt 外置：
   - 新增目录 `prompt/`
   - 拆分：
     - `core_task.md` (主指令：Python -> C + PikaPython 集成)
     - `test_template.md` (可选：测试脚本生成模板)
     - `messages/` (后续可扩展追加系统/安全策略)
   - `Client.py` 读取：`prompt_loader.load("core_task.md")`
2. Client 逻辑调整：
   - 去除 ctypes 动态库编译逻辑相关描述 (gcc, .so, ctypes.CDLL 等)
   - 新工具调用序列：
     1) 写入模块目录（根目录 `<module_name>/`）
     2) 创建/更新执行脚本（例如 `test_example.py` 或用户指定脚本）
     3) 调用文件写入工具 + 运行工具：执行 `python run_pika.py --module <module_name> <entry_script>`
     4) 读取 run.log / compile.log 解析
3. 结果解析：
   - 只关心模块函数输出与自测断言结果 (`[EXAMPLE][SELFTEST] ... OK`)
   - 若编译失败：截取 compile.log 尾部若干行给用户
4. 失败回滚：
   - `run_pika.py` 已有自清理，无需额外回滚
   - 若生成的模块目录保留以便用户迭代，可在下一次覆盖写入
5. 可选增强：
   - 支持多模块（暂不做；保持单一）
   - 允许动态选择 prompt 文件名参数化启动

## Prompt 设计要点 (core_task.md)
- 明确要求：
  1. 校验用户 Python 代码基本语法
  2. 产出 `.pyi` + 对应 C 实现 (命名 `<module>_<Class>_<method>`)
  3. 生成最小测试脚本调用模块类与方法，打印标准化标记行
  4. 调用 run_pika 构建与运行，读取结果返回
  5. 出错时立即报告并停止后续生成
- 限制：禁止引入多模式 / fallback；禁止生成与当前任务无关的文件

## 接口/工具调整建议
| 需求 | 现状工具 | 需要 | 说明 |
|------|----------|------|------|
| 写文件 | write_file | 继续用 | 生成 .pyi / .c / 脚本 |
| 读文件 | read_file | 继续用 | 解析 logs 输出 |
| 运行命令 | run_bash? (若有) | 需提供 run_cmd 工具 | 执行 `python run_pika.py ...` |
| 列目录 | list_dir | 可选 | 校验模块是否存在 |

(若缺少通用 shell 执行工具，可在 MCP_server.py 新增一个 `run_shell(cmd: str)` 工具。)

## 步骤拆解 (执行路径)
1. 载入主 prompt 文本 (`core_task.md`) 形成 SystemMessage
2. 用户输入 -> LLM 解析 -> 生成待创建文件清单
3. 工具写入：
   - `<module_name>/<module_name>.pyi`
   - `<module_name>/<module_name>_impl.c` (或拆分多个函数文件)
   - 生成执行脚本：`test_example.py` （可固定复用）
4. 触发运行：`python run_pika.py --module <module_name> test_example.py`
5. 读取日志：
   - `logs/run/<latest>/run.log`
   - 失败则读取 `compile.log` 尾部
6. 汇总结果发送给用户

## 版本迁移策略
| 阶段 | 内容 | 成功判定 |
|------|------|----------|
| P0 | 外置 prompt + 新 plan 文档 | Client 仍旧旧逻辑，但能加载外部 prompt |
| P1 | 替换执行流为 run_pika.py | 用户一次输入可得到 PikaPython 运行结果 |
| P2 | 增强日志解析与错误提示 | 构建失败能定位错误段落 |

## 兼容性处理
- 不保留 ctypes 旧路径；直接删相关 prompt 步骤文字
- 若用户仍输入“生成动态库”类指令：提示已迁移为 PikaPython 运行模式

## 风险与缓解
| 风险 | 描述 | 缓解 |
|------|------|------|
| 运行脚本失败 | 模块文件不符合预编译约束 | 在 prompt 中强化命名/结构校验说明 |
| 注入残留 | 异常中断未清理 | run_pika 已做清理；下次前再次删除同名目录 (已实现) |
| LLM 输出偏离 | prompt 不够约束 | 分离 core_task.md，逐步迭代加强指令 |

## 后续工作 (落地实现顺序建议)
1. 新建 `prompt/core_task.md`（依据上文要点）
2. 实现 `prompt_loader` 工具函数 (如 `load_prompt(name) -> str`)
3. 修改 `Client.py`：
   - 移除对 `Prompt.prompt` 的直接引用，改为动态加载
   - 更新 SystemMessage 内容
4. 添加 `run_shell` MCP 工具 (若当前无通用命令执行)
5. 在 LLM 指令中使用 `run_shell` 触发：`python run_pika.py --module <module> test_example.py`
6. 实现日志结果抓取逻辑：查找最新时间目录，读取 run.log
7. 移除/废弃旧的动态库测试相关描述与模板
8. 验证端到端：输入简单 Python 函数 -> 生成模块 -> 构建 -> 输出自测 PASS

## 验收标准
- 修改后 `Client.py` 启动时不再 import `Prompt.prompt`
- `prompt/core_task.md` 内容加载成功（可打印 hash 验证）
- 输入一段 Python 函数定义，最终响应中包含 PikaPython run.log 的核心输出标识 `[EXAMPLE][SELFTEST]` 行
- 仓库不再出现 ctypes 动态库生成命令字样

---
(本文件存放于 `doc/plan/` 以便后续跟踪执行进度。)
