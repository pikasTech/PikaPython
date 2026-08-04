import _struct


def pack(format, *values):
    return _struct.pack_values(format, values)


def unpack(format, buffer):
    return _struct.unpack(format, buffer)


def unpack_from(format, buffer, offset=0):
    return _struct.unpack_from(format, buffer, offset)


def calcsize(format):
    return _struct.calcsize(format)
