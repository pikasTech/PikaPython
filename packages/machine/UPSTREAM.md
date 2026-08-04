# STM32 machine 上游来源

本目录包含从 PikaPython V1 导入的 STM32 平台和 `pika_hal` 设备层资产。
这些文件只作为平台驱动来源，不继承 V1 的 Python API、对象模型或 binding ABI。

## 来源

- 仓库：`https://github.com/pikasTech/PikaPython.git`
- 快照提交：`bae6cd1f69608aa8e09d41ea6a336f3f6e8b5f96`
- STM32 与 `pika_hal` 最近共同变更提交：
  `e0e6d56e6a49cc6c16d9a61dcb8cdb7d6d950af8`
- 规格：`PJ2026-050112 STM32 machine v0.2`

## 导入范围

- `package/STM32` 顶层的 7 个 C 文件和 2 个 H 文件导入到 `STM32/`，
  保持原文件名。
- `package/STM32/config/*/*.h` 的 132 个系列配置头全部导入到
  `STM32/config/`，保持系列目录、文件名和相对 include 关系。
- `package/PikaStdDevice` 中 5 个 `pika_hal*.c` 和 4 个 `pika_hal*.h`
  设备层文件导入到 `PikaStdDevice/`，保持原文件名。
- 上游根 `LICENSE` 保存为 `LICENSE.V1`。
- 带 `SPDX-License-Identifier: Apache-2.0` 的 RT-Thread 派生文件同时附带
  `LICENSE.Apache-2.0`。

目录使用直接、可逆的来源映射：

```text
package/STM32/<file>
  -> packages/machine/STM32/<file>
package/STM32/config/<series>/<name>.h
  -> packages/machine/STM32/config/<series>/<name>.h
package/PikaStdDevice/<file>
  -> packages/machine/PikaStdDevice/<file>
```

全部 150 个 C/H 在导入时逐文件执行字节比较。本快照没有改写文件内容。

## 明确排除

- `package/STM32/STM32.pyi` 不导入。公开模块由新的 `machine.pyi` 定义，
  并以 MicroPython STM32 port 1.28.0 为行为权威。
- `PikaStdDevice` 的 Python 包装、`PikaObj`、`Arg`、生成文件、VM 和 runtime
  不导入。
- 未经当前平台 shim 适配的快照 C 文件不进入安装导出或 target source manifest。

## 后续修改规则

- 自有 machine facade 和平台 shim 使用职责化的新接口。
- 适配快照文件时保留原版权和 SPDX 头，并在本文件记录语义差异。
- 任何 V1 对象或 binding 类型进入平台接口都视为规格偏离。
- 目标包保留 `STM32/config/<series>/` 和 `PikaStdDevice/` 职责层级；
  installer、source manifest 与 target adapter 必须递归消费并保持相对路径。
