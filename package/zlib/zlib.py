import _zlib


def compress(data: bytes, level: int = 1) -> bytes:
    return _zlib.compress(data, level)


def decompress(data: bytes) -> bytes:
    return _zlib.decompress(data)
