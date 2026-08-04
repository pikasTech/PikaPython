import _aes


MODE_ECB = _aes.MODE_ECB
MODE_CBC = _aes.MODE_CBC


def new(key, mode, iv=None):
    return _aes.AES(key, mode, iv)
