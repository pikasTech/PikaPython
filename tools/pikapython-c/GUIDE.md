# PikaPython 模块新增与测试指南 (单一路径注入模式)

要求（对应 REQUIRE.md）：
1. 不做任何自动生成 / 模板生成 / 回退 / fallback / 默认逻辑；只有一种方法。
2. 通过 `--module <name>` 将根目录已存在的模块目录 `<name>/` 注入到 `pikapython-linux/pikapython/` 参与一次性构建，结束后清理。
3. 提供示例脚本 `test_example.py`：`python run_pika.py --module test_module_example test_example.py` 可直接运行通过。

核心准则：简单、单一、可预测。

---
## 目录
- [快速开始](#快速开始)
- [核心概念回顾](#核心概念回顾)
- [模块文件规范（唯一方式）](#模块文件规范唯一方式)
- [测试脚本编写规范](#测试脚本编写规范)
- [性能对比规范](#性能对比规范)
- [示例：test_module_example](#示例test_module_example)
- [常见问题排查](#常见问题排查)
- [进阶与扩展建议](#进阶与扩展建议)

---
## 快速开始
根目录已有示例模块目录：
```
test_module_example/
  ├─ test_module_example.pyi
  └─ test_module_example_Test.c
```
示例脚本：`test_example.py`

运行：
```bash
python run_pika.py --module test_module_example test_example.py
```
期望输出包含：
```
[EXAMPLE] add(7, 35)= 42
[EXAMPLE][SELFTEST] test_module_example OK
```
运行完成后，注入到 `pikapython-linux/pikapython/` 的 `.pyi` 与 `pikascript-lib/test_module_example/` 会被自动清理，只保留根目录原始示例。

---
## 核心概念回顾
- `.pyi` 文件：定义模块的类与方法签名（声明式），预编译器扫描后生成 C 头文件 + 绑定代码。
- C 实现：位于 `pikapython-linux/pikapython/pikascript-lib/<module>/`，函数名模式：`<module>_<Class>_<method>`。
- 预编译：通过 `wine rust-msc-latest-win10.exe` 把 `.py` + `.pyi` 打包到自动生成的 `pikascript-api` 中。
- 运行脚本：`run_pika.py` 会临时替换 `main.py` 内容，编译并运行后恢复。
（本指南仅使用“根目录已有模块 + --module 注入 + 运行后清理”这一条路径。）

---
## 模块目录规范（根目录形态）
放置位置（根目录）：
```
<module>/
  <module>.pyi
  <module>_*.c   (可一个或多个 C 源文件)
```
构建时通过：
```
python run_pika.py --module <module> your_script.py
```
脚本会：
1. 复制 `<module>.pyi` 到 `pikapython-linux/pikapython/`
2. 复制所有 `<module>_*.c` 到 `pikapython-linux/pikapython/pikascript-lib/<module>/`
3. 若 `main.py` 开头未导入则插入 `import <module>`（保证预编译扫描）
4. 强制（若需要）重新 cmake + 预编译 + make
5. 运行可执行文件
6. 结束：恢复原 `main.py`，删除复制进去的 `.pyi` 和临时 C 目录

---
## 测试脚本编写规范
建议放置在 `file_create/` 下：
- 文件命名：`<module>_test.py` 或 `<module>_demo.py`
- 输出前缀：
  - 功能示例：`[EXAMPLE] ...`
  - 自测通过：`[EXAMPLE][SELFTEST] <module> OK`
  - 性能指标：`[PERF] python_total=... mean=...us` / `cmod_total=...` / `speedup(...)= ...x`
- 避免依赖外部文件系统（所有逻辑内联）
- 使用 `assert` 直接失败可暴露栈信息

---
## 性能对比规范
为直观展示 C 模块与纯 Python 版本性能差异，测试脚本中应：
1. 内联定义一个 Python 基线函数（与 C 模块方法语义一致，如 `def py_add(a,b): return a+b`）。
2. 设定 `ITER = 10000`，循环分别调用 Python 与 C 版本累计时间。
3. 使用 `time.time()`（兼容环境）计时；不依赖 `statistics`、`perf_counter` 或复杂表达式。
4. 输出格式示例：
```
[PERF] python_total=0.002345s mean=23.45us
[PERF] cmod_total=0.000980s mean=9.80us
[PERF] speedup(py_mean/c_mean)= 2.39x
```
5. 最终仍需打印 `[EXAMPLE][SELFTEST] <module> OK`。

注意：保持算法极简，避免引入在精简运行时不可用的库。

## 示例：test_module_example
根目录：
```
test_module_example/
  test_module_example.pyi
  test_module_example_Test.c
test_example.py
```
示例脚本 `test_example.py`（含性能对比精简实现）：
```python
import time
import test_module_example

def py_add(a, b):
  return a + b

obj = test_module_example.Test()
print('[EXAMPLE] add(7, 35)=', obj.add(7, 35))
print('[EXAMPLE] greet:"', obj.greet('demo'), '"', sep='')
assert obj.add(1, 2) == 3
assert obj.greet('x').startswith('Hello,')

ITER = 10000
py_total = 0.0
c_total = 0.0
for _ in range(ITER):
  t0 = time.time(); py_add(123,456); py_total += (time.time()-t0)
  t1 = time.time(); obj.add(123,456); c_total += (time.time()-t1)

if ITER > 0:
  py_mean = py_total / ITER
  c_mean = c_total / ITER
else:
  py_mean = c_mean = 0.0
ratio = py_mean / c_mean if c_mean > 0 else 0.0
print('[PERF] python_total=' + ('%.6f' % py_total) + 's mean=' + ('%.2f' % (py_mean*1e6)) + 'us')
print('[PERF] cmod_total=' + ('%.6f' % c_total) + 's mean=' + ('%.2f' % (c_mean*1e6)) + 'us')
print('[PERF] speedup(py_mean/c_mean)= ' + ('%.2f' % ratio) + 'x')

print('[EXAMPLE][SELFTEST] test_module_example OK')
```
运行：
```bash
python run_pika.py --module test_module_example test_example.py
```

---
## 常见问题排查
| 现象 | 排查 | 解决 |
|------|------|------|
| 运行输出缺少新模块方法 | 未导入模块 | 确认脚本或插入的 import 存在 |
| 链接时报 undefined reference | C 函数命名不匹配 | 确认 `<module>_<Class>_<method>` 命名并重新构建 |
| 重复残留导致拒绝注入 | 上次异常未清理 | 重新运行会先删除遗留；若失败手工清理 build/ 内残留再尝试 |
| 字符串返回乱码 | 未使用缓存返回 | 用 `obj_cacheStr(self, buf)` 返回局部缓冲内容 |
| 修改 .pyi 不生效 | 仍在用旧构建 | 删除 build/ 或确保触发重新 cmake（注入时自动触发） |

---
## 进阶与扩展建议
- 扩展模板：在 `.pyi` 增加 float / bytes / 指针类型以测试类型映射。
- 批量测试：编写脚本枚举 `file_create/` 下 `*_test.py` 并逐一运行收集 `[SELFTEST]` 行。
- 输出结构化：后续可在 `run_pika.py` 中解析运行日志并生成 JSON 汇总。
- 语法子集扫描：新增简单 lint 工具，提前拒绝不受支持语法（见调研报告章节 5）。

---
## FAQ 摘要
1. 需要手动改 CMake 吗？无需，GLOB 会包含复制进去的 C。
2. 有自动生成骨架吗？没有，必须自己写 `.pyi` + C。
3. 注入后不清理可以吗？不可以，脚本自动清理；如需保留请直接放进源码树长期存在。

---
如需新增更复杂示例或自动化测试框架，请在 Issue/需求中提出。

> 本指南采用唯一“根目录已有模块 + --module 注入”模式。无其它路径，无生成，无回退。

---
## LLM 调用重试策略说明 (运行期)
自 `Client.py` 引入自定义重试后，所有 LLM 调用遵循以下规则：

1. 关闭 LangChain 默认重试 (`max_retries=0`)，完全由自定义逻辑控制。
2. 默认延迟序列（单位: 秒）：`1, 2, 5, 10, 30, 60, 120`，共 7 次等待 + 首次立即调用 = 最多 8 次尝试。
3. 可通过环境变量覆盖：
  ```bash
  export LLM_RETRY_DELAYS="0.5,1,2,4,8"
  ```
  - 逗号分隔浮点或整数；非法/空值会被忽略；若整体解析失败则回退默认序列。
4. 失败判定：捕获所有异常（包括 4xx 如余额不足 402、临时网络问题、限流等）。不中途过滤，以支持在重试窗口内外部补款或限流恢复。
5. 日志：
  - 每次失败追加写入 `logs/session_<ts>/llm_retry.log`（JSON，多条覆盖写入最终状态）。
  - 最终仍失败则写入 `llm_final_error.log`，并抛出异常终止图执行。
6. 成功后续：正常记录本次 usage 统计，照常写入 `summary_stats.log`。
7. 若需彻底禁用重试（不建议），可将环境变量设置为 `LLM_RETRY_DELAYS=""` 或单元素 `>=1`，例如：`export LLM_RETRY_DELAYS="1"`。

此策略保证：
* 平均恢复时间适中（前 4 次在 18s 内完成），
* 长尾最高等待 ~228s，
* 可在余额/限流临时问题下获得最大生存性。

> 注意：一旦最终失败，外层交互模式会直接看到异常堆栈；非交互一次性模式会在统计输出前终止。
