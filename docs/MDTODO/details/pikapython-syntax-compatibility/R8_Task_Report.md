# R8 任务报告

- 完成已有 GitHub #367 的五类 Python 3 语义语法修复。
- 同时修复嵌套非法子 AST 的悬空指针、断言和双重释放路径。
- 新增 8 个非法样例和合法邻接表驱动测试。
- default 399 项、minimal 180 项隔离全量通过。
- data/bss 不变。
- 临时 text 增量：
  - default 1016 字节；
  - minimal 296 字节。
- 下一轮独立回收 Flash，VM/runtime 性能优先级保持最高。
