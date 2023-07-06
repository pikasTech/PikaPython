import zlib
try:
    zlib.compress("This should cause a TypeError because it's a str not bytes")
except:
    print("TypeError")

try:
    zlib.decompress("This should also cause a TypeError because it's a str not bytes")
except:
    print("TypeError")

try:
    zlib.decompress(b"This should cause a zlib.error because it's not valid zlib compressed data")
except:
    print("zlib.error")

