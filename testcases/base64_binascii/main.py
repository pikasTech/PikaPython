import base64
import binascii


encoded = base64.b64encode(bytearray(b"Pika\x00"))
print("base64-encode:", type(encoded) == bytes, encoded)
print("base64-decode-bytes:", base64.b64decode(b"UGlrYQ=="))
print("base64-decode-bytearray:", base64.b64decode(bytearray(b"UGlrYQ==")))
print("base64-decode-text:", base64.b64decode("UGlrYQ=="))
print("base64-filter:", base64.b64decode(b"U G\nlrYQ!!=="))
print("base64-padding:", base64.b64decode(b"==UGlrYQ===="))
print("base64-noncanonical:", base64.b64decode(b"AB=="))

try:
    base64.b64decode(b"A===")
    print("base64-invalid: accepted")
except Exception:
    print("base64-invalid: rejected")

try:
    base64.b64decode(b"UGlrYQ")
    print("base64-truncated: accepted")
except Exception:
    print("base64-truncated: rejected")
print("base64-recovery:", base64.b64decode(b"T0s=") == b"OK")

hexed = binascii.hexlify(bytearray(b"Pika\x00"))
print("binascii-encode:", type(hexed) == bytes, hexed)
print("binascii-decode:", binascii.unhexlify("50696B6100"))
try:
    binascii.unhexlify("abc")
    print("binascii-invalid: accepted")
except Exception:
    print("binascii-invalid: rejected")
print("binascii-recovery:", binascii.unhexlify(b"4f4b") == b"OK")
