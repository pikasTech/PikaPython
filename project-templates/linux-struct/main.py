import struct


packed = struct.pack("<hI", -2, 305419896)
print("pack:", packed)
print("unpack:", struct.unpack("<hI", packed))
print("unpack-from:", struct.unpack_from("<H", b"\x00\x34\x12", 1))
print("calcsize:", struct.calcsize(">bhiq"))
try:
    struct.pack("<b", 128)
    print("range: accepted")
except Exception:
    print("range: rejected")
try:
    struct.unpack("<I", b"\x00")
    print("length: accepted")
except Exception:
    print("length: rejected")
