import socket
from network_fixture import HOST, PORT


connection = socket.socket()
try:
    connection.settimeout(1)
    connection.connect((HOST, PORT))
    print(connection.send(b"ping"))
    connection.sendall(b"")
    print(connection.recv(4))
    print(connection.recv(4))
finally:
    connection.close()
