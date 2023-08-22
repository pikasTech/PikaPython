import struct
unpacked_bytes = struct.unpack('bbhhh', b'\x01\x02\x03\x04\x05\x06\x07\x08')
assert unpacked_bytes[0] == 1
assert unpacked_bytes[1] == 2
assert unpacked_bytes[2] == 1027
assert unpacked_bytes[3] == 1541
assert unpacked_bytes[4] == 2055

unpacked_bytes = struct.unpack('If', b'\x01\x00\x00\x00\x42\x28\xAE\x47')

assert unpacked_bytes[0] == 1
assert unpacked_bytes[1] == 89168.515625

unpacked_string_numbers = struct.unpack(
    '5sIf', b'Hello\x00\x00\x00\x01\x00\x00\x00B(\xaeG')
assert unpacked_string_numbers[0] == b'Hello'
assert unpacked_string_numbers[1] == 1
assert unpacked_string_numbers[2] == 89168.515625

unpacked_mixed_data = struct.unpack(
    'BffI', b'\x01\x00\x00\x00+R\x9aD\x00\x00\x80?P\\\x01\x00')
assert unpacked_mixed_data[0] == 1
assert unpacked_mixed_data[1] == 1234.5677490234375
assert unpacked_mixed_data[2] == 1
assert unpacked_mixed_data[3] == 89168

print('PASS')
