import hashlib


print("md5:", hashlib.md5(b"abc").hexdigest())
print("sha1:", hashlib.sha1(b"abc").hexdigest())
print("sha256:", hashlib.sha256(b"abc").hexdigest())
value = hashlib.sha256(b"prefix")
copied = value.copy()
value.update(b"-left")
copied.update(b"-right")
print("copy-left:", value.hexdigest())
print("copy-right:", copied.hexdigest())
mutable = bytearray(b"abc")
print("bytearray:", hashlib.sha256(mutable).hexdigest())
try:
    hashlib.new("missing", b"abc")
    print("algorithm: accepted")
except Exception:
    print("algorithm: rejected")
