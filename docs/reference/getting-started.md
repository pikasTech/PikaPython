# 快速开始

## 前置条件

- Python：
  - 版本至少为 3.9；
  - 使用 `pip` 安装当前仓库提供的 `pikapython-cli`；
  - CLI 运行时依赖 `PyYAML>=6.0,<7`。
  - Debian/Ubuntu 创建虚拟环境前需要安装 `python3-venv`。
- 包源：
  - 本地源必须是 Git working tree；
  - 远程源必须是 Git 可以克隆的 URL；
  - 可复现项目应把 `packages.ref` 固定为可解析的 commit 或 tag。
- 编译工具：
  - Linux 模板需要 CMake 和可用的 C 编译器；
  - Keil 模板需要 Keil MDK 的 `UV4.exe`；
  - STM32F405 QEMU 模板：
    - 需要 ARM GNU 工具链和 newlib 头文件；
    - 需要 `qemu-system-arm`；
  - Beckus QEMU 模板需要 Docker 和 Python 3。

Debian/Ubuntu 上为 STM32F405 模板准备最小依赖：

```bash
apt-get install -y python3-venv cmake gcc-arm-none-eabi qemu-system-arm
apt-get install -y --no-install-recommends libnewlib-arm-none-eabi
```

- `--no-install-recommends`：
  - 避免仅为 C 固件构建安装无关的 ARM C++ newlib 包；
  - 不改变目标工具链和 newlib 头文件的版本匹配要求。
- 其他发行版：
  - 安装 ARM GNU 工具链对应的 newlib 或 sysroot 头文件；
  - 不直接套用 Debian/Ubuntu 包名。

从当前源码仓库安装 CLI：

```bash
python3 -m pip install .
pikapython-cli --help
```

- 仓库内核对 CLI：
  - 可以运行 `pikapython-cli --help`。
- 项目文档：
  - 统一使用安装后的 `pikapython-cli` 命令。

## 从空目录完成预构建

以下流程不要求先有 CMake 或 IDE 工程：

- 创建项目配置；
- 安装内核依赖；
- 选择 capability；
- 把 `main.py` 预构建到 `pikapython-generated/`。

```bash
mkdir pika-app
cd pika-app
pikapython-cli init
pikapython-cli config capability \
  --config-file .pikapython/source/config/pikapython-capabilities.yaml \
  --profile runtime-full
printf 'print("hello")\n' > main.py
pikapython-cli install
pikapython-cli config validate
pikapython-cli build --prebuild-only
```

- `pikapython-cli init` 创建 `pikapython.yaml`：
  - 默认依赖是 `pikapython-kernel==2.0.0`；
  - 默认包源是公开 `PikaPython.git` 的 `v2` 分支；
  - 重复初始化会返回 `config_exists`。
- `pikapython-cli install`：
  - 解析精确版本依赖和传递依赖；
  - 把包文件发布到项目目录；
  - 把包源快照和安装清单保存在 `.pikapython/`。
- `pikapython-cli build --prebuild-only`：
  - 要求已经安装依赖；
  - 要求项目根目录存在 `main.py`；
  - 要求显式配置 capability profile；
  - 生成文件但不启动 target toolchain。

示例中的 `printf` 只用于说明最小 `main.py`。实际项目应正常编辑该文件。

## 使用 Linux 模板

从一个空目录复制模板内容，普通用户直接使用模板保存的公开 `v2` 包源：

```bash
mkdir linux-app
cd linux-app
cp -R /absolute/path/to/pikapython/project-templates/linux-console/. .
pikapython-cli install
pikapython-cli build --prebuild-only
pikapython-cli build
python3 tools/verify-linux-output.py
```

- 开发当前内核 checkout 时显式覆盖包源：

  ```bash
  pikapython-cli config set packages.sourceUrl /absolute/path/to/pikapython-v2
  pikapython-cli config unset packages.ref
  ```

  - 本地源使用当前已提交的 `HEAD`；
  - 未提交修改不会进入 install 快照；
  - CLI 不会从公开源失败后自动回退到本地研发仓。
- `build` 默认选择 `linux` target：
  - CMake 读取 `pikapython-generated/source-manifest.json`；
  - 构建目标名为 `linux-console`。

## 使用离线源码快照

- 本地目录约束：
  - `packages.sourceUrl` 必须指向 Git working tree 根目录；
  - 普通压缩包解压后没有 `.git`，不能直接作为包源。

- 优先方案：
  - 使用正式 Git checkout；
  - 把 `packages.sourceUrl` 指向 checkout 根目录；
  - 运行 `pikapython-cli config unset packages.ref` 使用本地 `HEAD`。
- 只有源码归档时：
  - 在归档根目录初始化本地 Git；
  - 提交需要进入包源快照的文件；
  - 再把该目录配置为 `packages.sourceUrl`。
- `install` 只归档已提交的 commit：
  - 未提交文件不会进入 `.pikapython/source`；
  - 这保证离线快照和普通 Git 包源使用同一套可复现语义。

## 使用嵌入式模板

Keil 模板：

```bash
mkdir stm32-keil-app
cd stm32-keil-app
cp -R /absolute/path/to/pikapython/project-templates/stm32f103ze-keil/. .
pikapython-cli config set packages.sourceUrl /absolute/path/to/pikapython
pikapython-cli config unset packages.ref
pikapython-cli install
pikapython-cli build --prebuild-only
pikapython-cli build
```

Beckus QEMU 模板：

```bash
mkdir stm32-qemu-app
cd stm32-qemu-app
cp -R /absolute/path/to/pikapython/project-templates/stm32f103rb-qemu-beckus/. .
pikapython-cli config set packages.sourceUrl /absolute/path/to/pikapython
pikapython-cli config unset packages.ref
pikapython-cli install
pikapython-cli build --prebuild-only
pikapython-cli build
python3 tools/build-provider.py
python3 tools/run-qemu.py
python3 testcases/verify_repl.py
```

- Keil adapter 在预构建后同步三个受管工程组：
  - `PikaPython Kernel`；
  - `PikaPython Packages`；
  - `PikaPython Generated`。
- QEMU 的运行与验证结果只用于：
  - 固件启动；
  - UART2 交互；
  - REPL 故障恢复；
  - Flash、heap 和 stack 的资源检查。
- QEMU 结果不得用于目标性能结论。

## 目标工具链失败

- `build` 顺序：
  - 先完成并发布 prebuild 生成物；
  - 再启动 target toolchain。
- target configure 或 compile 失败时：
  - CLI 返回稳定的 `code`、`stage` 和 `hint`；
  - 已成功发布的 prebuild 生成物继续保留；
  - 修复工具链后直接重新运行 `pikapython-cli build`。
- ARM GNU 编译出现 `stdlib.h: No such file or directory` 时：
  - Debian/Ubuntu 安装 `libnewlib-arm-none-eabi`；
  - 其他系统安装当前工具链匹配的 newlib 或 sysroot 头文件。

## 常用检查

```bash
pikapython-cli list
pikapython-cli config show
pikapython-cli config validate
pikapython-cli target list
pikapython-cli target show --id linux
pikapython-cli --json config show
```

- 后续参考：
  - 项目配置见[项目配置](project-configuration.md)；
  - 模块预构建见[模块与 binding](modules-and-bindings.md)。
