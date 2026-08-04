# STM32F405 QEMU 交互式模板

该模板在 QEMU `netduinoplus2`（Cortex-M4）上执行同一套 PikaPython 内核与
prebuild 生成物。它是交互式开发模板，因此在 `main.py` 成功后显式进入 REPL；
普通板级模板仍由应用决定是否调用 REPL。

## 从模板副本启动

```bash
pikapython-cli add pikapython-re==1.0.0
pikapython-cli install
pikapython-cli build
python3 tools/run-qemu.py
```

开发当前源码时，先用 CLI 把 `packages.sourceUrl` 改为本地 Git 路径，避免网络波动。
退出 REPL 使用 Ctrl-D。

自动验证实际固件进程和原始控制字节：

```bash
python3 testcases/verify_repl.py
```

模板自身只依赖 `pikapython-kernel`，因此复制后无需安装任何可选包也能启动。
`pikapython-re` 只是命令示例；其他模块也沿用相同的 `add`、`install`、`build`
路径。`add` 只更新依赖，`install` 投影公开 Python 接口和包内平铺 C/H，
`build` 生成 binding 并从统一 source manifest 编译。

启动链路保持为：

```text
Reset_Handler
  -> C main()
  -> pikaPythonInit()
  -> 执行一次 main.py
  -> pikaPythonRepl()
  -> QEMU semihosting stdin/stdout
```

QEMU 只用于兼容性、故障恢复和目标资源测量，不作为性能基准。

模板为 128 KiB RAM 目标预置了有界模块图：同时最多装载 16 个 Python 模块，单模块
最多分析 16 条导入、64 个函数、4 个类、64 个全局名和 32 个局部名。该默认值可覆盖
多包集成测试；所有上限仍是编译宏，产品可以按目标 RAM 调整，达到上限时内核明确
拒绝，不进行越界访问或无界扩容。
