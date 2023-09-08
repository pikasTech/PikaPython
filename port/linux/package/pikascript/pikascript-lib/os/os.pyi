O_RDONLY: int
O_WRONLY: int
O_RDWR: int
O_APPEND: int
O_CREAT: int


def __init__(self):
    pass


def mkdir(self, path: str, *mode):
    pass


def rmdir(self, path: str):
    pass


def chdir(self, path: str):
    pass


def listdir(self, path: str) -> list:
    pass


def getcwd(self) -> str:
    pass


def open(self, filename: str, flags: int) -> FILE:
    pass


def read(self, fd: FILE, len: int) -> str:
    pass


def write(self, fd: FILE, buf: any) -> int:
    pass


def lseek(self, fd: FILE, pos: int, how: int) -> int:
    pass


def close(self, fd: FILE):
    pass


def fstat(self, fd: FILE) -> fileStat:
    pass


def remove(self, filename: str):
    pass


def rename(self, old: str, new: str):
    pass


class fileStat:
    def st_size(self) -> int:
        pass


class path:
    def join(self, *paths) -> str:
        pass

    def split(self, path: str) -> tuple:
        pass

    def splitext(self, path: str) -> tuple:
        pass

    def basename(self, path: str) -> str:
        pass

    def dirname(self, path: str) -> str:
        pass

    def exists(self, path: str) -> bool:
        pass

    def isdir(self, path: str) -> bool:
        pass

    def isfile(self, path: str) -> bool:
        pass

    def isabs(self, path: str) -> bool:
        pass

    def abspath(self, path: str) -> str:
        pass
