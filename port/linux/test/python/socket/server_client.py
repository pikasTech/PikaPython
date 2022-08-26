import socket
import random
from PikaStdData import String
from PikaStdData import ByteArray

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = "127.0.0.1"
port = 9999 + random.randint(0, 100)
print("port:", port)
server.bind((host, port))
server.listen(5)

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((host, port))

res = server.accept()
accept = res[0]
addr = res[1]

print("recv from client： %s" % str(addr))

client.send(String("send test from client").encode())
print("server recv：", ByteArray(accept.recv(1024)).decode())

accept.send(String("send test from server").encode())
print("client recv：", ByteArray(client.recv(1024)).decode())

accept.close()
client.close()
server.close()
