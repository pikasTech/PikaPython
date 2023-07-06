import aes


raw = b"0102030405060708"

print("== aes 128 ecb test begin ==")
a = aes.new(b"0102030405060708", aes.MODE_ECB)
b = a.encrypt(raw)
c = a.decrypt(b)
print("encrypt:", b)
print("decrypt:", c)
del a
del b
del c
print("== aes 128 ecb test end ==")

print("== aes 192 ecb test begin ==")
a = aes.new(b"010203040506070811121314", aes.MODE_ECB)
b = a.encrypt(raw)
c = a.decrypt(b)
print("encrypt:", b)
print("decrypt:", c)
del a
del b
del c
print("== aes 192 ecb test end ==")


print("== aes 256 ecb test begin ==")
a = aes.new(b"01020304050607081112131415161718", aes.MODE_ECB)
b = a.encrypt(raw)
c = a.decrypt(b)
print("encrypt:", b)
print("decrypt:", c)
del a
del b
del c
print("== aes 256 ecb test end ==")


raw = b"0102030405060708111213141516171821222324252627283132333435363738"


print("== aes 128 cbc test begin ==")
a = aes.new(b"0102030405060708", aes.MODE_CBC, iv=b"0102030405060708")
b = a.encrypt(raw)
c = a.decrypt(b)
print("encrypt:", b)
print("decrypt:", c)
del a
del b
del c
print("== aes 128 cbc test end ==")

print("== aes 192 cbc test begin ==")
a = aes.new(b"010203040506070811121314", aes.MODE_CBC, iv=b"0102030405060708")
b = a.encrypt(raw)
c = a.decrypt(b)
print("encrypt:", b)
print("decrypt:", c)
del a
del b
del c
print("== aes 192 cbc test end ==")


print("== aes 256 cbc test begin ==")
a = aes.new(b"01020304050607081112131415161718",
            aes.MODE_CBC, iv=b"0102030405060708")
b = a.encrypt(raw)
c = a.decrypt(b)
print("encrypt:", b)
print("decrypt:", c)
del a
del b
del c
print("== aes 256 cbc test end ==")
