# 平台移植与模板

平台工程负责启动、I/O、工具链和链接。内核、包源码、capability 头和 application 入口由 CLI 安装或生成，板级工程不复制第二份内核配置。

## 最小启动链路

```text
reset/startup
  -> C main()
  -> 初始化时钟、内存、外设和输出
  -> pikaPythonInit()
  -> 校验冻结模块和 binding
  -> 加载并执行一次 __main__ 程序镜像
  -> 返回 PikaStatus
  -> 可选 pikaPythonRepl()
```

最小 C 入口：

```c
#include "pikapython_modules.h"

int main(void) {
    PikaStatus status;

    platform_initialize();
    status = pikaPythonInit();
    if (status != PIKA_STATUS_OK) {
        platform_fail(status);
    }

    for (;;) {
        platform_poll();
    }
}
```

- `pikapython_modules.h` 由 prebuild 生成。
- `pikaPythonInit()`：
  - 使用生成的冻结模块和 binding 清单；
  - 要求唯一入口模块 `__main__`；
  - 执行 prebuild 从项目根目录 `main.py` 生成的程序镜像；
  - 失败时返回稳定 `PikaStatus`。
- 内核入口不替平台初始化：
  - 不初始化 HAL、时钟、UART、文件系统或 RTOS；
  - 不自动进入 REPL。

## weak 平台覆盖

内核提供两个 weak 默认实现：

```c
PikaStatus pika_platform_write(const char* data, size_t size);

PikaStatus pika_platform_repl_read(
    uint8_t* data,
    size_t capacity,
    size_t* received);
```

- `pika_platform_write()`：
  - 承载 `print()`、启动错误和 REPL 输出；
  - 默认 weak 实现丢弃输出并返回成功；
  - 产品平台应覆盖它并报告实际 I/O 错误。
- `pika_platform_repl_read()`：
  - 为阻塞式 application REPL 提供输入字节；
  - 默认 weak 实现返回 `PIKA_STATUS_UNSUPPORTED_CAPABILITY`；
  - 启用 REPL 的平台必须覆盖它。
- 覆盖规则：
  - 使用与 `pika_platform.h` 完全一致的签名；
  - 在板级源码中提供强符号；
  - 不修改内核默认实现；
  - 输出与输入都保留原始字节，不私自改写换行或编码。

Linux 模板使用 `stdout` 和 `stdin`，Keil 模板使用 USART1，Beckus QEMU 模板使用 UART2。它们是平台实现示例，不构成所有板卡的固定传输。

## 可选 REPL

- REPL 的 capability 边界：
  - 只在选中 profile 的 capability 闭包包含 `repl.core` 时进入目标产物；
  - 不在 CMake、Keil 或 QEMU 命令行直接定义 `PIKA_REPL_ENABLE`。

启用后的 application 调用方式：

```c
PikaStatus status = pikaPythonInit();

if (status != PIKA_STATUS_OK) {
    platform_fail(status);
}
#if PIKA_REPL_ENABLE
status = pikaPythonRepl();
if (status != PIKA_STATUS_OK) {
    platform_fail(status);
}
#endif
```

- `pikaPythonRepl()`：
  - 复用 `main.py` 建立的持久模块全局和 binding；
  - 不再次执行 `main.py`；
  - 通过 `pika_platform_repl_read()` 读取输入；
  - 通过 `pika_platform_write()` 输出。
- RTOS、TCP 或非阻塞驱动：
  - 不应调用阻塞式 `pikaPythonRepl()`；
  - 应使用传输无关的 REPL session/stream 接口；
  - 可参考 `pika_repl_feed()` 和 `pika_repl_stream_accept()` 的当前声明。

## CMake 接入

CMake target 应读取 `pikapython-generated/source-manifest.json`：

1. 遍历 `sources` 并加入 target。
2. 遍历 `includeDirectories` 并加入 include path。
3. 加入板级 `main.c` 和平台覆盖源码。
4. 由 `pikapython-cli build` 启动 configure 和 compile。

- CLI configure 命令固定包含：
  - `cmake -S <projectFile 所在目录>`；
  - `-B <build-directory>`；
  - target `arguments`。
- CLI compile 命令固定包含：
  - `cmake --build <build-directory>`；
  - `--target <targetName>`。
- capability 边界：
  - CMake 只消费生成头；
  - 不用 option 或 `target_compile_definitions` 重建 capability 选择。

## Keil 接入

Keil target 必须提供：

- `projectFile`：
  - 指向存在的 `.uvprojx`；
  - 使用项目相对路径。
- `targetName`：
  - 必须与工程 XML 的 TargetName 完全一致。
- `buildDirectory`：
  - 保存 `keil-build.log`。

Keil adapter 在 prebuild 后：

- 同步 `PikaPython Kernel`：
  - 来源是 `pikapython-kernel/*.c`。
- 同步 `PikaPython Packages`：
  - 来源是 `pikapython-packages/*/*.c`。
- 同步 `PikaPython Generated`：
  - 来源是生成目录的 `*.c`。
- 追加对应 include path。
- 保留板级启动、HAL、链接脚本和工程 C 标准：
  - adapter 不替换这些平台事实。

默认自动查找 `UV4.exe` 或 `UV4`。也可以通过 target 的 `toolExecutable` 指定命令或文件路径。

## QEMU 模板边界

当前仓库包含两类 QEMU 模板：

- `project-templates/stm32f405-qemu/`：
  - 通用 STM32F405 QEMU 工程。
  - REPL 使用 semihosting 字节通道，不使用模拟 UART。
- `project-templates/stm32f103rb-qemu-beckus/`：
  - Olimex STM32-P103；
  - STM32F103RBT6；
  - Beckus QEMU `2.1.3`；
  - `stm32-p103` machine；
  - UART2 REPL；
  - 128 KiB Flash 和 20 KiB RAM。

Beckus 模板的受控入口：

```bash
pikapython-cli install
pikapython-cli build --prebuild-only
pikapython-cli build
python3 tools/build-provider.py
python3 tools/run-qemu.py
python3 testcases/verify_repl.py
```

- `tools/build-provider.py` 构建固定 provider 镜像。
- `tools/run-qemu.py` 启动交互会话。
- `testcases/verify_repl.py`：
  - 等待 UART2 ready 标志；
  - 注入原始 transcript 字节；
  - 验证编辑、历史和异常恢复。
- STM32F405 semihosting PTY：
  - 使用 `-chardev pty,id=pika_stdio`；
  - 使用 `-semihosting-config enable=on,target=native,chardev=pika_stdio`；
  - 使用 `-serial none`，避免把无关模拟 UART 误判为固件 REPL；
  - QEMU 可能把 PTY 路径写到 stdout 或 stderr；
  - 自动化 harness 必须同时非阻塞读取两路，解析出 PTY 后再打开设备。
- Beckus semihosting：
  - 只用于退出模拟器；
  - REPL 输入和输出固定走 UART2。
- 传输判定：
  - `-serial pty` 只会创建模拟 UART；
  - 固件使用 semihosting 时，该 PTY 没有 REPL 数据；
  - 移植时必须先从板级 `platform.c` 确认字节输入输出实现，再选择 QEMU chardev。
- QEMU 结论：
  - 可以说明功能通过、故障恢复和资源边界；
  - 不能说明 MCU 性能；
  - 不能替代真实板卡外设兼容验证。

- 后续参考：
  - 完整项目流程见[快速开始](getting-started.md)；
  - 生成源码边界见[模块与 binding](modules-and-bindings.md)。
