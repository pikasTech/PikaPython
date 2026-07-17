# R5 语法、runtime 与 VM 批量修复报告

## 结果

本批在现有语法子集内探测 22 个解析期和运行期场景，修复两个稳定问题：

1. 非可迭代对象进入 `for` 时由断言终止，现改为可捕获 `TypeError`，并验证错误后的 VM 继续执行和零堆残留。
2. stepped slice 被 parser 接受后由不支持该格式的 `SLC` 错误执行，现于 parser 稳定拒绝，同时保留普通二元切片合法邻接。

参数绑定、默认参数、关键字、重复参数、额外参数、方法调用、不可调用对象及错误恢复探测未发现新的可复现崩溃，因此没有扩大修改面。

## 架构与资源

- 未改变 VM、对象模型、字节码或栈格式。
- 未扩展语法；仅把既有 VM 不支持的 stepped slice 前移为 parser 错误。
- default：`text -13 B / data 0 / bss 0`。
- minimal：`text -53 B / data 0 / bss 0`。
- default 核心宽回归 302/302，minimal 核心回归 30/30。

## 后续债务

typed `except` 与 `except ... as alias` 的条件和绑定语义仍需独立梳理；该问题涉及异常分派合同，本批不以局部补丁改变架构，由 [#368](https://github.com/pikasTech/PikaPython/issues/368) 跟踪。隔离全量中因主动移除可选绑定产生的失败已在 R5.4 记录。
