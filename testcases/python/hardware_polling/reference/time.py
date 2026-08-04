_ticks = 1000


def ticks_ms():
    global _ticks
    value = _ticks
    _ticks += 5
    return value


def ticks_diff(new, old):
    return new - old


def sleep_ms(duration):
    global _ticks
    _ticks += duration
