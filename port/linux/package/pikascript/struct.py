import _struct


def pack(fmt: str, *args) -> bytes:
    return _struct.pack(fmt, *args)


def unpack(fmt: str, data: bytes, offset=0) -> tuple:
    return _struct.unpack(fmt, data, offset)


def unpack_from(fmt: str, data: bytes, offset=0) -> tuple:
    return _struct.unpack(fmt, data, offset)
