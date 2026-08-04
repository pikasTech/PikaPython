# hmac 算法来源

本目录按 RFC 2104 实现 HMAC，并通过 `pikapython-hashlib` 提供的摘要算法合同
选择 MD5、SHA-1 和 SHA-2。实现不复制 PikaPython V1 的 mbedTLS 对象层。

源码适用本仓库许可证。测试使用 CPython `hmac` 结果作为逐字节对照，并覆盖
分片更新、复制独立性和等长完整比较。
