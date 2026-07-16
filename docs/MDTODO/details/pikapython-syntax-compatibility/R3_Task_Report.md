# R3 任务报告

## 结果

完成 #366 的参数过多和非法语法死机修复：

- 固定参数槽位超过 `PIKA_ARG_NUM_MAX` 时返回可捕获的 `OverflowError: arg limit`，不再越界写。
- 合法变长参数调用已验证 25 个位置实参通过。
- 栈元数据改为写前边界检查。
- 空 AST/ASM 错误逐层传播，函数声明开头逗号和连续逗号返回 `PIKA_RES_ERR_SYNTAX_ERROR`。
- 补齐原生/Python 参数边界、合法变长参数、栈边界、非法语法和合法语法边界单元测试。

## 验收

- default：394/394。
- minimal：179/179。
- 四种相关配置：各 74/74。
- 默认 core text 比 R2 少 88 字节，minimal 少 56 字节；data/bss 不变。
- 未扩大语法范围、未改变架构、未增加 RAM/Flash。
- GitHub 证据：[pikasTech/PikaPython#366 comment](https://github.com/pikasTech/PikaPython/issues/366#issuecomment-4991623622)。
