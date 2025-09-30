# 面向PikaPython的AI驱动Python-C编译加速器与自动化验证平台设计

## run_pika.py 使用说明（单一路径：入口替换 + 可选模块注入）

脚本 `run_pika.py` 用于将指定的 Python 脚本临时作为 `pikapython` 的入口并完成：预构建 -> 编译 -> 运行，全过程日志分离保存。

核心特性（无历史兼容分支）：
1. 备份并临时替换 `pikapython-linux/pikapython/main.py`
2. 可选：`--module <name>` 将根目录 `<name>/` 中的 `<name>.pyi` 与 `<name>_*.c` 临时复制注入（构建后清理）
3. 若首次或无 build/Makefile 或发生模块注入则执行 `cmake ..`
4. 执行预构建（`wine rust-msc-latest-win10.exe`，若存在）
5. 执行并发编译（`make -jN`）
6. 运行生成的 `./build/pikapython`（可 `--no-run` 跳过）
7. 全程日志分离：`logs/run/<timestamp>/compile.log` / `run.log`
8. 结束后恢复 `main.py` 并清理注入文件

### 用法

最简单（仅替换入口，不注入模块）：
```bash
python3 run_pika.py file_create/test_code.py
```

带模块注入（示例模块 `test_module_example`，参考 `GUIDE.md`）：
```bash
python3 run_pika.py --module test_module_example test_example.py
```

指定并行度：
```bash
python3 run_pika.py -j 32 file_create/test_code.py
```

仅编译不运行：
```bash
python3 run_pika.py --no-run file_create/test_code.py
```

### 可选环境变量

| 变量 | 说明 |
| ---- | ---- |
| `KEEP_BUILD=1` | 若已有 build 目录且包含 Makefile，则跳过 cmake 重新生成 |
| `VERBOSE=1` | 打印调试级日志（命令执行细节） |

示例：
```bash
VERBOSE=1 KEEP_BUILD=1 python3 run_pika.py file_create/test_code.py
```

### 日志结构

执行一次后将生成形如：
```
logs/run/20250922_214705/
	compile.log   # cmake (若发生)、预构建、make 的输出
	run.log       # 可执行程序运行输出（若未使用 --no-run）
```

运行过程中终端只展示每类日志的最后若干行（默认 5 行，可通过 `--tail-lines` 调整）。
失败时会自动展示最近 `--fail-tail` 行（默认 20 行）帮助定位问题，并给出完整日志路径。

终端摘要格式示例（成功场景）：
```
[run_pika] 执行完成，摘要输出：
[run_pika] ======== COMPILE TAIL (last 5) ========
... <compile.log 最后 5 行> ...
[run_pika] ======== END ========
[run_pika] ======== RUN TAIL (last 5) ========
... <run.log 最后 5 行> ...
[run_pika] ======== END ========
[run_pika] 日志路径汇总 =>
[run_pika] compile_log: /abs/path/logs/run/<ts>/compile.log
[run_pika] run_log:     /abs/path/logs/run/<ts>/run.log
```

失败时会输出：
```
[run_pika] [ERROR] Command failed (exit 2): make -j16
[run_pika] 失败摘要输出：
[run_pika] ======== COMPILE FAIL TAIL (last 20) ========
... <compile.log 最后 20 行> ...
[run_pika] ======== END ========
[run_pika] ======== RUN FAIL TAIL (last 20) ========  # 若存在 run.log
... <run.log 最后 20 行> ...
[run_pika] ======== END ========
[run_pika] 日志路径汇总 =>
[run_pika] compile_log: /abs/path/logs/run/<ts>/compile.log
[run_pika] run_log:     /abs/path/logs/run/<ts>/run.log
```

可调参数摘要：

| 参数 | 说明 |
| ---- | ---- |
| `--tail-lines N` | 成功时展示最后 N 行编译/运行日志（默认 5） |
| `--fail-tail N` | 失败时展示最后 N 行日志（默认 20） |
| `--no-run` | 只编译不运行 |
| `-j / --jobs` | make 并行度（默认 16） |

### 返回码

- 0：执行成功（已恢复 main.py）
- 非 0：构建或运行阶段失败（仍会尝试恢复 main.py）

### 常见错误

- 参数缺失或文件不存在
- 传入的不是 `.py` 脚本
- 生成的可执行文件缺失（编译失败或路径异常）

### 模块注入说明

参见 `GUIDE.md`：只保留“根目录已有模块 + --module 注入 + 构建后清理”这一条路径，无自动生成、无 fallback。

### 未来可选增强（未实现，仅想法）
- 回归测试批处理与结果聚合
- 运行输出解析 + 基线 diff
- 编译耗时阶段统计
- JSON 结果摘要导出（供 CI 使用）

## 动态工作目录机制（Agent 侧新增）

Agent 在每次启动/首次用户任务时会自动创建独立的会话工作目录：

```
./file_create/<timestamp>/
```

示例：`./file_create/20250924_140233/`

所有由 LLM 生成的 `.pyi`、`.c`、`test_example.py` 等文件必须写入该目录或其子目录；写入其它路径将被 MCP `write_file` 工具拒绝，并返回错误：

```
ERROR: 写入被拒绝。文件不在本次会话工作目录内: <path>. 允许根目录: <abs_work_dir>/
```

Prompt 中历史出现的 `./file_create/` 会被动态替换为当前工作目录（尾随 `/`），也支持使用占位符 `{{WORK_DIR}}`。这保证多任务/并发或连续会话间互不污染。

环境变量 `SESSION_WORK_DIR` 会在会话内注入供服务端工具判断合法写入范围。

### 目的
1. 避免不同任务生成文件冲突（尤其同名模块）。
2. 提供可溯源的产物归档路径。
3. 降低清理与回滚复杂度。

### 注意事项
- 读取（read_file）不受限制，可读取仓库其它位置（如日志）但不要写。
- 构建调用 `run_pika.py` 时需要使用 `--module <name>` 并指定该动态目录为 `--module-dir`，即：
	`python run_pika.py --module <module_name> --module-dir <dynamic_dir> <dynamic_dir>/test_example.py`
- 跨会话复用文件需手工复制到新的会话目录。


