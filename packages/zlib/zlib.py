import _zlib


def compress(data, level=1):
    return _zlib.compress(data, level)


def decompress(data):
    return _zlib.decompress(data)
