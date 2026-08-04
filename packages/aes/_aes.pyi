from typing import Any


MODE_ECB: int = 0
MODE_CBC: int = 1


class AES:
    MODE_ECB: int = 0
    MODE_CBC: int = 1

    def __init__(self, key: bytes, mode: int, iv: Any) -> None: ...
    def encrypt(self, data: bytes) -> bytes: ...
    def decrypt(self, data: bytes) -> bytes: ...
