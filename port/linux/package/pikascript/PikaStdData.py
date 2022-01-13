from PikaObj import *


class List(TinyObj):
    def __init__():
        pass

    # add an arg after the end of list
    def append(arg: any):
        pass

    # get an arg by the index
    def get(i: int) -> any:
        pass

    # set an arg by the index
    def set(i: int, arg: any):
        pass

    # get the length of list
    def len() -> int:
        pass
    # support for loop
    def __iter__() -> any:
        pass

    # support for loop
    def __next__() -> any:
        pass

    # support list[]  = val
    def __set__():
        pass

    # support val = list[] 
    def __get__() -> any:
        pass

class Dict(TinyObj):
    def __init__():
        pass

    # get an arg by the key
    def get(key: str) -> any:
        pass

    # set an arg by the key
    def set(key: str, arg: any):
        pass

    # remove an arg by the key
    def remove(key: str):
        pass

    def __iter__() -> any:
        pass

    def __next__() -> any:
        pass

    # support dict[]  = val
    def __set__():
        pass

    # support val = dict[] 
    def __get__() -> any:
        pass


class String(TinyObj):
    def set(s:str):
        pass

    def get()->str:
        pass

    def __iter__() -> any:
        pass

    def __next__() -> any:
        pass

    # support string[]  = val
    def __set__():
        pass

    # support val = string[] 
    def __get__() -> any:
        pass