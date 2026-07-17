# R4 异常语义修复总报告

本轮在现有语法子集和架构内完成 #368 的可局部修复部分：

- typed `except` 按内建异常类型及父类关系匹配，多 handler 只执行首个命中分支。
- `except ... as alias` 在 handler 内可见；`raise` 内建异常 constructor/instance 可被捕获。
- 非法 typed except 形式在 parser 阶段稳定拒绝，旧格式无类型 `EXP` 字节码保持兼容。
- 正式矩阵覆盖 23 个 root runtime 场景，另有 3 个临时探针；逐场景验证析构后堆占用归零。
- default 异常定向 7/7、排除既有隔离失败后的核心宽回归 303/303、minimal 核心承诺范围 30/30 通过。
- 相对 R4 前基线，default 最终 `text -294 B`、`data -24 B`、`bss 0`；minimal 最终 `text -1379 B`、`data -24 B`、`bss 0`。

未改变 VM、对象模型和字节码大架构，未扩大 Python 语法范围。handler 内再次抛出和 alias 完整保留子类实例语义仍受现有异常状态模型限制，继续由 #368 跟踪。
