# CPython -> C 与 PikaPython -> C 迁移调研报告

> 日期: 2025-09-22
> 目的: 深入调研从“以 CPython 为中心的 Python->C 扩展/加速链路”迁移到“以 PikaPython 为中心的嵌入式 Python->C（PikaScript）链路”的关键差异、适配路径与风险，回答 4 个核心问题：
> 1) 如何用 PikaPython 运行一段 Python 脚本
> 2) 如何用 PikaPython 绑定一个 C 语言函数（.pyi 绑定机制）
> 3) 如何在 PikaPython 中运行测试代码，同时调用 Python 函数和 C 函数
> 4) PikaPython 的语法/特性限制与在其上编写 Python 需注意的问题
>
> 并给出：迁移策略、最佳实践、典型对比、潜在风险与改进建议。

---

## 目录
- [1. PikaPython 概念速览](#1-pikapython-概念速览)
- [2. 运行 Python 脚本的机制](#2-运行-python-脚本的机制)
- [3. C 函数绑定机制 (.pyi -> 预编译 -> C 实现)](#3-c-函数绑定机制-pyi---预编译---头文件初始化函数---c-实现)
- [4. 基于当前工程的调用与测试实践](#4-基于当前工程的调用与测试实践)
- [5. 语法与特性限制清单](#5-语法与特性限制清单)
- [6. CPython 扩展体系 vs PikaPython 绑定体系对比](#6-cpython-扩展体系-vs-pikapython-绑定体系对比)
- [7. 迁移实施分阶段路线图](#7-迁移实施分阶段路线图)
- [8. 示例（概念性）代码与工作流片段](#8-示例概念性代码与工作流片段)
- [9. 风险、陷阱与规避建议](#9-风险陷阱与规避建议)
- [10. 后续可演进方向](#10-后续可演进方向)
- [附录 A: 关键能力对照表](#附录-a-关键能力对照表)
- [附录 B: 常见调试技巧](#附录-b-常见调试技巧)

---

## 1. PikaPython 概念速览
PikaPython（又称 PikaScript）是面向 MCU/嵌入式/资源受限场景的轻量级 Python 解释执行/静态打包方案，特点（当前仓库处于“脚本模式”，交互 Shell 被注释，详见 2.9）：
- 体积小：核心几十 KB~百余 KB 级。
- 可将 Python 源（或中间形式）打包为 C 代码参与编译（减少运行时解析开销，利于免文件系统部署）。
- 通过 .pyi stub 描述模块 / 类 / 方法签名，实现自动生成 C API 适配层，达到“声明即绑定”。
- 面向对象模型比 CPython 精简；内存采用手动/对象池/引用计数混合策略（实现细节依版本）。
- 典型启动方式：C 端 `pikaScriptInit()` 初始化根对象 -> 执行交互式 Shell 或运行预置脚本。

本仓库当前最小示例：
- `pikapython-linux/main.c`:
```c
PikaObj* root = pikaScriptInit();
// pikaScriptShell(root);  // 已注释，禁用 REPL
obj_deinit(root);
```
- `pikapython-linux/pikapython/main.py`:
```python
import PikaStdLib
print('hello pikascript')
```
说明：原始模板支持交互 Shell；当前版本已注释 `pikaScriptShell`，因此运行后仅执行 `main.py` 顶层语句并退出，没有 REPL。

---

## 2. 运行 Python 脚本的机制（基于当前仓库 `pikapython-linux` 实例）
本节不再给出抽象流程，而是直接对应仓库现有目录与脚本，说明“如何调用 / 如何运行”。

### 2.1 目录关键文件
| 路径 | 作用 |
|------|------|
| `pikapython-linux/main.c` | C 入口：初始化解释器（当前不进入 Shell，因为 `pikaScriptShell` 已注释） |
| `pikapython-linux/pikapython/main.py` | Python 启动脚本（被预编译器嵌入） |
| `pikapython-linux/pikapython/*.pyi` | 模块接口（如 `PikaStdLib.pyi` 等） |
| `pikapython-linux/pikapython/rust-msc-latest-win10.exe` | 预编译器（wine 运行） |
| `pikapython-linux/make.sh` | 一键：生成 build 目录 -> 运行预编译器 -> CMake 编译 -> 执行 |
| `pikapython-linux/CMakeLists.txt` | 汇集生成的 C 文件与核心库编译成可执行 `pikapython` |

### 2.2 实际构建-运行链路
`make.sh` 关键步骤（已存在脚本）：
1. 若无 `build/`：创建并运行 `cmake ..` 生成构建系统（首次配置头文件和依赖）。
2. 进入 `pikapython/` 目录，执行：`wine rust-msc-latest-win10.exe`
     - 作用：扫描 `main.py` 与 `.pyi`，输出到 `pikascript-api/`（生成 `pikaScript.c/h`、类头文件等）。
3. 返回根目录进入 `build/`，执行 `make`，链接生成可执行文件 `./build/pikapython`。
4. 运行可执行：加载内嵌脚本；若启用 Shell 会进入交互；当前配置执行完脚本即退出。

终端行为：执行脚本后你会看到 `hello pikascript`（来自 `main.py`），随后可以在交互环境继续输入 Python 语句（受支持子集）。

### 2.3 添加/修改 Python 脚本的正确方式
- 修改 `pikapython-linux/pikapython/main.py` 顶层语句：重新执行 `make.sh`，预编译器会重写 `pikaScript.c`，使 `pikaScriptInit()` 嵌入新的代码段。
- 添加新模块接口：在同目录放置 `MyMod.pyi` 并在 `main.py`里 `import MyMod`；再运行 `make.sh` 生成对应头文件。
- 添加纯 Python 逻辑：若无需 C 绑定，可直接写 `helper.py` 并在 `main.py` 中 `import helper`（确保预编译器能发现）。

### 2.4 交互 Shell 调用（仅在启用 `pikaScriptShell` 时）
当前仓库禁用 Shell，本小节仅作为启用后参考：
- 启用方式：取消注释 `pikaScriptShell(root);`。
- 运行后在 `>>>` 提示符可输入：`import PikaStdLib`; `mem = PikaStdLib.MemChecker()`; `mem.max()`。

### 2.5 底层初始化函数回顾
生成的 `pikaScriptInit()`（预编译结果）逻辑大致：
```c
PikaObj* pikaScriptInit(){
        PikaObj* pikaMain = newRootObj("pikaMain", New_PikaMain);
        obj_run(pikaMain, "print('hello pikascript')\n");
        return pikaMain;
}
```
随着 `main.py` 顶层语句增加，该字符串块会累积行。

### 2.6 内存与生命周期（针对当前工程）
- 启用 Shell：`pikaScriptShell` 返回后调用 `obj_deinit(root)` 释放对象。
- 当前脚本模式：`pikaScriptInit()` 执行后立即 `obj_deinit(root)` 并退出。
- 若后续扩展：在交互阶段创建的大对象未显式释放，也会在最终 `obj_deinit` 时释放，但建议对绑定的硬件资源类提供 `close()` 方法。

### 2.7 最简“如何运行”一览
在仓库根目录：
```
sh pikapython-linux/make.sh
```
完成预编译、编译与运行。

若仅重新打包脚本（不改 CMake 配置）：
```
cd pikapython-linux/pikapython && wine rust-msc-latest-win10.exe && cd ..
cd build && make && cd .. && ./build/pikapython
```

（建议仍统一用 `make.sh` 以减少遗漏步骤。）

### 2.8 实际运行输出示例（当前环境）
执行：`bash pikapython-linux/make.sh`（首次或脚本内容未改动时的典型输出，截取核心片段）
```
-- The C compiler identification is GNU 11.4.0
-- The CXX compiler identification is GNU 11.4.0
... (CMake 配置若干行)
(pikascript) packages installed:
        pikascript-core==v1.12.2
        PikaStdLib==v1.12.2

(pikascript) pika compiler:
    scanning main.py...
        binding PikaStdLib.pyi...
        binding PikaStdTask.pyi...
        binding PikaStdData.pyi...
        binding PikaDebug.pyi...
    compiling main.py...
    linking pikascript-api/pikaModules.py.a...
    loading pikaModules_py_a[]...
[  2%] Building C object CMakeFiles/pikapython.dir/pikapython/pikascript-api/__asset_pikaModules_py_a.c.o
... (核心库与标准模块持续编译进度)
[100%] Linking C executable pikapython
======[pikascript packages installed]======
pikascript-core==v1.12.2 (...timestamp...)
PikaStdLib==v1.12.2
===========================================

~~~/ POWERED BY \~~~
~  pikascript.com  ~
~~~~~~~~~~~~~~~~~~~~
hello pikascript
>>>   # （该 REPL 提示符在当前禁用 Shell 的仓库设置下不会出现）
```
关键阶段说明：
- `packages installed`：预编译器识别已打包核心与标准库版本。
- `binding *.pyi`：解析并生成对应头文件/绑定代码。
- `loading pikaModules_py_a[]`：将聚合的 Python 模块数组链接进最终二进制。
- `[xx%] Building`：标准 C 源与自动生成文件被编译。
- `hello pikascript`：来自 `main.py` 顶层打印；若启用 Shell 则随后出现 REPL 提示符 `>>>`。

（启用 Shell 时可验证交互：`print('hello')` -> 输出 hello；`Ctrl+C` 退出。）

版本记录（当前验证得到）：`pikascript-core==v1.12.2`, `PikaStdLib==v1.12.2`。

### 2.9 运行模式差异（脚本模式 vs 交互模式）
当前 `main.c`：
```c
PikaObj* root = pikaScriptInit();
// pikaScriptShell(root);
obj_deinit(root);
```
含义：仅执行 `main.py` 顶层代码，自动结束（适合批量/自测）。

切换为交互：
```c
PikaObj* root = pikaScriptInit();
pikaScriptShell(root);  // 启用 REPL
obj_deinit(root);
```
模式对比：
| 模式 | 输入能力 | 退出方式 | 适用场景 |
|------|----------|----------|----------|
| 脚本模式（当前） | 否 | 代码执行完毕 | 自动化、自测、固件分发 |
| 交互模式 | 是 (REPL) | Ctrl+C / 关闭终端 | 调试、现场诊断、探索 |

推荐实践：
- 保持脚本模式用于 CI 与批处理。
- 本地调试临时启用 Shell，结束后再注释。
- 若需编译时切换，可用：
    ```c
    #ifdef ENABLE_PIKA_SHELL
        pikaScriptShell(root);
    #endif
    ```
    并在构建命令中添加 `-DENABLE_PIKA_SHELL` 宏（或通过头文件统一）。

### 2.10 使用 `run_pika.sh` 快速运行任意脚本
为支持“将仓库中任意一个独立 Python 脚本（例如实验/测试脚本）快速打包运行”而不手工编辑 `main.py`，新增辅助脚本：`./run_pika.sh`。

核心需求：临时以某脚本内容替换 `pikapython-linux/pikapython/main.py`，调用现有 `make.sh` 完成预编译 + 构建 + 执行，然后自动恢复原文件。

#### 2.10.1 工作流程
1. 备份原始 `main.py`（生成 `main.py.bak.<timestamp>`）
2. 复制用户指定脚本内容覆盖 `main.py`
3. `cd pikapython-linux && bash make.sh`（内部调用预编译器 + make + 运行）
4. 无论成功/失败均恢复原始 `main.py`（使用 `trap` 保证异常/中断也恢复）

#### 2.10.2 用法示例
在仓库根目录执行：
```bash
./run_pika.sh file_create/test_code.py
```
（脚本需存在且以 `.py` 结尾，路径相对仓库根。）

调试：
```bash
VERBOSE=1 ./run_pika.sh file_create/test_code.py
```
说明：
- 现行策略始终强制干净构建（删除并重新生成 build/），`KEEP_BUILD` 已废弃。
- `VERBOSE=1`：输出调试日志（构建步骤、命令等）。

#### 2.10.3 典型输出片段
```
[run_pika] 备份原 main.py -> .../main.py.bak.2025...
[run_pika] 复制 file_create/test_code.py -> main.py
[run_pika] 执行编译与运行: bash make.sh
... (预编译与编译输出) ...
[run_pika] 执行完成，恢复 main.py 后退出
```

#### 2.10.4 错误与退出码
| 场景 | 表现 | 处理 |
|------|------|------|
| 缺少参数 | 输出 `[ERROR] 需要且仅需要 1 个参数` | 退出码 ≠ 0，已恢复 main.py |
| 文件不存在 | 输出 `[ERROR] 找不到文件` | 退出码 ≠ 0，已恢复 |
| 非 .py | 输出后缀错误 | 退出码 ≠ 0，已恢复 |
| 构建失败 | 输出 `[ERROR] 构建或运行失败 (退出码 X)` | 退出码传递 make/sh 实际值 |

#### 2.10.5 适用场景
- 临时验证：快速跑 `file_create/` 下新生成的 Python 片段
- 自测与迭代：无需手动编辑真正的 `main.py`
- 批处理扩展（后续可加）：遍历某目录下全部 `test_*.py` 收集结果

#### 2.10.6 后续可扩展建议
- 增加 `--keep` 参数：执行后不恢复，用于调试多次增量构建
- 增加批量模式：`--glob 'file_create/*.py'` 逐个运行并聚合报告
- 标准输出结构化：将 `[SELFTEST]` / `[BENCH]` 行抽取写入 `logs/auto/summary.json`
- 与 MCP 客户端联动：由上位机自动调用脚本并进行差异分析

通过该脚本，开发者可最小成本验证任意单体 Python 逻辑是否能被 PikaPython 预编译 & 运行，为后续自动化测试管线奠定基础。

---

## 3. C 函数绑定机制 (.pyi -> 预编译 -> 头文件/初始化函数 -> C 实现)
本节按官方开发文档流程重新阐述，纠正“手写注册表/宏”这一错误描述。PikaPython 的 C 绑定不需要手写模块注册宏，核心是：使用 `.pyi` 作为“模块接口（package interface）”声明类与方法，预编译器（例如仓库中提供的 `rust-msc-latest-win10.exe`）扫描这些接口，自动生成对应的头文件与初始化脚本（`pikaScript.c/h`）。

### 3.1 基本概念
| 名称 | 含义 | 说明 |
|------|------|------|
| 模块接口 (.pyi) | 使用 Python 语法 + `pass` / `...` 的类与方法声明文件 | 仅声明，不含实现；是“面向接口”定义 |
| 模块实现 (.c) | 放在 `pikascript-lib/<Module>/` 下的 C 源文件 | 为接口中声明的方法提供真正逻辑 |
| 预编译器 | `rust-msc-*` 可执行工具 | 解析 `.py` / `.pyi`，生成头文件、对象构造、初始化代码 |
| 初始化脚本 | `pikaScript.c` 中的 `pikaScriptInit()` | 含把 `main.py` 顶层语句转成 `obj_run()` 字符串的函数 |
| 生成的类头文件 | `<Module>_<Class>.h` | 声明构造函数 `New_<Module>_<Class>` 与每个方法的 C 原型 |

### 3.2 Device 模块例子（官方示例结构）
`Device.pyi`：
```python
class LED:
    def on(self):
        pass
    def off(self):
        pass

class Uart:
    def send(self, data: str):
        pass
    def setName(self, name: str):
        pass
    def printName(self):
        pass
```
在 `main.py` 中：
```python
import Device
import PikaStdLib

led = Device.LED()

mem = PikaStdLib.MemChecker()
```
运行预编译器（双击或在 CI 中调用）后，会在 `pikascript-api` 目录生成：
- `Device_LED.h`, `Device_Uart.h` （每个类一个头文件）
- `PikaMain.h`（由 `main.py` 顶层代码生成的主类）
- `pikaScript.c/h`（包含 `pikaScriptInit()`，内部把 `main.py` 的顶层语句拼接为 `obj_run()` 代码块）

### 3.3 Math 模块开发全流程示例
1. 新建接口 `Math.pyi`：
   ```python
   class Adder:
       def byInt(self, a: int, b: int) -> int:
           pass
       def byFloat(self, a: float, b: float) -> float:
           pass

   class Multiplier:
       def byInt(self, a: int, b: int) -> int:
           pass
       def byFloat(self, a: float, b: float) -> float:
           pass
   ```
2. 在 `main.py` 中 `import Math` 以触发预编译器处理它。
3. 运行预编译器 -> 生成 `Math_Adder.h` 与 `Math_Multiplier.h`，内容（节选）：
   ```c
   /* Math_Adder.h */
   /* Warning! Don't modify this file! */
   PikaObj *New_Math_Adder(Args *args);
   double Math_Adder_byFloat(PikaObj *self, double a, double b);
   int Math_Adder_byInt(PikaObj *self, int a, int b);
   ```
4. 在 `pikascript-lib/Math/` 下分别实现 `Math_Adder.c`, `Math_Multiplier.c`：
   ```c
   #include "pikaScript.h"
   double Math_Adder_byFloat(PikaObj *self, double a, double b) { return a + b; }
   int    Math_Adder_byInt  (PikaObj *self, int a, int b)     { return a + b; }
   double Math_Multiplier_byFloat(PikaObj *self, double a, double b){ return a * b; }
   int    Math_Multiplier_byInt  (PikaObj *self, int a, int b){ return a * b; }
   ```
5. 编译工程——链接生成的头文件 + 核心库即可。无需手写任何“注册表”宏。
6. 运行后 Python 侧：
   ```python
   import Math
   adder = Math.Adder()
   print(adder.byInt(1, 2))
   ```

### 3.4 生成物解析
- `New_<Module>_<Class>`：构造函数，返回该类对应的 `PikaObj*`。
- `<Module>_<Class>_<method>`：方法实现的 C 原型，参数类型按照类型注解映射转换。
- `pikaScriptInit()`：在内部创建根对象并执行 `main.py` 顶层语句（以字符串形式拼接）。

### 3.5 类型注解与 C 类型映射
| Python 注解 | 对应 C 原生/抽象类型 | 说明/注意 |
|-------------|----------------------|-----------|
| `int` | `int` | 可能为固定宽度（实现一般 32-bit）|
| `int64` | `int64_t` | 显式 64 位整型 |
| `float` | `double` | 使用双精度传递 |
| `str` | `char*` (返回需缓存) | 返回局部字符串需用 `obj_cacheStr(self, s)` 缓存 |
| `bytes` | 返回 `Arg*` / 形参 `uint8_t*` | 返回时用 `arg_newBytes(bytes, len)` 构造 |
| `pointer` | `void*` | 传递底层指针句柄；注意生命周期 |
| `any` | `Arg*` | 通用容器，需按期望类型再解析 |
| `ClassName` | `PikaObj*` | 指向另一个对象实例 |

返回值注意：
- `str`：不要返回栈上临时 `char buf[]`；使用 `obj_cacheStr(self, buf)`。
- `bytes`：需要长度信息，使用 `arg_newBytes(data, len)` 返回。
- 指针类型：确保上层不会在对象释放后继续访问；必要时增加 `close()/deinit()` 接口。

### 3.6 设计约束与实践
- `.pyi` 只做声明：业务逻辑完全在 C 实现。保持接口瘦身，避免高频创建临时对象的 API 形态。
- 触发生成的条件是 `import`：未被 `main.py`（或其他入口）引用的模块不会生成头文件（可在构建脚本中做显式集合）。
- 方法命名建议：`<verb><Noun>`，返回错误码时用 `int`，错误细节另行查询或使用约定的 `last_error` 属性。
- 变更接口必须同步删除旧生成物防止“幽灵声明”。

### 3.7 与 CPython C API 的重新对比
| 方面 | CPython | PikaPython |
|------|---------|------------|
| 绑定入口 | 手写 `PyModuleDef`/`PyMethodDef` | 自动：`.pyi` + 预编译器生成头文件 |
| 注册方式 | 运行时初始化函数填表 | 预编译静态生成，无运行期反射注册表手写 |
| 方法实现 | 使用 `PyObject*` 操作堆栈/引用计数 | 直接按生成的 C 函数签名实现 |
| 初始化脚本 | 解释器读取 .py 文件 | `pikaScriptInit()` 内嵌脚本字符串 |
| 类型注解 | 运行时可忽略（PEP484） | 代码生成阶段用于决定 C 原型 |
| 动态加载 | 支持 `.so/.pyd` | 不支持动态加载（需重编译） |

小结：PikaPython 的绑定流程核心是“声明 (pyi) -> 预编译器自动产出（头文件+初始化） -> 填写 C 实现”，开发者无需关心注册表或反射细节。
---

## 4. 基于当前工程的调用与测试实践
本仓库现状：`main.py` 仅打印一行文本。以下给出在当前结构上“如何调用已有模块 / 添加新模块 / 编写测试”的可执行式指导。

### 4.1 现有调用路径
运行 `make.sh` 后执行的顺序：
1. 预编译器把 `main.py` 顶层 `print('hello pikascript')` 嵌入 C。
2. 可执行启动进入 Shell 输出该行。
3. 在 Shell 输入：
    ```
    import PikaStdLib
    mem = PikaStdLib.MemChecker()
    mem.max()
    mem.now()
    ```
    用于查看内存使用（验证运行环境基础功能）。

    REPL 操作补充：
    - 提示符 `>>>` 表示可输入单行 Python；多行结构（如函数定义）暂不推荐在交互中书写，优先放入 `main.py` 重新打包。
    - 退出方式：`Ctrl+C`（发送中断离开 REPL 并结束进程）或关闭终端。若 REPL 捕获中断未退出，可再按一次 `Ctrl+C`。

### 4.2 添加一个测试脚本（扩展 main.py）
在 `pikapython-linux/pikapython/main.py` 追加（示例）：
```python
import PikaStdLib
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()
print('mem used now:')
mem.now()
```
步骤：重新执行 `sh pikapython-linux/make.sh`，输出的 `pikaScriptInit()` 将包含新打印。运行后自动执行这些语句，无需手动输入。

### 4.3 新增 C 绑定模块并测试
1. 在 `pikapython-linux/pikapython/` 创建 `Math.pyi`：
    ```python
    class Adder:
         def byInt(self, a:int, b:int)->int:...
    ```
2. 在 `main.py` 顶部添加：`import Math`
3. 运行 `make.sh` 生成 `pikascript-api/Math_Adder.h`
4. 在（若存在）`pikascript-lib/Math/` 目录下创建 `Math_Adder.c`：
    ```c
    #include "pikaScript.h"
    int Math_Adder_byInt(PikaObj* self, int a, int b){ return a + b; }
    ```
5. 再次 `make` & 运行后，在 Shell：
    ```
    import Math
    adder = Math.Adder()
    adder.byInt(1,2)
    ```

### 4.4 集成“内置测试”方法
若希望自动执行一组断言，可在 `main.py` 末尾加入：
```python
def _selftest():
     import Math
     a = Math.Adder()
     assert a.byInt(2,3) == 5
     print('[SELFTEST] Math.Adder.byInt OK')

_selftest()
```
这样预编译后启动就会跑测试。失败会抛出异常（在当前 Shell 输出栈信息）。

### 4.5 与上位机/自动化结合思路
- 通过外层（本项目已有的 MCP 客户端）运行可执行，捕获标准输出解析 `[SELFTEST]` 前缀行，生成测试报告。
- 若需要区分阶段：使用环境变量/宏控制是否调用 `_selftest()`（例如在 `main.py` 中判断一个占位变量）。

### 4.6 运行后交互补充测试
即便 `_selftest()` 已执行，也可以后续人工或脚本注入：
```
adder = Math.Adder()
for i in range(10):
     adder.byInt(i, i+1)
```
用于简单压力尝试。

### 4.7 推荐最小自动化约定
| 目标 | 约定 |
|------|------|
| 功能断言 | `_selftest()` 中使用 `assert` |
| 性能烟囱 | 在 `_selftest()` 中循环 N 次并打印 `[BENCH] <name> <loops> <ticks>`（后续扩展获取 tick） |
| 内存监控 | 前后调用 `mem.max()`/`mem.now()` 打印 `[MEM]` 前缀行 |
| 错误用例 | 使用 `try/except` 打印 `[NEG]` 前缀行 |

### 4.8 何时需要拆分测试文件
当前结构简单，将测试嵌入 `main.py` 成本最低；若模块增多：
```
tests/
  test_math.py   # 仅含断言函数
```
在 `main.py`：
```python
import test_math
test_math.run()
```
仍经预编译器打包，不需要文件系统加载。

### 4.9 常见问题定位
| 现象 | 排查 |
|------|------|
| 运行看不到新代码 | 确认已执行 `wine rust-msc-latest-win10.exe`（`make.sh` 是否被中断） |
| 新增 .pyi 未生效 | 是否在 `main.py` `import` 了该模块 |
| 方法符号未定义链接错误 | 检查对应 `<Module>_<Class>.c` 是否实现所有生成头文件列出的函数 |
| Shell 调用崩溃 | 打印参数，确认未传入超出范围的整数/空指针 |

---

## 5. 语法与特性限制清单
(具体需依据所用 PikaPython 版本，这里列常见限制范畴)

### 5.1 语法子集
- 支持：基本的赋值/if/while/for/break/continue/def/class/return
- 可能不支持或有限：
  - 推导式（list/dict/生成器）可能只支持简单形式或不支持嵌套复杂表达式
  - 生成器函数 `yield`/`async/await` 通常不支持
  - 装饰器：可能仅支持最简单函数装饰器或不支持参数化装饰器
  - 上下文管理器 `with`：若无实现 `__enter__/__exit__` 机制则不可用
  - 异常体系：支持基础 `try/except`，但异常类集合精简
  - 类型注解：仅作静态信息，不参与运行检查

### 5.2 内置类型与差异
| 类型 | 支持情况 | 备注 |
|------|----------|------|
| int/float/str/bool | 支持 | int 可能固定宽度(例如 32-bit) |
| list/dict | 支持 | 容量与嵌套层数受内存限制 |
| tuple | 可能支持只读结构 | 赋值特性有限 |
| bytes/bytearray | 可能支持基础操作 | 高级切片/方法受限 |
| set/frozenset | 常缺失 | 需用 dict/list 替代 |
| complex | 通常不支持 | 需用两个 float 表示 |

### 5.3 标准库与模块
- 没有完整 `os`, `sys`（可能只有部分字段），无 `multiprocessing`、`threading`（或自定义任务库）。
- 使用 PikaStdLib / PikaStdData / PikaStdTask 提供：IO、字符串、任务调度、数据结构工具。

### 5.4 内存/性能注意
- 避免大型临时列表；循环中尽量原地复用对象。
- 避免深递归（栈有限）。
- 字符串拼接建议使用列表收集后 join（若 join 支持）。
- 避免使用大量动态属性赋值（对象头可能较小）。

### 5.5 其他差异
- 没有 JIT；所有“优化”依靠 C 绑定 + 静态打包。
- 时间/随机等功能需依赖底层适配层实现。

---

## 6. CPython 扩展体系 vs PikaPython 绑定体系对比
| 维度 | CPython 扩展 | PikaPython 绑定 |
|------|---------------|------------------|
| 构建心智 | 运行时加载动态模块 | 编译期固化静态注册 |
| 开发入口 | `PyModuleDef`, `PyMethodDef` | `.pyi` + 自动代码生成 |
| ABI 稳定性 | 与解释器版本绑定 | 与框架内部结构绑定（更轻，但升级需重编译） |
| 调试工具 | gdb + Python 调试符号 | gdb + 直接 C 函数；对象模型更简单 |
| 运行部署 | 依赖文件系统 + 动态链接器 | 固件整体刷写 / 静态二进制 |
| 性能热点优化 | 可用 Cython / cffi / PyPy | 直接写 C 并通过 stub 暴露 |
| 社区生态 | 海量包 | 需手工移植 / 使用 PikaStd* |

### 6.1 迁移策略
| 场景 | 策略 |
|------|------|
| 仅算法内核 (纯计算) | 直接翻译为 C 函数 + 简单 stub 暴露 |
| 依赖复杂标准库 | 拆分：核心算子迁移；外围逻辑在上位机保留 |
| 需要动态 import 插件 | 预先打包所有可能模块；或建立条件编译开关 |
| 大量类层级 + 动态元编程 | 扁平化；预生成必要方法；避免 `metaclass` |

---

## 7. 迁移实施分阶段路线图
1. 评估与切分
   - 标记现有 CPython 工程中的：a) 算法热点 b) I/O层 c) 依赖的第三方库集合。
   - 选择首批 MVP 功能（计算密集、依赖少）。
2. Stub 设计
   - 按功能模块设计 `.pyi` 接口：保持输入输出尽量为基本类型。
   - 约束：避免可变参数、复杂容器（嵌套 dict[list[dict]]）。
3. C 内核实现
   - 依据 `.pyi` 生成适配层后，在 C 中填充核心逻辑。
   - 写最小测试脚本验证参数边界。
4. 自动化测试体系
   - 设计测试入口 `test_main.py`，包含：功能断言 + 性能冒烟 + 错误注入。
   - 规划与上位机（如本项目的 MCP 客户端）联动，采集日志。
5. 性能与内存分析
   - 循环调用关键函数 N 次测峰值内存；记录执行时间（若有 tick 接口）。
6. 扩展与优化
   - 模块化拆分；封装统一错误码；抽象公共参数解析逻辑。
7. 文档与规范
   - 形成接口文档 + 移植注意事项列表；内核函数命名统一前缀。

---

## 8. 示例（概念性）代码与工作流片段
### 8.1 示例 `.pyi`
```python
# sensors.pyi
class SensorDrv:
    def init(port: int, baud: int) -> int: ...  # 返回0表示成功
    def read() -> int: ...
```
### 8.2 C 实现片段（概念）
```c
// sensor_drv.c
int sensor_init_impl(int port, int baud) { /* hw init */ return 0; }
int sensor_read_impl() { /* read register */ return 123; }
```
适配层由生成工具基于 `.pyi` 自动创建，对外在 Python 中：
```python
import SensorDrv
SensorDrv.init(1, 115200)
val = SensorDrv.read()
```

### 8.3 性能冒烟脚本
```python
import SensorDrv
for i in range(1000):
    SensorDrv.read()
print('done')
```

### 8.4 失败场景测试
```python
try:
    SensorDrv.init(-1, 0)
except Exception as e:
    print('invalid param caught', e)
```
(若框架以返回码而非异常表达错误，则改为断言返回值)

---

## 9. 风险、陷阱与规避建议
| 类别 | 风险 | 规避 |
|------|------|------|
| 语法| 使用不支持的高级语法（装饰器嵌套、生成器） | 预先 lint：建立“Pika 语法白名单”规则 |
| 内存| 容器扩张或递归爆栈 | 设定最大测试规模；监控失败回退 |
| 绑定| .pyi 与 C 实现签名不一致 | 建立脚本自动 diff stub 与实现函数统计 |
| 维护| 随版本升级宏/内部结构变化 | 锁定版本；引入升级兼容清单 |
| 性能| 热点仍在 Python 层 | 按调用次数/耗时排行筛选再下移 C |
| 可测| 嵌入式难以断言复杂行为 | 引入 PC 仿真（Linux 版）先跑用例 |
| 可靠| 错误处理路径缺失 | 统一返回码/错误码表，写入文档 |

---

## 10. 后续可演进方向
- 生成层增强：自动从 `.pyi` 推导参数合法性检查模板。
- 静态分析：对 Python 侧脚本做子集合规扫描（禁用黑名单语法）。
- 基准测试框架：提供统一 `benchmark()` 装饰器（若未来支持简单装饰器）。
- 工具集成：在 MCP 工具集中加入自动 stub 验证、打包、差异报告工具。
- 混合编译：探索将部分 Python 算法 AST 转换为等价 C（超出当前 stub 能力）。

---

## 附录 A: 关键能力对照表
| 能力 | CPython | PikaPython | 迁移建议 |
|------|---------|------------|----------|
| 运行模式 | 文件/REPL/动态模块 | 静态打包/REPL | 优先静态打包减少解析开销 |
| 包管理 | pip | 手工合入 | 保留最小功能子集 |
| 异常 | 完整层级 | 精简 | 用错误码替代复杂异常语义 |
| 多线程 | GIL + thread 支持 | 受限/任务调度 | 拆解为事件循环或轮询 |
| 大整数 | 任意精度 | 可能受限 | 提前截断或使用字符串处理 |

## 附录 B: 常见调试技巧
| 目标 | 做法 |
|------|------|
| 查看对象属性 | 在 Shell 中 `dir(obj)`（若实现）或打印关键字段 |
| 性能热点 | 手工计时：记录硬件 tick/loop 次数 |
| 内存泄漏 | 多次调用后查看 `mem` 命令（若提供）或加打印钩子 |
| 绑定出错 | 检查生成的 `__pikaBinding.c` 中方法表是否包含目标函数 |
| 初始化失败 | 确认 `pikaScriptInit()` 返回非 NULL 并检查配置宏 |

---

## 总结
PikaPython 适合在资源受限设备上承载一小部分 Python 逻辑并通过 `.pyi` 高效暴露 C 实现。迁移时核心是“接口瘦身 + 算法下沉 + 静态打包 + 语法子集约束”。本报告提供了运行机制、绑定流程、测试组织、语法限制与对比策略，可作为后续自动化工具与加速平台建设的知识基线。