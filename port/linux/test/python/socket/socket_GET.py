import socket

# 创建一个socket对象
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 获取服务器的IP地址
server_ip = socket.gethostbyname('pikapython.com')
server_port = 80

# 连接到服务器
s.connect((server_ip, server_port))

# 创建HTTP GET请求
request = 'GET / HTTP/1.1\r\nHost: pikapython.com\r\n\r\n'
s.send(request.encode())

# 接收服务器的响应
response = ''
while True:
    recv = s.recv(1024)
    if not recv:
        break
    response += recv.decode()

s.close()

assert 'HTTP/1.1 200 OK' in response
print('PASS')
