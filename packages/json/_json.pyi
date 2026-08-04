INPUT_BYTE_LIMIT: int = 1024
OUTPUT_BYTE_LIMIT: int = 1024
STRING_BYTE_LIMIT: int = 240
CONTAINER_MEMBER_LIMIT: int = 64
DEPTH_LIMIT: int = 16
TOKEN_END: int = 0
TOKEN_LEFT_BRACKET: int = 1
TOKEN_RIGHT_BRACKET: int = 2
TOKEN_LEFT_BRACE: int = 3
TOKEN_RIGHT_BRACE: int = 4
TOKEN_COMMA: int = 5
TOKEN_COLON: int = 6
TOKEN_STRING: int = 7
TOKEN_INTEGER: int = 8
TOKEN_FLOAT: int = 9
TOKEN_TRUE: int = 10
TOKEN_FALSE: int = 11
TOKEN_NULL: int = 12


def escape(value: str) -> str: ...
def unescape(value: str) -> str: ...
def scan(value: str, position: int) -> tuple: ...
