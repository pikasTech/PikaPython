# 项目配置

- `pikapython.yaml`：
  - 位于项目根目录；
  - 是依赖、包源、capability 和 target 的唯一项目配置。
- 当前 schema：
  - 版本固定为 `1`；
  - 未知字段会被拒绝。

## 完整结构

```yaml
version: 1
dependencies:
  - pikapython-kernel==2.0.0
packages:
  sourceUrl: https://github.com/pikasTech/PikaPython.git
  ref: v2
capability:
  configFile: .pikapython/source/config/pikapython-capabilities.yaml
  profile: runtime-full
targets:
  linux:
    adapter: cmake
    projectFile: CMakeLists.txt
    targetName: linux-console
    buildDirectory: .pikapython/build/linux
    outputDirectory: pikapython-generated
    arguments:
      - -DCMAKE_BUILD_TYPE=Release
defaultTarget: linux
```

## 依赖与包源

- `dependencies`：
  - 必须是列表；
  - 每项使用 `<name>==<version>`；
  - 包名使用小写字母、数字和连字符；
  - 版本使用三段版本号；
  - 同名依赖不能重复。
- `packages.sourceUrl`：
  - `pikapython-cli init` 默认写入公开产品仓；
  - 可以是本地 Git working tree；
  - 可以是远程 Git URL；
  - 相对本地路径按项目根目录解析。
  - 本地路径必须指向 working tree 根目录，不能指向其子目录或普通解压目录。
- `packages.ref`：
  - `pikapython-cli init` 默认写入公开产品分支 `v2`；
  - 可选；
  - 可以解析 branch、tag 或 commit；
  - 未设置时使用包源的 `HEAD`。

- 本地归档：
  - 应优先恢复为正式 Git checkout；
  - 只有归档文件时，在归档根目录初始化 Git 并提交后才能作为本地包源；
  - 使用本地 `HEAD` 时运行 `pikapython-cli config unset packages.ref`；
  - 未提交改动不会进入 `install` 创建的源码快照。

受控修改命令：

```bash
pikapython-cli add pikapython-math==1.0.0
pikapython-cli remove pikapython-math
pikapython-cli list
pikapython-cli config set packages.sourceUrl /absolute/path/to/pikapython
pikapython-cli config set packages.ref <commit-or-tag>
pikapython-cli config unset packages.ref
pikapython-cli install
```

- 普通用户保持默认公开源和 `v2`；
- 内核开发者把 `sourceUrl` 显式改为本地研发 Git 根；
- 开发当前研发仓 HEAD 时取消 `packages.ref`；
- CLI 不根据当前目录或本地仓库存在性自动切换到研发源。

- `add` 和 `remove` 只修改配置：
  - 修改后必须重新运行 `install`。
- `install` 使用 Git archive 生成不可变快照：
  - 包文件按 `skills/pikapython-cli/assets/package-catalog.json` 发布；
  - 源快照保存为 `.pikapython/source`；
  - 安装事实保存为 `.pikapython/install-manifest.json`。

## capability

`capability` 必须同时包含 `configFile` 和 `profile`：

```bash
pikapython-cli config capability \
  --config-file .pikapython/source/config/pikapython-capabilities.yaml \
  --profile embedded-app
```

- `configFile`：
  - 必须是项目内的可移植相对路径；
  - 不能是绝对路径；
  - 不能包含 `..` 或反斜杠。
- `profile`：
  - 必须存在于 capability 配置的 `profiles`；
  - prebuild 解析依赖闭包并生成全部宏值；
  - 未知 profile、未知 capability、循环依赖或不支持能力会在生成目标文件前失败。
- target toolchain：
  - 只读取 `pikapython-generated/capability-config.h` 和生成清单；
  - 不直接定义 capability `*_ENABLE` 宏；
  - 不以 CMake option、Keil define 或 QEMU 参数建立旁路。

当前仓库配置提供的 profile 可用以下命令和配置核对：

```bash
pikapython-cli config show
python3 - <<'PY'
from pathlib import Path

import yaml

path = Path(".pikapython/source/config/pikapython-capabilities.yaml")
document = yaml.safe_load(path.read_text(encoding="utf-8"))
print(*document["profiles"], sep="\n")
PY
```

profile 是能力根集合，不是 target，也不等同于线性兼容等级。

## targets

每个 `targets.<id>` 都有一个 adapter。target ID 只允许小写字母、数字和连字符。

| 字段 | 作用 |
| --- | --- |
| `adapter` | 配置解析接受 `cmake`、`keil`、`make`、`iar` |
| `projectFile` | CMake 的 `CMakeLists.txt` 或 Keil 的 `.uvprojx` |
| `targetName` | CMake build target 或 Keil TargetName |
| `toolExecutable` | 可选的 `cmake` 或 `UV4.exe` 路径/命令 |
| `outputDirectory` | prebuild 输出目录，默认 `pikapython-generated` |
| `buildDirectory` | toolchain 构建目录 |
| `arguments` | 传给 adapter 的附加字符串参数 |

- `cmake` 和 `keil`：
  - 当前已实现配置、集成和编译；
  - 必须提供 `projectFile`、`targetName` 和 `buildDirectory`。
- `make` 和 `iar`：
  - 当前只被 schema 接受；
  - `build` 会返回 `adapter_not_implemented`；
  - 不应据此宣称目标已经可构建。
- CMake `arguments` 支持路径占位符：
  - `{project}`；
  - `{source}`；
  - `{generated}`；
  - `{build}`。
- Keil `arguments`：
  - 原样追加到 UV4 build 命令；
  - adapter 不读取或改写工程的 C 语言标准。

受控 target 命令：

```bash
pikapython-cli target add \
  --id linux \
  --adapter cmake \
  --project-file CMakeLists.txt \
  --target-name linux-console \
  --build-directory .pikapython/build/linux \
  --argument=-DCMAKE_BUILD_TYPE=Release \
  --default
pikapython-cli target update --id linux --tool-executable cmake
pikapython-cli target show --id linux
pikapython-cli target list
pikapython-cli target remove --id linux
```

## build 选择规则

```bash
pikapython-cli build --prebuild-only
pikapython-cli build
pikapython-cli build --target linux
pikapython-cli build --output generated-for-review --prebuild-only
```

- 显式 `--target` 优先。
- 未显式指定时使用 `defaultTarget`。
- 没有默认目标且只有一个 target 时自动选择该 target。
- 多个 target 且没有默认目标时，完整 build 返回 `target_ambiguous`。
- 没有 target 时：
  - prebuild 仍可完成；
  - 完整 build 只返回 `target_config_missing` warning，不启动编译器。
- `--output` 和 `outputDirectory` 必须解析到项目目录内部。

依赖安装、prebuild 和完整 build 的顺序见[快速开始](getting-started.md)。
