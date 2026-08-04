from typing import Any


A: int = 33554432
ASCII: int = 33554432
I: int = 1
IGNORECASE: int = 1
M: int = 2
MULTILINE: int = 2
S: int = 4
DOTALL: int = 4


def match(pattern: str, subject: str, flags: int = 0) -> Any: ...
def search(pattern: str, subject: str, flags: int = 0) -> Any: ...
def fullmatch(pattern: str, subject: str, flags: int = 0) -> Any: ...
def escape(pattern: str) -> str: ...
