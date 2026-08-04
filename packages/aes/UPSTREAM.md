# AES 算法来源

本目录的 AES 核心按 FIPS 197 重新实现，ECB 与 CBC 链式处理依据
NIST SP 800-38A。PikaPython V1 只用于确认公开 `new`、`MODE_ECB`、
`MODE_CBC`、`encrypt` 和 `decrypt` 接口，没有复用其 mbedTLS binding ABI。

源码适用本仓库许可证。测试覆盖 FIPS 197 的 128、192、256 bit key 已知答案
向量，以及 SP 800-38A 的多块 CBC 向量。模块不提供 padding，也不把 ECB 或
CBC 描述为认证加密协议。
