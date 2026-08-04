# hashlib 算法来源

本目录的摘要实现按公开标准重新实现，没有复制 PikaPython V1 的 mbedTLS 适配层，
也不链接宿主 OpenSSL 或目标端完整 mbedTLS。

- MD5 语义和轮函数依据 RFC 1321。
- SHA-1、SHA-224、SHA-256、SHA-384 和 SHA-512 依据 FIPS 180-4，
  并参考 RFC 6234 的输入、填充和长度编码合同。
- Python 用户接口参考 PikaPython V1 与 Python 3 `hashlib` 的共同子集。

源码适用本仓库许可证。正确性由 CPython 对照向量、分片更新、复制独立性和
严格 C99 编译测试共同验证。
