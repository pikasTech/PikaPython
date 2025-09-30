# run_pika.py 模块临时注入 (--module) 实施计划

## 1. 背景与目标
现有 `run_pika.py` 仅支持：临时以指定脚本替换 `pikapython-linux/pikapython/main.py`，完成预编译 + 构建 + 运行，再恢复 `main.py`。迁移调研文档提出需要支持“临时添加一个被测试的模块（含 .pyi stub 与对应 C 实现目录）”，用于快速验证绑定 + Python 调用链路。该功能需：
1. 通过新增 CLI 参数 `--module <name>` 指定测试模块名（示例：`test_module_example`）。
2. 在运行流程开始前临时创建：
   - `pikapython-linux/pikapython/<name>.pyi`
   - `pikapython-linux/pikascript-lib/<name>/` 目录（可含 1 个最小 C 实现文件）
3. 自动在临时代码注入后触发现有编译流程（预编译会看到该 .pyi 并生成头文件）。
4. 运行用户传入的测试脚本（例如 `file_create/test_code.py`），脚本内部可 `import <name>` 并调用其类/方法。
5. 完成后清理临时创建的 `.pyi` 与 `pikascript-lib/<name>`，同时仍要恢复原 `main.py`。
6. 失败路径（构建或运行失败）也必须清理并恢复。

不改变原有无 `--module` 使用模式；新功能是可选增强且向后兼容。

## 2. 需求清单
| 编号 | 需求 | 说明 | 验收要点 |
|------|------|------|----------|
| R1 | CLI 增加 `--module NAME` | 可出现 0 或 1 次 | `run_pika.py --module foo x.py` 正常解析 |
| R2 | 生成 `<NAME>.pyi` 模板 | 内容包含至少一个可调用类与方法 | 能在编译日志看到 `binding <NAME>.pyi` |
| R3 | 生成 `pikascript-lib/<NAME>/` 目录 | 包含与 .pyi 对应的 C 函数实现文件 | 链接阶段无缺符号错误 |
| R4 | 不污染仓库 | 运行结束后删除新增文件/目录 | 再次运行 `git status` 不出现残留 |
| R5 | 失败回滚 | 任意阶段异常也应清理 | 人为制造编译失败后确认清理 |
| R6 | 与 VERBOSE 兼容 | 仅调试日志保留 | KEEP_BUILD 已废弃，始终全量干净构建 |
| R7 | 多次运行同名模块 | 应当覆盖/报错策略 | 采用检查：若已存在则报错并退出 (防误删真实目录) |
| R8 | 支持自定义最小接口骨架 | 提供简单默认模板 | 用户无需手写即可调用 |

## 3. CLI 设计
新增参数：
```
--module NAME        指定临时测试模块名（小写/下划线/数字，首字符字母）。
```
校验规则：`^[a-zA-Z][a-zA-Z0-9_]*$`。存在同名 `.pyi` 或 `pikascript-lib/NAME` 时直接报错退出（除非未来增加 `--force`）。

保持原 positional `script` 语义不变。

## 4. 临时文件/目录结构与模板
当指定 `--module test_module_example` 时生成：
```
pikapython-linux/
  pikapython/
    test_module_example.pyi
  pikascript-lib/
    test_module_example/
      test_module_example_Test.c
```

### 4.1 .pyi 模板内容（最小可用）
```python
# 自动生成: 临时测试模块 stub (run_pika.py --module)
class Test:
    def add(self, a: int, b: int) -> int: ...
    def greet(self, name: str) -> str: ...
```

### 4.2 C 实现模板
文件：`test_module_example_Test.c`
```c
#include "pikaScript.h"
// 自动生成: 临时测试模块实现 (run_pika.py --module)
int test_module_example_Test_add(PikaObj* self, int a, int b){
    return a + b;
}
char* test_module_example_Test_greet(PikaObj* self, char* name){
    // 使用 obj_cacheStr 保障返回生命周期
    char buf[64];
    int n = snprintf(buf, sizeof(buf), "Hello,%s", name);
    (void)n;
    return obj_cacheStr(self, buf);
}
```

