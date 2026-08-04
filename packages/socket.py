# SPEC: PJ2026-050109 network-ready v0.1; public socket wrapper.
import _socket


AF_INET = _socket.AF_INET
SOCK_STREAM = _socket.SOCK_STREAM


def gethostbyname(host):
    return _socket.gethostbyname(host)


class socket:
    def __init__(self, family=2, type=1, proto=0, native=None):
        if native is None:
            native = _socket.Socket(family, type, proto)
        self._native = native

    def bind(self, address):
        return self._native.bind(address[0], address[1])

    def listen(self, backlog=1):
        return self._native.listen(backlog)

    def accept(self):
        accepted = self._native.accept()
        address = (accepted.peer_host(), accepted.peer_port())
        return (socket(native=accepted), address)

    def connect(self, address):
        return self._native.connect(address[0], address[1])

    def send(self, data):
        return self._native.send(data)

    def sendall(self, data):
        return self._native.sendall(data)

    def recv(self, size):
        return self._native.recv(size)

    def setblocking(self, flag):
        return self._native.setblocking(flag)

    def settimeout(self, seconds):
        return self._native.settimeout(seconds)

    def close(self):
        return self._native.close()
