from PikaStdData import String as string
from PikaStdData import ByteArray as bytearray

b = bytearray(b'test')
b_s = b.decode()
print(b_s)
s = string('test')
s_b = s.encode()
print(s_b)
