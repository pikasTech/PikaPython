# R9 任务报告

R9 在保持 R8 语义、崩溃修复和 VM/runtime 热路径不变的前提下，完成一轮 parser 局部资源回收。default text 回收 128 B，minimal 无变化，所有静态 data/bss 均无增长；default 399/399、minimal 180/180 通过。

本轮没有完全回收 R8 的正确性成本：相对 R7 仍有 default +888 B、minimal +296 B。剩余增量主要来自错误逐层传播、重复形参/keyword 检测和顶层所有权修复，继续压缩应优先寻找共享错误出口或更紧凑的 parser 临时集合表示，禁止改回悬空指针、双重释放或错误接受，也不得影响 VM/runtime 性能。
