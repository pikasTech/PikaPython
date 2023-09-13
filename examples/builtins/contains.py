# test for __contains__

assert 't' in 'test'
assert 'q' not in 'test'
assert 'te' in 'test'
assert 'tq' not in 'test'

assert b't' in b'test'
assert b'q' not in b'test'
assert b'te' in b'test'
assert b'tq' not in b'test'

assert bytearray(b't') in bytearray(b'test')
assert bytearray(b'q') not in bytearray(b'test')
assert bytearray(b'te') in bytearray(b'test')
assert bytearray(b'tq') not in bytearray(b'test')

assert 1 in range(10)
assert 10 not in range(10)

assert (1,) in [(1,), (2,)]
assert (1,) not in [(2,), (3,)]

assert 'a' in {'a': 1, 'b': 2}
assert 'c' not in {'a': 1, 'b': 2}

print('PASS')
