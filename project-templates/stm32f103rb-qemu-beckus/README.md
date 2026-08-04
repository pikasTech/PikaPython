# STM32F103RB Beckus QEMU 交互式模板

该模板面向 Olimex STM32-P103（STM32F103RB、Cortex-M3），使用固定版本的
Beckus QEMU `2.1.3` 和 `stm32-p103` machine。目标容量严格保持为 128 KiB
Flash 和 20 KiB RAM；QEMU 结果只用于兼容性、故障恢复和资源检查。

## 从模板副本启动

开发当前源码时，先把包源改为本地 Git 工作区，再安装和构建：

```bash
pikapython-cli config set packages.sourceUrl /absolute/path/to/pikapython
pikapython-cli config unset packages.ref
pikapython-cli install
pikapython-cli build --prebuild-only
pikapython-cli build
```

首次使用时构建固定 provider 镜像：

```bash
python3 tools/build-provider.py
```

交互启动和自动验证分别为：

```bash
python3 tools/run-qemu.py
python3 testcases/verify_repl.py
```

自动验证等待 UART2 输出启动标志后才注入原始字节。编辑、历史、异常恢复和
Ctrl-D 直接复用已安装源码中的公共 REPL transcript，不维护板卡专用副本。

启动链路为：

```text
Reset_Handler
  -> C main()
  -> pikaPythonInit()
  -> 执行一次 main.py
  -> pikaPythonRepl()
  -> UART2
```

semihosting 只负责退出模拟器，不承载 REPL 输入或输出。模拟器未实现的外设路径
不能作为 STM32F103 真机兼容结论。
