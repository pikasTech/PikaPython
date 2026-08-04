from struct import pack, unpack

little = pack("<H", 0x1234)
print(little[0])
print(little[1])

signed = pack(">i", -2)
print(signed[0])
print(signed[3])

decoded = unpack(">I", b"\x01\x02\x03\x04")
print(decoded[0])

negative = unpack("<h", b"\xfe\xff")
print(negative[0])

try:
    pack("<H", -1)
except Exception:
    print(7)

try:
    unpack(">I", b"\x00")
except Exception:
    print(8)
