from PikaObj import *

A: int
ASCII: int
I: int
IGNORECASE: int
M: int
MULTILINE: int
S: int
DOTALL: int
# here, not as in python, there is no 'UNICODE' flags,
# cause this version only support UTF-8 characters


def __init__(): ...


class Pattern:
    def __init__(self):
        pass

    def __del__(self):
        pass

    def findall(self, subject: str, *flags) -> list:
        pass

    def sub(self, repl: str, subjet: str, *count__flags) -> str:
        pass

    def subn(self, repl: str, subjet: str, *count__flags) -> list:
        pass

    def match(self, subject: str, *flags) -> Match:
        pass

    def fullmatch(self, subject: str, *flags) -> Match:
        pass

    def search(self, subject: str, *flags) -> Match:
        pass

    def split(self, subject: str, *maxsplit__flags) -> list:
        pass


class Match:
    def __init__(self):
        pass

    def __del__(self):
        pass

    def group(self, *n) -> str:
        pass

    def groups(self) -> list:
        pass
    # ! may returns wrong offset when subject contains widechar, like Chinese
    # this function returns exactly memory offset between the begin of string and the target substring
    def span(self, *group_n) -> list:
        pass


def findall(pattern: str, subject: str, *flags) -> list: ...
# def sub(pattern, repl, string, count=0, flags=0)
def sub(pattern: str, repl: str, subjet: str, *count__flags) -> str: ...
def match(pattern: str, subject: str, *flags) -> Match: ...
def fullmatch(pattern: str, subject: str, *flags) -> Match: ...
def search(pattern: str, subject: str, *flags) -> Match: ...
def compile(pattern: str, *flags) -> Pattern: ...

def escape(pattern: str) -> str: ...
# def subn(pattern, repl, string, count=0, flags=0)
def subn(pattern: str, repl: str, subjet: str, *count__flags) -> list: ...
# def finditer(pattern: str, subject: str, *flags):
def split(pattern: str, subject: str, *maxsplit__flags) -> list: ...
