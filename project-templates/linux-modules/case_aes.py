import aes


plain = b"\x6b\xc1\xbe\xe2\x2e\x40\x9f\x96\xe9\x3d\x7e\x11\x73\x93\x17\x2a"

cipher = aes.new(
    b"\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c",
    aes.MODE_ECB,
)
encrypted = cipher.encrypt(plain)
print("aes-128-ecb:", encrypted == b"\x3a\xd7\x7b\xb4\x0d\x7a\x36\x60\xa8\x9e\xca\xf3\x24\x66\xef\x97", cipher.decrypt(encrypted) == plain)

cipher = aes.new(
    b"\x8e\x73\xb0\xf7\xda\x0e\x64\x52\xc8\x10\xf3\x2b\x80\x90\x79\xe5\x62\xf8\xea\xd2\x52\x2c\x6b\x7b",
    aes.MODE_ECB,
)
encrypted = cipher.encrypt(plain)
print("aes-192-ecb:", encrypted == b"\xbd\x33\x4f\x1d\x6e\x45\xf2\x5f\xf7\x12\xa2\x14\x57\x1f\xa5\xcc", cipher.decrypt(encrypted) == plain)

cipher = aes.new(
    b"\x60\x3d\xeb\x10\x15\xca\x71\xbe\x2b\x73\xae\xf0\x85\x7d\x77\x81\x1f\x35\x2c\x07\x3b\x61\x08\xd7\x2d\x98\x10\xa3\x09\x14\xdf\xf4",
    aes.MODE_ECB,
)
encrypted = cipher.encrypt(plain)
print("aes-256-ecb:", encrypted == b"\xf3\xee\xd1\xbd\xb5\xd2\xa0\x3c\x06\x4b\x5a\x7e\x3d\xb1\x81\xf8", cipher.decrypt(encrypted) == plain)

cipher = aes.new(
    b"\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c",
    aes.MODE_CBC,
    b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
)
encrypted = cipher.encrypt(plain)
print("aes-128-cbc:", encrypted == b"\x76\x49\xab\xac\x81\x19\xb2\x46\xce\xe9\x8e\x9b\x12\xe9\x19\x7d", cipher.decrypt(encrypted) == plain)

try:
    aes.new(b"short", aes.MODE_ECB)
    print("invalid-key: accepted")
except ValueError:
    print("invalid-key: rejected")
try:
    aes.new(b"0123456789abcdef", aes.MODE_CBC)
    print("missing-iv: accepted")
except ValueError:
    print("missing-iv: rejected")
try:
    cipher.encrypt(b"x")
    print("invalid-block: accepted")
except ValueError:
    print("invalid-block: rejected")
try:
    aes.new(b"0123456789abcdef", 99)
    print("invalid-mode: accepted")
except ValueError:
    print("invalid-mode: rejected")
