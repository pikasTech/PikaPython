import zlib


payload = b"PikaPython V2 zlib cross-runtime payload"
compressed = zlib.compress(bytearray(payload), 1)
print("compressed:", compressed)
print("roundtrip:", type(compressed) == bytes, zlib.decompress(compressed) == payload)
cpython_stream = b"x\x01\x0b\xc8\xccN\x0c\xa8,\xc9\xc8\xcfS\x083R\xa8\xca\xc9LRH.\xca/.\xd6-*\xcd+\xc9\xccMU(H\xac\xcc\xc9OL\x01\x00.%\x0f\x06"
print("cpython-stream:", zlib.decompress(cpython_stream))
print("bytearray-stream:", zlib.decompress(bytearray(cpython_stream)))
try:
    zlib.decompress(b"not-zlib")
    print("invalid-header: accepted")
except Exception:
    print("invalid-header: rejected")
try:
    zlib.decompress(cpython_stream[:-1])
    print("truncated: accepted")
except Exception:
    print("truncated: rejected")
corrupt = bytearray(cpython_stream)
corrupt[-1] = corrupt[-1] ^ 1
try:
    zlib.decompress(corrupt)
    print("checksum: accepted")
except Exception:
    print("checksum: rejected")
print("recovery:", zlib.decompress(cpython_stream) == payload)