说明：函数命名模式 `<module>_<Class>_<method>` 与生成头文件期望一致。类名 `Test` 与方法名需与 .pyi 严格匹配。

## 5. 运行流程序列 (含注入)
1. 解析参数；若存在 `--module NAME`：
2. 校验命名与目标路径不存在：
   - `pikapython-linux/pikapython/NAME.pyi`
   - `pikapython-linux/pikascript-lib/NAME/`
3. 生成 .pyi 与 C 文件（记录到待清理列表）。
4. 继续现有备份+替换 `main.py` 流程。
5. 执行（可选）cmake、预编译、make、运行。
6. finally 阶段：先恢复 `main.py`，再删除注入的 .pyi 与目录。
7. 输出 tail 摘要。

## 6. 关键实现点
- 在 `parse_args()` 中新增可选参数 `--module`。
- 在 `main()` 中解析后若存在 name：执行 `inject_module(name)`。
- `inject_module()` 返回结构体/字典，包含：
  - `pyi_path`、`c_dir`、`c_file`、`cleanup()` 回调
- `cleanup()` 在 finally 中调用；需捕获并打印删除异常但不阻断主流程。
- C 函数模板与 .pyi 需保持一致大小写；类名固定 `Test`，可简单满足验证需求。
- 失败时（异常抛出）也会走 finally -> cleanup。

## 7. 清理与回滚策略
- 始终在 finally：
  1. 恢复 `main.py`（已有逻辑）。
  2. 若存在临时模块：
     - 删除 `.pyi`
     - 递归删除 `pikascript-lib/NAME` 目录
  3. 提示 `[run_pika] 已清理临时模块 <NAME>`
- 若用户手动中断 (KeyboardInterrupt)，也被 except 捕获后进入 finally。

## 8. 边界与错误处理
| 场景 | 行为 |
|------|------|
| 同名模块已存在 | 立刻报错退出，保留 `main.py` 原状 |
| 生成文件失败 | 报错并退出；若部分生成需清理已创建部分 |
| 预编译未绑定 | 确认测试脚本已 `import <module>`；否则头文件不会影响 (可在计划外备选：自动在 main.py 头插入 `import <module>` ) |
| 用户脚本未 import | 提示：日志中查看是否出现 `binding <NAME>.pyi`，否则说明未被扫描 |
| 多模块需求 | 当前版本不支持；后续可扩展 `--module` 多次或 `--modules a,b` |

## 9. 验证步骤
1. 正常路径：
   ```bash
   python run_pika.py --module test_module_example file_create/test_code.py
   ```
   观察 compile.log 出现 `binding test_module_example.pyi`；运行期测试脚本可调用：
   ```python
   import test_module_example
   t = test_module_example.Test()
   print(t.add(1,2))
   ```
2. 冲突：预先手工创建同名 .pyi 再执行，应报错。
3. 构建失败：手动改模板制造语法错误，确认清理仍发生。
4. 重复运行：两次执行均成功且无残留。

## 10. 后续扩展建议
- 支持 `--force` 覆盖已有真实模块（谨慎）。
- 支持 `--class-name`、`--methods` 自定义生成接口。
- 支持多模块注入：`--module A --module B`。
- 自动在临时 `main.py` 顶部插入 `import <module>` 免用户脚本显式 import（可由 `--auto-import` 控制）。
- 生成更丰富的模板（含 bytes、float、错误处理）。
- 生成对应的最小测试断言片段并 append 至 main.py（通过标识包裹）。

## 11. 开发任务拆分 (供实现参考)
1. 修改 `parse_args()`：新增 `--module`。
2. 新增 `inject_module(name: str)` 函数。
3. 在 `main()` 里注入模块并将清理函数纳入 finally。
4. 编写冲突检测与命名校验。
5. 验证构建日志与运行脚本行为。
6. 记录 README/文档（可追加到运行文档）。

## 12. 时间与复杂度评估
- 代码改动：~80-120 行新增。
- 风险：低（核心编译链路不变）。
- 预计实施：0.5 人日（含测试）。

---
**完**
