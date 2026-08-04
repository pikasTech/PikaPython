# PikaPython v2

PikaPython v2 提供面向嵌入式设备的 Python 内核、模块包、项目模板和统一构建 CLI。
公开产品版本位于 `pikasTech/PikaPython` 仓库的 `v2` 分支。

## 本地使用 CLI

以下方式直接运行仓库内的 `pikapython_cli`，不需要通过 pip 安装：

```bash
git clone --branch v2 --single-branch https://github.com/pikasTech/PikaPython.git
cd PikaPython
python3 -m pikapython_cli --help
```

前置条件：

- Python 3.9 或更高版本；
- Python 环境中已有 `PyYAML>=6.0,<7`；
- Git、CMake 和可用的 C 编译器。

## 构建并运行 Linux 模板

从产品仓复制公开模板，然后仍使用仓库内的 CLI 安装依赖和构建：

```bash
mkdir -p ../pika-linux-app
cp -R project-templates/linux-console/. ../pika-linux-app/
cd ../pika-linux-app

PYTHONPATH=../PikaPython python3 -m pikapython_cli install
PYTHONPATH=../PikaPython python3 -m pikapython_cli build
./.pikapython/build/linux/linux-console
python3 tools/verify-linux-output.py
```

模板中的 `packages.sourceUrl` 默认指向公开产品仓，`packages.ref` 固定为 `v2`。
`install` 会从该 release 源解析并安装依赖；不会在网络失败时回退到内部研发仓。

更多配置、平台模板和模块开发说明见 [`docs/reference/index.md`](docs/reference/index.md)。
