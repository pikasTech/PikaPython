import socket

# 创建一个 socket 对象
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 使用域名和端口来进行连接
s.connect(("dns.google", 53))

# 关闭连接
s.close()

print("PASS")
