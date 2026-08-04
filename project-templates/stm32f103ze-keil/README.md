# STM32F103ZE Keil 工程模板

这是 PikaPython 的首个 `project-templates` 工程模板，面向 STM32F103ZE 和
Keil MDK。板级启动文件、CMSIS、STM32F1 HAL、CubeMX 配置和 Keil 工程来自
已经完成 Windows 原生 UV4 编译验证的 V1 `simulation-keil` 工程。

模板只保存板级工程、最小应用和声明配置，不保存 PikaPython 内核副本、预构建
生成物、本机工具路径或 Keil 编译产物。

板级时钟、GPIO 和串口初始化完成后，用户启动 PikaPython 只需调用
`pikaPythonInit()`。预编译阶段把根目录 `main.py` 和安装模块生成到
`pikapython-generated/`，该入口随后建立运行时并执行 `main.py`。

启动顺序固定为：

1. Cortex-M 复位入口设置栈、数据段和 BSS，并调用 C `main()`；
2. `main()` 初始化 HAL、72 MHz 系统时钟、GPIO 和 USART1；
3. `pikaPythonInit()` 传入 prebuild 生成的冻结模块和 binding 清单；
4. 内核校验唯一 `__main__`，注册 C 模块，编译模块图并按实际需求分配存储；
5. VM 受保护执行 `main.py`，`print()` 经板级 `pika_platform_write()` 输出到
   USART1；
6. 成功后运行状态保持，`main()` 进入应用主循环；重复初始化不会重复执行脚本；
7. 任一步失败都返回 `PikaStatus` 并输出稳定错误名，模板进入
   `Error_Handler()`，不静默进入 REPL。

内核入口不初始化 HAL、时钟、文件系统、RTOS 或 REPL。后续把冻结源码替换为
预编译程序时，用户 C 代码中的 `pikaPythonInit()` 不需要变化。

需要串口交互时，在初始化成功后显式增加一次调用即可，默认模板不自动进入 REPL：

```c
if (pikaPythonInit() != PIKA_STATUS_OK) {
    Error_Handler();
}
#if PIKA_REPL_ENABLE
if (pikaPythonRepl() != PIKA_STATUS_OK) {
    Error_Handler();
}
#endif
```

模板的 `pika_platform_repl_read()` 默认从 USART1 阻塞读取一个字节。REPL 沿用
`main.py` 已建立的全局变量、对象和 C binding，不会再次执行 `main.py`。RTOS、TCP
或非阻塞串口应用应直接驱动传输无关的 `pika_repl_feed()` 接口，而不调用这个阻塞入口。

## 使用

把本目录复制到新的空目录后执行：

```text
pikapython-cli install
pikapython-cli build --prebuild-only
pikapython-cli build
```

`build` 从 `pikapython.yaml` 选择 `firmware` target，并同步、编译：

```text
firmware/MDK-ARM/pikapython.uvprojx
target: PikaPython
```

Keil 安装位置由 CLI 自动探测；如需指定其他位置，应通过
`pikapython-cli target update` 写入项目配置，不应把本机绝对路径提交到模板。

## 边界

- `firmware/` 只负责板级启动、HAL、链接与 Keil 工程。
- `pikapython-kernel/` 和 `pikapython-generated/` 由 CLI 生成或安装。
- Keil adapter 只管理 PikaPython 的 Kernel、Packages 和 Generated 三个工程组。
- target 的 C 语言标准由 Keil 工程决定，adapter 不读取或改写该设置。
- STM32 HAL 与 CMSIS 文件保留各自源码头中的上游版权和许可证声明。
