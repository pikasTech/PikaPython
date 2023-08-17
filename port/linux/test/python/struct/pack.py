import struct

# Test strings and fixed-length strings
assert struct.pack('5s', b'Hello') == b'Hello'
assert struct.pack('10s', b'World') == b'World\x00\x00\x00\x00\x00'

assert struct.pack(
    'hhl', 1, 2, 3) == b'\x01\x00\x02\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00'

# Test signed short, unsigned short, and signed long
assert struct.pack(
    'hHl', -1, 255, 1234567890) == b'\xff\xff\xff\x00\x00\x00\x00\x00\xd2\x02\x96I\x00\x00\x00\x00'

# Test single precision float and double precision float
assert struct.pack(
    'fd', 3.14159, 123.45678901234567) == b'\xd0\x0fI@\x00\x00\x00\x00\x98L\xfb\x07<\xdd^@'

# Test byte order and alignment
assert struct.pack('>ih5s', 256, 6553,
                   b'Python') == b'\x00\x00\x01\x00\x19\x99Pytho'

# Test combination of multiple values
#! NOT SUPPORTED
# assert struct.pack('chHiIq', b'A', 127, 3268, 2147, 42945, 9220) == \
#     b'A\x00\x7f\x00\xc4\x0c\x00\x00c\x08\x00\x00\xc1\xa7\x00\x00\x04$\x00\x00\x00\x00\x00\x00'

print('PASS')
