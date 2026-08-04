import _re


A = _re.A
ASCII = _re.ASCII
I = _re.I
IGNORECASE = _re.IGNORECASE
M = _re.M
MULTILINE = _re.MULTILINE
S = _re.S
DOTALL = _re.DOTALL


class Match:
    def __init__(self, result, subject):
        self._start = result[0]
        self._end = result[1]
        self._group = subject[result[0]:result[1]]

    def group(self, index=0):
        if index == 0:
            return self._group
        raise ValueError("only group 0 is supported")

    def span(self, index=0):
        if index == 0:
            return (self._start, self._end)
        raise ValueError("only group 0 is supported")

def match(pattern, subject, flags=0):
    result = _re.match(pattern, subject, flags)
    if result is None:
        return None
    return Match(result, subject)


def search(pattern, subject, flags=0):
    result = _re.search(pattern, subject, flags)
    if result is None:
        return None
    return Match(result, subject)


def fullmatch(pattern, subject, flags=0):
    result = _re.fullmatch(pattern, subject, flags)
    if result is None:
        return None
    return Match(result, subject)


def escape(pattern):
    return _re.escape(pattern)
