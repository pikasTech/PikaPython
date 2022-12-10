import _aes

MODE_ECB = _aes.AES.MODE_ECB
MODE_CBC = _aes.AES.MODE_CBC

def new(password:any, mode:int, iv=None) -> _aes.AES:
    aes = _aes.AES()
    aes.new(password, mode, iv)
    return aes
