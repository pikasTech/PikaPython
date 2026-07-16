# R1 任务报告

- 完成内容：
  - 建立运行时效率 MDTODO、基线、Linux-only 函数调用 benchmark 和多关键字顺序单元测试。
  - 优化 dict 重复查找、函数调用 scratch、参数栈扫描、关键字字典整理和 event-off 空 yield。
  - 保持 VM、对象模型、字节码、解析器架构和公开 API 不变。
- 资源结论：
  - 默认与 minimal 的 `data/bss` 均不增长。
  - 默认 core `text` 减少 44 字节，minimal core `text` 减少 108 字节。
  - 不增加常驻 RAM；每次调用 scratch 请求总字节不变，allocator 调用由 3 次降为 1 次。
- 验证结论：
  - minimal 隔离全量 178/178，默认隔离全量 387/387。
  - default、float、optimize-speed、pool 相关 suites 各 69/69。
  - 新增与已有参数、dict、parser 定向测试全部通过。
- 性能结论：
  - 三轮交替函数调用 benchmark 原始耗时均下降，但共享节点控制项波动较大，不声明固定提速百分比。
  - 算法复杂度和 allocator 调用数的下降是可确定收益。
- 保留项：
  - 在零 RAM、零架构变化约束下，单链表正序遍历无法安全降为 O(n)，本轮不强行实现。
  - 网络、LVGL、hashlib 按用户授权或依赖事实隔离。
