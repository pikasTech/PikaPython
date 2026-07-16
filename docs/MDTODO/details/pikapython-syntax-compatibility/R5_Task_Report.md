# R5 任务报告

优先处理 #328 后，已把原生 C 方法收到未知 keyword 时的断言终止改为可捕获 `TypeError`。位置、混合、乱序全 keyword、缺参和未知 keyword 均有 Linux 回归；default 隔离全量 396/396，minimal 全量 221/221。资源方面 default text 减少 222 字节，data/bss 不变，minimal 完全不变。位置参数与同名 keyword 重复赋值仍存在 Python 3 精确语义差异，已回写同一 issue，未以 Flash 增长扩大本轮范围。
