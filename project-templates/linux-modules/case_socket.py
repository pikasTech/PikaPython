import socket


print("constants:", socket.AF_INET, socket.SOCK_STREAM)
print("dns:", socket.gethostbyname("localhost") == "127.0.0.1")

server = socket.socket()
client = socket.socket()
try:
    server.bind(("127.0.0.1", 0))
    local = server.getsockname()
    print("local:", local[0], local[1] > 0)
    server.listen(1)
    client.connect(local)
    accepted, peer = server.accept()
    try:
        print("peer:", peer[0], peer[1] > 0)
        print("send:", client.send(b"ping"))
        print("recv:", accepted.recv(4))
        accepted.sendall(b"pong")
        print("sendall:", client.recv(4))
        client.setblocking(False)
        try:
            client.recv(1)
            print("would-block: accepted")
        except OSError:
            print("would-block: rejected")
        client.setblocking(True)
        client.settimeout(0.01)
        try:
            client.recv(1)
            print("timeout: accepted")
        except OSError:
            print("timeout: rejected")
        client.settimeout(None)
    finally:
        accepted.close()
    print("eof:", client.recv(1) == b"")
finally:
    client.close()
    client.close()
    server.close()
    server.close()

try:
    client.recv(1)
    print("closed: accepted")
except OSError:
    print("closed: rejected")
