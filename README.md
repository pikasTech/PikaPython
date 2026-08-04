# PikaPython v2

PikaPython v2 提供面向嵌入式设备的 Python 内核、模块包、项目模板和统一构建 CLI。
公开产品版本位于 `pikasTech/PikaPython` 仓库的 `v2` 分支。

## 首选入口：PikaPython CLI Skill

使用 PikaPython v2 创建项目、安装模块、配置 capability、生成 binding、构建模板或
诊断 CLI 时，优先阅读并遵循 [`skills/pikapython-cli/SKILL.md`](skills/pikapython-cli/SKILL.md)。

使用 Codex、Claude Code 等 Code Agent 开发时，可以直接要求 Code Agent 读取并遵循
该 Skill。Code Agent 应按照其中定义的标准工作流完成项目创建、依赖安装、配置、
预构建、目标构建和问题诊断，并以其中的来源边界和失败处理规则为准。

## 第二入口：手动使用 CLI

以下方式直接运行仓库内的 `pikapython_cli`，不需要通过 pip 安装：

```bash
git clone --branch v2 --single-branch https://github.com/pikasTech/PikaPython.git
cd PikaPython
python3 skills/pikapython-cli/scripts/pikapython-cli.py --help
```

前置条件：

- Python 3.9 或更高版本；
- Python 环境中已有 `PyYAML>=6.0,<7`；
- Git、CMake 和可用的 C 编译器。

## 进度、性能与资源

![PikaPython v2 进度与性能指标](assets/pikapython-v2-progress-and-performance.png)

相同 workload、匹配 CPU 的 Linux 对比中，PikaPython v2 相对于 MicroPython
v1.28.0 Unix minimal 的结果包括：

- 模块调用最高 `3.54x`；
- 控制流 `3.53x`；
- 可变序列 `2.43x`；
- 对象字段 `1.57x`。

资源与完整性指标：

- Flash 为 `148.23 kB`，MicroPython 为 `202.27 kB`，减少 `26.7%`；
- 静态 RAM 为 `1.88 kB`，MicroPython 为 `16.07 kB`，减少 `88.3%`；
- 完成 `506` 项语法兼容检查，测试覆盖率为 `99.31%`；
- 提供 `6` 个可用模块和 `31` 个可运行里程碑程序。

性能数字来自匹配 CPU 的 Linux 测量，不是 QEMU 模拟速度。QEMU 只用于功能、
Flash、RAM 和故障恢复验证，不用于性能结论。图表与指标来源：
[`pikasTech/pikapython-v2-preview-stm32f4-qemu`](https://github.com/pikasTech/pikapython-v2-preview-stm32f4-qemu)。

## 构建并运行 Linux 模板

从产品仓复制公开模板，然后仍使用仓库内的 CLI 安装依赖和构建：

```bash
mkdir -p ../pika-linux-app
cp -R project-templates/linux-console/. ../pika-linux-app/
cd ../pika-linux-app

python3 ../PikaPython/skills/pikapython-cli/scripts/pikapython-cli.py install
python3 ../PikaPython/skills/pikapython-cli/scripts/pikapython-cli.py build
python3 tools/verify-linux-output.py
```

自动验证通过后，可以交互运行生成的 Linux 程序；按 `Ctrl-D` 退出 REPL：

```bash
./.pikapython/build/linux/linux-console
```

模板中的 `packages.sourceUrl` 默认指向公开产品仓，`packages.ref` 固定为 `v2`。
`install` 会从该 release 源解析并安装依赖；不会在网络失败时回退到内部研发仓。

更多配置、平台模板和模块开发说明见 [`docs/reference/index.md`](docs/reference/index.md)。
