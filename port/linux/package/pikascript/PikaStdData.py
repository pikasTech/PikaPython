from PikaObj import *


class List(TinyObj):
    def __init__(self):
        pass

    # add an arg after the end of list
    def append(self, arg: any):
        pass

    # get an arg by the index
    def get(self, i: int) -> any:
        pass

    # set an arg by the index
    def set(self, i: int, arg: any):
        pass

    # get the length of list
    def len(self) -> int:
        pass
    # support for loop
    def __iter__(self) -> any:
        pass

    # support for loop
    def __next__(self) -> any:
        pass

    # support list[]  = val
    def __set__(self, __key: any, __val: any):
        pass

    # support val = list[] 
    def __get__(self, __key: any) -> any:
        pass

class Dict(TinyObj):
    def __init__(self):
        pass

    # get an arg by the key
    def get(self, key: str) -> any:
        pass

    # set an arg by the key
    def set(self, key: str, arg: any):
        pass

    # remove an arg by the key
    def remove(self, key: str):
        pass

    def __iter__(self) -> any:
        pass

    def __next__(self) -> any:
        pass

    # support dict[]  = val
    def __set__(self, __key: any, __val: any):
        pass

    # support val = dict[] 
    def __get__(self, __key: any) -> any:
        pass


class String(TinyObj):
    def __init__(self, s:str):
        pass

    def set(self, s:str):
        pass

    def get(self)->str:
        pass

    def __iter__(self) -> any:
        pass

    def __next__(self) -> any:
        pass

    # support string[]  = val
    def __set__(self, __key: any, __val: any):
        pass

    # support val = string[] 
    def __get__(self, __key: any) -> any:
        pass

class ByteArray(List):
    # convert a string to ByteArray
    def fromString(self, s:str):
        pass
