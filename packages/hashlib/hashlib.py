import _hashlib


def new(name, data=b""):
    return _hashlib.Hash(name, data)


def md5(data=b""):
    return new("md5", data)


def sha1(data=b""):
    return new("sha1", data)


def sha224(data=b""):
    return new("sha224", data)


def sha256(data=b""):
    return new("sha256", data)


def sha384(data=b""):
    return new("sha384", data)


def sha512(data=b""):
    return new("sha512", data)
