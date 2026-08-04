# 模块与 binding

- 当前模块链路：
  - 由包目录、`skills/pikapython-cli/assets/package-catalog.json`、
    `pikapython-cli install` 和 `pikapython-cli build` 共同完成；
  - target 不直接扫描源码仓库。

## 安装后的项目布局

- 产品包索引：
  - 路径是 `skills/pikapython-cli/assets/package-catalog.json`；
  - export destination 只允许以下布局。

```text
<project>/
├── main.py
├── <python-module>.py
├── <binding-module>.pyi
├── pikapython-kernel/
│   ├── <flat-kernel-source>.c
│   └── <flat-kernel-header>.h
├── pikapython-packages/
│   └── <package-name>/
│       ├── <module-source>.c
│       └── <module-header>.h
├── .pikapython/
│   ├── install-manifest.json
│   └── source/
└── pikapython-generated/
```

- 项目根目录的公开包文件只允许 `.py` 或 `.pyi`。
- `pikapython-kernel/`：
  - 只允许一层扁平的 `.c`、`.h` 或 `.inc`。
- `pikapython-packages/<package-name>/`：
  - 只允许一层扁平的 `.c` 或 `.h`。
- install 拥有并更新 catalog 声明的目标：
  - 手工文件与受管目标冲突时返回 `package_file_conflict`；
  - 不应手工修改已安装副本并把它当作包源。

## 纯 Python 模块

当前 application prebuild 收集：

- 项目根目录的 `main.py`：
  - 映射为入口模块 `__main__`；
  - 缺失时返回 `main_module_missing`。
- 已安装依赖导出的根目录 `*.py`：
  - 文件名 stem 是模块名；
  - 与其它模块重名时返回 `module_name_conflict`。

开发可分发纯 Python 模块时：

1. 在包源仓库中维护模块 `.py`。
2. 在 `skills/pikapython-cli/assets/package-catalog.json` 中把它导出到项目根目录。
3. 使用精确三段版本声明依赖。
4. 重新运行 `pikapython-cli install`。
5. 从 `main.py` 或其它已安装模块导入它。
6. 重新运行 `pikapython-cli build --prebuild-only`。

- 项目本地 Python 文件：
  - 当前 prebuild 不会自动收集项目根目录任意新增的辅助 `.py`；
  - 除 `main.py` 外，纯 Python 模块必须来自安装清单；
  - 不能依赖 target 的文件系统扫描。

## C 模块与 `.pyi`

C 模块通常包含三层：

- Python 外观模块：
  - 例如 `math.py`；
  - 可以导入底层 binding 模块并提供 Python API。
- Python 3 stub：
  - 例如 `_math.pyi`；
  - 声明 Python 可见常量、函数、类、构造器和方法；
  - 是主机 prebuild 输入，不进入目标固件。
- C 实现：
  - 例如 `math_module.c`；
  - 实现生成 header 声明的 callback；
  - 使用 `PikaBindingCall` 读取参数；
  - 使用 `PikaBindingValue` 返回值和稳定状态。

最小声明示例：

```python
from typing import Any

pi: float = 3.141592653589793

def ceil(x: Any) -> int: ...
```

对应实现包含生成 header，并实现确定性 callback：

```c
#include "_math_binding.h"

PikaStatus pika_binding__math_ceil(
    const PikaBindingCall* call,
    PikaBindingValue* result) {
    /* 校验参数并填写 result。 */
    return PIKA_STATUS_OK;
}
```

- callback 名称由 binding prebuild 根据模块、类和函数名生成：
  - 不手写另一套注册表；
  - 不依赖 V1 的对象 ABI 或生成器 API。
- C 源应包含 `pika_capability_config.h`：
  - package catalog 的 `capability` 字段把包绑定到已有 capability；
  - profile 未包含该能力时，包文件不会进入 prebuild 的 active 视图；
  - 磁盘上的安装副本和 `.pikapython/install-manifest.json` 保持不变；
  - 不通过 target define 强行恢复被裁剪的包。
- 新的 Python 可见语义需要新的 capability 时：
  - 先更新 owning capability 规格和配置；
  - 本指南不提供绕过 capability 图的注册方法。

可参考的当前实现：

- `packages/math/math.py`；
- `packages/math/_math.pyi`；
- `packages/math/math_module.c`；
- `packages/struct/`；
- `packages/hashlib/`。

## prebuild 过程

```bash
pikapython-cli install
pikapython-cli build --prebuild-only
```

prebuild 按以下顺序执行：

1. 读取 capability 配置和选中 profile。
2. 展开 capability 依赖闭包。
3. 基于安装清单构造 active 视图，过滤未启用 capability 所属包的文件。
4. 对 active `.pyi` 生成 binding descriptor、`*_binding.c` 和 `*_binding.h`。
5. 收集 `main.py` 和 active `.py`。
6. 生成冻结模块与 application 入口。
7. 生成 target 消费的源码和 include 清单。

## 生成物

`pikapython-generated/` 由 CLI 原子替换，不应手工编辑：

| 文件 | 用途 |
| --- | --- |
| `pika_capability_config.h` | capability prebuild 的原始生成头 |
| `capability-config.h` | target 统一包含的 capability 头副本 |
| `pika_capability_manifest.json` | profile、闭包、宏映射与资源限制 |
| `binding-descriptor.json` | 全部 active binding 描述集合 |
| `<module>_binding.c/.h` | 模块注册和 callback 边界 |
| `pikapython_program.c/.h` | 预构建程序数据 |
| `pikapython_modules.c/.h` | 冻结模块、binding 清单和 application 入口 |
| `program-image.json` | `__main__` 和源码模块摘要 |
| `source-manifest.json` | target 的 C 源、include、Python 模块和 binding 模块清单 |

- CMake 模板读取 `source-manifest.json`：
  - 按清单加入源码和 include 路径。
- Keil adapter 读取已安装目录和生成目录：
  - 同步固定受管组；
  - 不在 IDE 内运行 binding 或 capability 生成器。

- 后续参考：
  - 项目配置见[项目配置](project-configuration.md)；
  - 启动与平台边界见[平台移植与模板](porting-and-templates.md)。
