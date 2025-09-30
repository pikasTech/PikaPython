from PikaObj import *
import builtins


class Tuple:
    def __init__(self): ...

    def get(self, i: int) -> any:
        """get an arg by the index"""

    def len(self) -> int:
        """get the length of list"""

    def __iter__(self) -> any:
        """support for loop"""

    def __next__(self) -> any:
        """support for loop"""

    def __getitem__(self, __key: any) -> any:
        """support val = list[]"""

    def __del__(self): ...
    def __str__(self) -> str: ...
    def __len__(self) -> int: ...

    def __eq__(self, other: any) -> int:
        """support tuple == tuple""" 

    def __add__(self, others: Tuple) -> Tuple:
        """support tuple + tuple"""
    
    def __mul__(self, n: int) -> Tuple:
        """support tuple * int"""

class List(Tuple):
    def __init__(self): ...

    def append(self, arg: any):
        """add an arg after the end of list"""

    def set(self, i: int, arg: any):
        """set an arg by the index"""

    def reverse(self):
        """reverse the list"""

    def pop(self, *index) -> any:
        """pop the last element"""

    def remove(self, val: any):
        """remove the first element"""

    def insert(self, i: int, arg: any):
        """insert an arg before the index"""

    def __setitem__(self, __key: any, __val: any):
        """support list[]  = val"""

    def __str__(self) -> str: ...

    def __add__(self, others: List) -> List:
        """ support list + list"""

    def __mul__(self, n: int) -> Tuple:
        """ support list * int"""

class Dict:
    def __init__(self):
        """ get an arg by the key """

    def get(self, key: str) -> any: ...

    def set(self, key: str, arg: any):
        """ set an arg by the key """

    def remove(self, key: str):
        """ remove an arg by the key """

    def __iter__(self) -> any: ...
    def __next__(self) -> any: ...

    def __setitem__(self, __key: any, __val: any):
        """ support dict[]  = val """

    def __getitem__(self, __key: any) -> any:
        """ support val = dict[] """

    def __del__(self): ...
    def __str__(self) -> str: ...
    def keys(self) -> dict_keys: ...
    def items(self) -> dict_items: ...
    def __len__(self) -> int: ...

    def __contains__(self, val: any) -> int:
        """ support val in dict """

    def update(self, other: Dict):
        """ update dict """
    
    def __eq__(self, other: any) -> int:
        """ support dict == dict """


class dict_keys:
    def __iter__(self) -> any: ...
    def __next__(self) -> any: ...
    def __str__(self) -> str: ...
    def __len__(self) -> int: ...


class dict_items:
    def __iter__(self) -> any: ...
    def __next__(self) -> any: ...
    def __str__(self) -> str: ...
    def __len__(self) -> int: ...


class String:
    def __init__(self, s: str): ...
    def set(self, s: str): ...
    def get(self) -> str: ...
    def __iter__(self) -> any: ...
    def __next__(self) -> any: ...

    def __setitem__(self, __key: any, __val: any):
        """ support string[]  = val """

    def __getitem__(self, __key: any) -> any:
        """ support val = string[] """

    def __str__(self) -> str:
        """ support str() """

    def __len__(self) -> int: ...
    def encode(self, *encoding) -> bytes: ...
    def startswith(self, prefix: str) -> int: ...
    def endswith(self, suffix: str) -> int: ...
    def isdigit(self) -> int: ...
    def islower(self) -> int: ...
    def isalnum(self) -> int: ...
    def isalpha(self) -> int: ...
    def isspace(self) -> int: ...
    def split(self, *s) -> List: ...
    def replace(self, old: str, new: str) -> str: ...
    def strip(self, *chrs) -> str: ...
    def format(self, *vars) -> str: ...
    def join(self, val: any) -> str: ...
    def find(self, sub: str) -> int: ...


class ByteArray(builtins.bytearray):
    pass


class FILEIO:
    def init(self, path: str, mode: str) -> int: ...
    def read(self, *size) -> any: ...
    def write(self, s: any) -> int: ...
    def close(self): ...
    def seek(self, offset: int, *fromwhere) -> int: ...
    def tell(self) -> int: ...
    def readline(self) -> str: ...
    def readlines(self) -> List: ...
    def writelines(self, lines: List): ...


class Utils:
    def int_to_bytes(self, val: int) -> bytes:
        """ convert a int to bytes """
