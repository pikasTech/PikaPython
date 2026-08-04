import socket
from network_fixture import HOST, PORT


server = socket.socket()
try:
    server.bind((HOST, PORT))
    server.listen(1)
    client, address = server.accept()
    try:
        print(address[0])
        print(address[1] > 0)
        print(client.recv(4))
        client.sendall(b"pong")
    finally:
        client.close()
finally:
    server.close()
