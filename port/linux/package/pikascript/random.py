import _random


def __init__(self):
    """
    Initialize the random number generator.
    """
    return _random.__init__(self)


def random() -> float:
    """
    Return a random float in the range [0.0, 1.0).
    """
    return _random.random()


def randint(a: int, b: int) -> int:
    """
    Return a random integer in the range [a, b], including both end points.
    """
    return _random.randint(a, b)


def randrange(start: int, stop: int, step: int) -> int:
    """
    Return a randomly-selected element from range(start, stop, step).
    """
    return _random.randrange(start, stop, step)


def seed(a: int) -> None:
    """
    Initialize the random number generator.
    """
    return _random.seed(a)


def uniform(a: float, b: float) -> float:
    """
    Return a random float in the range [a, b).
    """
    return _random.uniform(a, b)
