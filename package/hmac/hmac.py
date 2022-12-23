import _hmac

def new(key:any, msg=None, digestmod="md5") -> _hmac.HMAC:
    hmac = _hmac.HMAC()
    hmac.new(key, msg, digestmod)
    return hmac

