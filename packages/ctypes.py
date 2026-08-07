"""Minimal ctypes-compatible scalar declarations for PikaPython."""


class _Scalar:
    def __init__(self, value=0):
        self.value = value

    def __int__(self):
        return int(self.value)

    def __repr__(self):
        return "%s(%r)" % (self.__class__.__name__, self.value)


class c_int(_Scalar):
    pass


class c_uint(_Scalar):
    pass


class c_float(_Scalar):
    pass


class c_double(_Scalar):
    pass


class c_char_p(_Scalar):
    pass


def sizeof(value):
    if isinstance(value, c_char_p):
        return 4
    if isinstance(value, c_double):
        return 8
    if isinstance(value, c_float):
        return 4
    return 4
