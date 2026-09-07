# Raspberry Pi Pico 2 ARM

该模板面向 Raspberry Pi Pico 2（RP2350A、ARM Secure Cortex-M33），使用
`~/pico-sdk` 中的 Pico SDK 2.3.0 或兼容版本构建。目标固定为
`rp2350-arm-s`；USB CDC 是默认的 PikaPython 输出和 REPL 传输，
`machine.Pin`、`machine.ADC` 和 `machine.PWM` 映射到 Pico SDK 的 GPIO、ADC 和 PWM。

## 构建

复制模板后执行：

```bash
pikapython-cli install
pikapython-cli build --prebuild-only
pikapython-cli build
```

如果 SDK 不在 `~/pico-sdk`，设置 `PICO_SDK_PATH` 或在 CMake 配置时传入
`-DPICO_SDK_PATH=/absolute/path/to/pico-sdk`。构建输出位于
`.pikapython/build/pico2/`，其中 `pico2-firmware.uf2` 可拖放到 Pico 2 的
`RPI-RP2` 启动盘。

首次启动后打开 USB CDC 串口即可看到 `main.py` 输出和 `>>>` 提示符。按 Ctrl-D
结束 REPL；固件随后保持运行。

## 支持边界

- 固件架构：RP2350A ARM Secure Cortex-M33。*RISCV 并未提供支持*
- SDK 后端：GPIO、ADC、PWM、时间、睡眠、临界区和 USB CDC。
- PikaPython：默认使用 `pico2-full` capability，可安装模板列出的全部包。
- 网络：socket、MQTT、HTTP 包可参与构建；Pico 2 本身没有网络控制器，网络操作
  需要额外的 lwIP/外部网络硬件后端。
- 输出：`.elf`、`.bin`、`.hex` 和 `.uf2` 均生成在 `.pikapython/build/pico2/`。
