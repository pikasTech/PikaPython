import _hashlib


def new(mode: str) -> _hashlib.Hash:
    hash = _hashlib.Hash()
    hash.new(mode)
    return hash


def md5(data = None) -> _hashlib.Hash:
    hash = _hashlib.Hash()
    hash.md5(data)
    return hash


def sha1(data = None) -> _hashlib.Hash:
    hash = _hashlib.Hash()
    hash.sha1(data)
    return hash


def sha224(data = None) -> _hashlib.Hash:
    hash = _hashlib.Hash()
    hash.sha224(data)
    return hash


def sha256(data = None) -> _hashlib.Hash:
    hash = _hashlib.Hash()
    hash.sha256(data)
    return hash


def sha384(data = None) -> _hashlib.Hash:
    hash = _hashlib.Hash()
    hash.sha384(data)
    return hash


def sha512(data = None) -> _hashlib.Hash:
    hash = _hashlib.Hash()
    hash.sha512(data)
    return hash
