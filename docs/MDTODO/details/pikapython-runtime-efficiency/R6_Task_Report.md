# R6 runtime/VM 性能优化总结

本轮先用仓库 performance、PikaMemInfo、gprof 和 Google Benchmark 定位热点，再优化普通函数调用中的单段 host-object 查询。最终同配置 200 万次默认参数调用由基线均值 `4.770823s` 降至 `4.160763s`，改善约 `12.8%`；目标函数 self 由 `10.89%` 降至 `2.02%`，分配与缓存次数不变。

对象定向 20/20、runtime/VM 核心 207/207 通过。真实 default Release 的 `text/data/bss` 净变化为 `-56/0/0`，minimal 为 `0/0/0`。未修改 parser 或大架构，未增加常驻 RAM。隔离基线的 parser 误拒绝已登记为 GitHub #369。
