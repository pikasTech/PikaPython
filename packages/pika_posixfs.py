"""POSIX file wrapper using the PikaPython os capability."""
from os import O_CREAT, O_RDONLY, O_RDWR, O_TRUNC, O_WRONLY
from os import SEEK_CUR, SEEK_END, SEEK_SET
from os import close, lseek, open, read, write


def _flags(mode):
    plus = mode.find('+') >= 0
    if mode[0] == 'r':
        flags = O_RDWR if plus else O_RDONLY
    else:
        flags = O_RDWR if plus else O_WRONLY
        flags = flags | O_CREAT
        if mode[0] == 'w':
            flags = flags | O_TRUNC
    return flags


class File:
    def __init__(self, fd):
        self.fd = fd
        self.closed = False

    def read(self, size=-1):
        if size < 0:
            size = 1048576
        return read(self.fd, size)

    def write(self, data):
        return write(self.fd, data)

    def seek(self, offset, whence=SEEK_SET):
        return lseek(self.fd, offset, whence)

    def tell(self):
        return lseek(self.fd, 0, SEEK_CUR)

    def close(self):
        if not self.closed:
            close(self.fd)
            self.closed = True


def fopen(filename, mode='r'):
    return File(open(filename, _flags(mode)))


def fclose(stream):
    stream.close()
    return 0


def fread(size, count, stream):
    return stream.read(size * count)


def fwrite(data, size, count, stream):
    written = stream.write(data[:size * count])
    if size == 0:
        return 0
    return written // size


def fseek(stream, offset, whence=SEEK_SET):
    stream.seek(offset, whence)
    return 0


def ftell(stream):
    return stream.tell()
