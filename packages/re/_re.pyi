from typing import Any


A: int = 256
ASCII: int = 256
I: int = 2
IGNORECASE: int = 2
M: int = 8
MULTILINE: int = 8
S: int = 16
DOTALL: int = 16


def match(pattern: str, subject: str, flags: int = 0) -> Any: ...
def search(pattern: str, subject: str, flags: int = 0) -> Any: ...
def fullmatch(pattern: str, subject: str, flags: int = 0) -> Any: ...
def escape(pattern: str) -> str: ...
