import base64

if base64.b64encode(b"123456").decode() == "MTIzNDU2":
    print("base64 encode test ok")
else:
    print("base64 encode test error")

if base64.b64decode(b"\x4d\x54\x49\x7a\x4e\x44\x55\x32").decode() == "123456":
    print("base64 decode test ok")
else:
    print("base64 decode test error")
