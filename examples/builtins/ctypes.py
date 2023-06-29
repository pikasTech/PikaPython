import ctypes
read_data = ctypes.c_buffer(b'', 16)
print("----read size----")
datalen = len(read_data.raw)
print(datalen)
print(read_data.raw)
print("----read data----")
for i in range(0,datalen):
    print(read_data.raw[i])
