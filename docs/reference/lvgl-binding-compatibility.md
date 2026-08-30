# LVGL binding 兼容性基线

## 目的

本文冻结 PikaPython V2 分阶段支持 LVGL binding 的来源、兼容合同和阶段验收。详细调查证据见开发仓 issue #32。

## 来源锁定

- LVGL：v9.5.0，提交 85aa60d18b3d5e5588d7b247abf90198f07c8a63。
- MicroPython binding：lvgl/lv_binding_micropython，提交 ac5d06c7d76b0f9e3ee76585bad1ba81c6744a72。
- MicroPython 集成仓：lvgl/lv_micropython，提交 28a722effaa8c2e9d6c7b9fcf45599f681a6671d。
- 生成器：gen/gen_mpy.py；输入先经 GCC C99 预处理，再由 pycparser 解析。
- V2 锁定文件：tools/lvgl-binding/inputs/v9.5.0.json。

输入改变时必须同时更新来源提交、profile、生成器输入和 manifest hash；不能只替换 LVGL 子模块。

## MicroPython 兼容合同

- 导入名固定为 lvgl，常用写法为 import lvgl as lv。
- 顶层函数、常量、枚举、struct 和 class 均通过 lv 暴露。
- obj、btn、label 等 class 的构造器、方法和相关枚举保持同名、同序参数和等价返回。
- color_t 等 struct 支持默认构造、dict 初始化、公开字段读写和嵌套字段访问。
- 枚举保持 lv.ENUM.MEMBER 访问形式和底层值语义。
- opaque C 指针只能通过稳定 Python wrapper 暴露，禁止暴露裸地址。
- callback 支持普通函数、绑定方法和 lambda；user_data 仅由 binding 内部保活，用户不可直接改写。
- parent/child ownership、screen 保活和显式 delete() 语义必须可测试。
- display/input driver 先按运行时注册边界设计；阶段 1 不引入线程锁，LVGL 与 VM 运行在同一线程。
- 非法参数、失效对象、错误 callback 和未启用 feature 必须返回稳定异常或状态，不得静默降级。
- v8/v9 差异必须用版本层或 capability 显式表达，不得用同名 wrapper 隐藏语义变化。

## 阶段 0 验收

- 输入锁定文件可解析，并包含 LVGL、binding、生成器、profile 和输出 schema。
- 生成器输入路径、预处理参数和输出模块名确定；同一输入重复运行得到相同 descriptor/hash。
- API matrix 至少覆盖 module、class、struct、enum、callback、ownership、error 和版本差异字段。
- 文档与 pikapython.yaml/capability 入口保持一致，不直接修改产品仓。

## 阶段 1 验收

- Linux V2 能导入 lvgl 并完成 init、obj、btn、label、screen、color_t 和基础 enum 最小闭环。
- 对象 wrapper、父子关系、字段访问和显式删除有正向与错误路径单测。
- 测试脚本出现 V2 不支持语法时，必须先在 Linux 内核加入最小语法回归单测并实现，再恢复 LVGL 测试；禁止改写用例绕过。
- 阶段 1 不宣称完整 LVGL v9 覆盖，不进入产品 release。

## 生成物身份

后续生成 manifest 至少记录 LVGL commit、binding commit、lv_conf profile/hash、generator hash、API descriptor hash 和 source manifest hash。生成物由 CLI 原子替换，禁止手工编辑。
