import hmac


value = hmac.new(b"key", b"message", "sha256")
print("sha256:", value.hexdigest())
copied = value.copy()
value.update(b"-left")
copied.update(b"-right")
print("copy-left:", value.hexdigest())
print("copy-right:", copied.hexdigest())
print("equal-bytes:", hmac.compare_digest(b"same", b"same"))
print("different-bytes:", hmac.compare_digest(b"same", b"diff"))
print("equal-text:", hmac.compare_digest("same", "same"))
