import _time


def sleep(s: float):
    return _time.sleep(s)


def sleep_s(s: int):
    return _time.sleep_s(s)


def sleep_ms(ms: int):
    return _time.sleep_ms(ms)


def time() -> float:
    return _time.time()


def time_ns() -> int:
    return _time.time_ns()


def gmtime(unix_time: float):
    return _time.gmtime(unix_time)


def localtime(unix_time: float):
    return _time.localtime(unix_time)


def mktime() -> int:
    return _time.mktime()


def asctime() -> str:
    return _time.asctime()


def ctime(unix_time: float) -> str:
    return _time.ctime(unix_time)
