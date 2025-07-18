# CLAUDE.md

本文件为 Claude Code (claude.ai/code) 在此代码仓库中工作时提供指导。

## 项目概述

PikaPython 是一个超轻量级的 Python 解释器，仅需 4KB RAM 即可运行，无任何依赖。它专为嵌入式系统和 MCU 开发而设计。该项目提供了完整的 Python 运行时环境，支持各种硬件平台和外设。

## 常用开发命令

### 构建系统
项目使用 CMake 作为构建系统，并提供便捷的自定义 shell 脚本：

```bash
# 构建项目
bash make.sh

# 快速测试
bash fast_test.sh

# 运行 Python 解释器(注意:互运行，claude 在测试的时候不要使用)
bash run.sh [script.py]
```
# 运行性能基准测试
bash ci_benchmark.sh
```

### 开发工作流

### 核心组件

**PikaVM** (`src/PikaVM.c/h`): 执行 Python 字节码的虚拟机。处理指令调度、栈管理和运行时执行。

**PikaCompiler** (`src/PikaCompiler.c/h`): 将 Python 源代码编译为字节码。包括词法分析、语法分析和代码生成。

**PikaObj** (`src/PikaObj.c/h`): 实现 Python 对象模型的核心对象系统。处理对象创建、方法调度和内存管理。

**数据结构** (`src/data*.c/h`): 基础数据结构，包括：
- `dataArgs`: 参数处理和参数传递
- `dataStack`: VM 执行的栈操作
- `dataMemory`: 内存管理和分配
- `dataString`: 字符串操作和工具

### 包系统

**包** (`package/`): 为各种功能提供 Python 绑定的模块化扩展：
- `PikaStdLib`: 标准库实现
- `PikaStdDevice`: GPIO、UART 等硬件抽象层
- `PikaCV`: 计算机视觉操作
- `PikaMath`: 数学运算
- 硬件特定包（STM32、ESP32 等）

**包管理器** (`tools/pikaPackageManager/`): 管理包依赖和安装。

### 平台支持

**BSP** (`bsp/`): 各种微控制器和开发板的板级支持包。

**Port** (`port/`): 平台特定的实现：
- `linux/`: Linux 开发和测试环境
- `rt-thread/`: RT-Thread RTOS 支持
- `vsf/`: 通用软件框架支持

### 构建配置

**配置系统** (`port/linux/config/`): 多种配置文件：
- `pika_config_default.h`: 标准配置
- `pika_config_optimize_speed.h`: 性能优化配置
- `pika_config_pool.h`: 内存池分配配置
- `pika_config_syntax_level_minimal.h`: 最小语法支持配置

## 关键开发模式

### C 模块绑定
PikaPython 使用 `.pyi` 接口定义系统自动生成 C 绑定：
1. 在 `.pyi` 文件中定义 Python API
2. 按照命名约定实现 C 函数
3. 使用 `rust-msc-latest-linux` 编译器生成绑定

### 内存管理
- 使用可配置分配器的自定义内存管理
- 支持标准 malloc/free 和内存池分配
- 使用标记-清除算法进行垃圾回收

### 硬件抽象
- `pika_hal` 提供统一的硬件接口
- 在相应的 BSP/port 目录中实现平台特定功能
- 设备驱动程序遵循 GPIO、UART、SPI 等标准化模式

## 测试策略

项目使用 Google Test 进行全面覆盖的单元测试：
- `port/linux/test/` 中的单元测试
- `port/linux/test/python/` 中的 Python 集成测试
- 性能验证的基准测试
- 通过 `gtest.sh` 进行多配置测试

## 开发说明

- 项目严格遵循 C99 标准
- 代码覆盖率跟踪集成到构建系统中
- 栈使用分析有助于优化嵌入式目标的内存使用
- 构建系统支持开发（带调试）和发布配置