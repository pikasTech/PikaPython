# SPEC: PJ2026-050114 runtime ecosystem packages v0.1; CPython-style os subset.
from _os import O_CREAT, O_RDONLY, O_RDWR, O_TRUNC, O_WRONLY, SEEK_CUR, SEEK_END, SEEK_SET
from _os import chdir, close, getcwd, listdir, lseek, mkdir, open, read, remove, rename, rmdir, stat_kind, write


class _Path:
    def join(self, left, right):
        if len(left) == 0:
            return right
        if len(right) == 0:
            return left
        if right.startswith("/"):
            return right
        if left.endswith("/"):
            return left + right
        return left + "/" + right

    def basename(self, value):
        last = -1
        index = 0
        while index < len(value):
            if value[index] == "/":
                last = index
            index += 1
        return value[last + 1:]

    def dirname(self, value):
        last = -1
        index = 0
        while index < len(value):
            if value[index] == "/":
                last = index
            index += 1
        if last < 0:
            return ""
        if last == 0:
            return "/"
        return value[:last]

    def exists(self, value):
        return stat_kind(value) != 0

    def isfile(self, value):
        return stat_kind(value) == 1

    def isdir(self, value):
        return stat_kind(value) == 2


path = _Path()
