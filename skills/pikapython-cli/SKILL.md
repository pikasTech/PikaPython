---
name: pikapython-cli
description: 管理 PikaPython v2 项目依赖、capability、Python 模块预构建和 CMake、Keil、QEMU 等目标工具链。用户要求创建 PikaPython 项目、编辑 pikapython.yaml、安装模块、生成 binding 或 Program Image、构建公开模板、诊断 pikapython-cli 错误时使用；开发 parser、VM、runtime 或内核实现时不使用本 skill。
---

# PikaPython CLI

遵循 Skill(cli-spec)。使用同一 CLI 完成项目配置、依赖安装、主机预构建和目标构建。

## 入口

- 已安装 Python 包时使用 `pikapython-cli`；
- 在产品仓 checkout 内且不通过 pip 安装时使用：

```bash
python3 skills/pikapython-cli/scripts/pikapython-cli.py --help
```

- 从仓库外项目目录使用本地 checkout 时，传入脚本的实际相对或绝对路径；
- 不复制 `pikapython_cli/`，不创建第二个 wrapper 或隐藏 fallback。

## 标准工作流

1. 在项目根创建或读取唯一的 `pikapython.yaml`；
2. 使用 `config` 和 `add|remove` 明确写入 package、capability 和 target；
3. 执行 `install`，并核对 `.pikapython/install-manifest.json` 的 source、ref 和 commit；
4. 执行 `build --prebuild-only` 检查生成阶段；
5. 执行 `build` 调用 YAML 选中的 target adapter；
6. 使用模板自带 verifier 或目标板入口运行产物。

```bash
pikapython-cli init
pikapython-cli config capability \
  --config-file .pikapython/source/config/pikapython-capabilities.yaml \
  --profile runtime-full
pikapython-cli install
pikapython-cli build --prebuild-only
pikapython-cli build
```

## 来源边界

- 普通项目默认使用公开 `https://github.com/pikasTech/PikaPython.git` 的 `v2`；
- 内部开发必须显式设置本地私有 Git 根和待测 ref；
- 使用本地 HEAD 时显式执行 `config unset packages.ref`；
- 网络失败不得静默回退到内部仓或其他 checkout；
- 公开产品问题先在私有 `PikaPython-v2` 修复并验证，再通过 workspace release CLI 发布，禁止直接修改 release 仓。

## 失败处理

- 先读取 CLI 返回的 `code`、`stage`、`message` 和 `hint`；
- `install` 失败时检查 sourceUrl、ref、Git 可达性和 package catalog；
- prebuild 失败时检查 capability、`.pyi`、`main.py` 和已安装 manifest；
- target build 失败时保留已生成产物，再检查 adapter、toolchain 和项目文件；
- 不绕过 CLI 手工运行生成器、拼接产品源码清单或让 CMake 解析 `.pyi`。

## Reference 路由

- 快速开始和公开模板：[`../../docs/reference/getting-started.md`](../../docs/reference/getting-started.md)；
- 项目 YAML 和命令合同：[`../../docs/reference/project-configuration.md`](../../docs/reference/project-configuration.md)；
- 模块、binding 和 Program Image：[`../../docs/reference/modules-and-bindings.md`](../../docs/reference/modules-and-bindings.md)；
- 平台移植和模板：[`../../docs/reference/porting-and-templates.md`](../../docs/reference/porting-and-templates.md)。
