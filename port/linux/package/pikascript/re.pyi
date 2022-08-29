from PikaObj import *

I: int
IGNORECASE: int
M: int
MULTILINE: int
DOTALL: int

def __init__(): ...

class Pattern:
    def __init__(self):
        pass
    def __del__(self):
        pass
    def findall(self, subject: str, *flags) -> list:
        pass
    def sub(self, repl: str, subjet: str, *flags) -> str:
        pass
    def match(self, subject: str, *flags) -> Match:
        pass
    def fullmatch(self, subject: str, *flags) -> Match:
        pass
    def search(self, subject: str, *flags) -> Match:
        pass

class Match:
    def __init__(self):
        pass
    def __del__(self):
        pass
    def group(self, n: int) -> str:
        pass
    def groups(self) -> list:
        pass
    def span(self, group_n: int) -> list:
        pass

def findall(pattern: str, subject: str, *flags) -> list: ...
def sub(pattern: str, repl: str, subjet: str, *flags) -> str: ...
def match(pattern: str, subject: str, *flags) -> Match: ...
def fullmatch(pattern: str, subject: str, *flags) -> Match: ...
def search(pattern: str, subject: str, *flags) -> Match: ...
def compile(pattern: str) -> Pattern: ...
