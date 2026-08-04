import socket


print(socket.AF_INET)
print(socket.SOCK_STREAM)
print(socket.gethostbyname("localhost"))

connection = socket.socket()
connection.setblocking(False)
connection.setblocking(True)
connection.settimeout(0.05)
connection.settimeout(None)
connection.close()
connection.close()

try:
    connection.recv(1)
except OSError:
    print("closed")
