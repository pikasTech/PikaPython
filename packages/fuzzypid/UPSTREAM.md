# fuzzypid 上游来源

- 上游项目：
  - 仓库：`https://github.com/FlameAlpha/fuzzy-pid`。
  - 固定提交：`38fb510656067f151701fc7cb8612cf2e807fec5`。
  - 许可证：GNU General Public License v3.0，原文保存在
    `upstream/LICENSE`。
- V1 迁移来源：
  - 仓库：`https://github.com/pikasTech/PikaPython`。
  - 初始导入提交：`ea37db682c9f89dad1bfffeb4aa22dd0a62bae27`。
  - 后续校验提交：`bbe04c7d93d7dd002b6708bb1b8b068f9b4b5755`。
- 快照布局：
  - `upstream/fuzzyPID.c`、`upstream/fuzzyPID.h` 与
    `upstream/LICENSE` 是未修改的字节级上游快照。
  - 快照只用于许可证和来源审计，不进入 package catalog 导出。
  - Python facade、V2 binding 与可构建适配层均位于快照目录外，不复用
    V1 内部 ABI。
- 适配层：
  - 实际构建使用 `algorithm/fuzzyPID.c` 和
    `algorithm/fuzzyPID.h`。
  - 上游类型、宏与全局符号全部增加 `PIKA_FUZZYPID` 或
    `pika_fuzzypid` 前缀。
  - V2 binding 使用可配置静态对象池并深复制控制器输入，初始化失败和池耗尽
    返回明确状态，计算热路径不执行动态分配。
  - 保留上游控制计算与输出量化；修正输出不足三个通道时误清零
    `delta_ki` 而非 `delta_kd` 的分支错误。
  - 梯形和三角隶属函数在退化肩点返回确定的 `1.0`，避免最大范围端点产生
    `0/0` 后静默丢失规则。
  - `direct=None` 明确定义为全部正向控制，避免继承 V1 的空指针行为。
