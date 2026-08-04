# PikaPython 二次开发指南

本目录面向使用当前内核、包管理和工程模板的二次开发者。V1 官网文档只作为信息架构参考；命令、配置字段、生成物和接口以当前仓库为准。

## 阅读顺序

- [快速开始](getting-started.md)：
  - 安装 CLI；
  - 从空目录完成初始化、依赖安装和预构建；
  - 使用 Linux、Keil 与 QEMU 模板完成构建。
- [项目配置](project-configuration.md)：
  - 解释 `pikapython.yaml` 的依赖、包源、capability 和 target；
  - 说明 adapter、默认目标和生成目录的选择规则。
- [模块与 binding](modules-and-bindings.md)：
  - 说明纯 Python 模块、C 模块和 `.pyi` 声明；
  - 说明包目录、安装布局和预构建生成物。
- [平台移植与模板](porting-and-templates.md)：
  - 说明平台 weak 覆盖；
  - 说明从 C `main()` 到 `main.py`、可选 REPL 以及 Keil/QEMU 接入边界。

## 稳定边界

- 项目配置的唯一入口是项目根目录的 `pikapython.yaml`。
- capability 数据链路：
  - 输入是 `pikapython.yaml`；
  - prebuild 入口是 `pikapython-cli build`；
  - target 只消费 `pikapython-generated/capability-config.h`。
- CMake、Keil、QEMU runner 和板级工程只消费 CLI 生成物：
  - 不直接传入或维护 capability 的 `*_ENABLE` 宏；
  - 不维护第二份 profile 或 capability 闭包。
- QEMU 用于功能、交互恢复和资源检查：
  - 不把模拟时间、吞吐或宿主资源解释为目标性能。
- 当前 CLI 的默认输出是人类可读文本：
  - 机器消费时在顶层命令后使用 `--json`。
