import binascii


encoded = binascii.hexlify(bytearray(b"Pika\x00"))
print("encode:", type(encoded) == bytes, encoded)
print("decode-text:", binascii.unhexlify("50696B6100"))
print("decode-bytes:", binascii.unhexlify(b"50696b6100"))
try:
    binascii.unhexlify("abc")
    print("odd-length: accepted")
except Exception:
    print("odd-length: rejected")
try:
    binascii.unhexlify("zz")
    print("invalid-character: accepted")
except Exception:
    print("invalid-character: rejected")
print("recovery:", binascii.unhexlify(b"4f4b") == b"OK")
