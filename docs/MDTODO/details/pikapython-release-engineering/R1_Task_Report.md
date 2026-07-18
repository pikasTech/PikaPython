# R1 PikaPython v1.14.0 发布总结

PikaPython v1.14.0 已正式发布。版本元数据、runtime header、CMSIS Pack、包索引、annotated tag 与 GitHub Release 已对齐；pikascript-core、PikaStdLib、threading、requests、mqtt 和 time 已登记对应版本。

按用户裁决，本次跳过 RC，接受异常正确性修复的少量 Flash 增量，开发测试工具、环境、LVGL 和网络问题不阻断源码发布。Issue #371、#372、#373 保持开放并转入发布后处理，不在本报告中宣称已经修复。

发布前完成 Docker Release core 编译、发布工具单测和三项 runtime 定向测试；发布后完成完整历史 fresh clone 的 tag/版本/包索引检查，并按标准 Linux 初始化入口再次编译 core。
