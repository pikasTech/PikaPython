import _random


def seed(a):
    return _random.seed(a)


def random():
    return _random.random()


def randint(a, b):
    return _random.randint(a, b)


def randrange(start, stop=None, step=1):
    if stop is None:
        stop = start
        start = 0
    return _random.randrange(start, stop, step)


def uniform(a, b):
    return _random.uniform(a, b)
