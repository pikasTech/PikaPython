assert type('test') == str
assert type(b'test') == bytes
assert type(bytearray(b'test')) == bytearray
assert type(1) == int
assert type(1.0) == float
assert type(True) == bool
assert type([]) == list
assert type({}) == dict
assert type(()) == tuple
print('PASS')
