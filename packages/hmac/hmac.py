import _hmac


def new(key, msg=b"", digestmod="md5"):
    return _hmac.HMAC(key, msg, digestmod)


def compare_digest(left, right):
    return _hmac.compare_digest(left, right)
