import _socket

AF_INET = 2
SOCK_STREAM = 1



class socket(_socket.socket):
    family = AF_INET
    type = SOCK_STREAM
    sockfd = 0
    client_sockfd = 0
    client_addr = ''
    protocol = 0

    def __init__(self, *vars):
        if len(vars) > 0:
            self.family = vars[0]
        if len(vars) > 1:
            self.type = vars[1]
        if len(vars) > 2:
            self.proto = vars[2]
        self._init()

    def bind(self, host_port):
        host = host_port[0]
        port = host_port[1]
        return self._bind(host, port)

    def listen(self, num):
        return self._listen(num)

    def accept(self):
        self._accept()
        client = socket()
        client.sockfd = self.client_sockfd
        return (client, self.client_addr)

    def send(self, data):
        return self._send(data)

    def close(self):
        self._close()

    def connect(self, host_port):
        host = host_port[0]
        port = host_port[1]
        if type(host) != str:
            print('Error: host must be a string')
            raise
        if type(port) != int:
            print('Error: port must be an integer')
            raise
        return self._connect(host, port)

    def recv(self, num):
        return self._recv(num)

    def setblocking(self, sta): 
        return self._setblocking(sta)

def gethostname():
    return _socket._gethostname()

def gethostbyname(host):
    return _socket._gethostbyname(host)
