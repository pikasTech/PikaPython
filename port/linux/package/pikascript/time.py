import _time


def sleep(s: float):
    for i in range(int(s)):
        _time.sleep_s(1)
    _time.sleep_ms(int((s - int(s)) * 1000))


def sleep_s(s: int):
    return _time.sleep_s(s)


def sleep_ms(ms: int):
    return _time.sleep_ms(ms)


def time() -> float:
    return _time.time()


def time_ns() -> int:
    return _time.time_ns()


def gmtime(unix_time: float = None):
    if unix_time is None:
        return _time.gmtime(time())
    return _time.gmtime(unix_time)


def localtime(unix_time: float = None):
    if unix_time is None:
        return _time.localtime(time())
    return _time.localtime(unix_time)


def mktime(tm: tuple) -> int:
    return _time.mktime(tm)


def ctime(unix_time: float = None) -> str:
    if unix_time is None:
        return _time.ctime(time())
    return _time.ctime(unix_time)


def asctime() -> str:
    return _time.asctime()


def tick_ms() -> int:
    _time.platformGetTick()
    return _time.tick
