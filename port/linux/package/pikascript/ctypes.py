from PikaObj import *

class cUint(TinyObj):
    def __init__(self, value:int):
        pass

class cWcharP(TinyObj):
    def __init__(self, value:str):
        pass

class Test(TinyObj):
    def add(self, c_uint1:pointer, c_uint2:pointer)->int:
        pass
