from PikaObj import *

class c_uint(TinyObj):
    def __init__(self, value:int):
        pass

class c_wchar_p(TinyObj):
    def __init__(self, value:str):
        pass

class Test(TinyObj):
    def add(self, c_uint1:c_uint, c_uint2:c_uint)->int:
        pass
