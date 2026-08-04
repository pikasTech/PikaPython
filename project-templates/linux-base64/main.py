import base64


encoded = base64.b64encode(bytearray(b"Pika\x00"))
print("encode:", type(encoded) == bytes, encoded)
print("decode-bytes:", base64.b64decode(b"UGlrYQ=="))
print("decode-bytearray:", base64.b64decode(bytearray(b"UGlrYQ==")))
print("decode-text:", base64.b64decode("UGlrYQ=="))
print("filter:", base64.b64decode(b"U G\nlrYQ!!=="))
print("padding:", base64.b64decode(b"==UGlrYQ===="))
print("noncanonical:", base64.b64decode(b"AB=="))
try:
    base64.b64decode(b"A===")
    print("invalid-padding: accepted")
except Exception:
    print("invalid-padding: rejected")
try:
    base64.b64decode(b"UGlrYQ")
    print("truncated: accepted")
except Exception:
    print("truncated: rejected")
print("recovery:", base64.b64decode(b"T0s=") == b"OK")
