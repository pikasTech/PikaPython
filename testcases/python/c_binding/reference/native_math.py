ANSWER = 42


def add(left: int, right: int = 1) -> int:
    return left + right


def invert(value: bool) -> bool:
    return not value


def scale(value: float) -> float:
    return value * 2.0


def echo(text: str) -> str:
    return text


def copy(data: bytes) -> bytes:
    return data


def passthrough(value):
    return value


def nothing() -> None:
    return None


class Counter:
    STEP = 5

    def __init__(self, start: int = 10) -> None:
        self.value = start

    def increment(self, amount: int = 1) -> int:
        self.value = self.value + amount
        return self.value
