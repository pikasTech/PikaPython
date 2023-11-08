# mult.py
assert 1 * 2 == 2
assert 1 * 2 * 3 == 6
assert 1 * 2 * 3 * 4 == 24
assert 'a' * 3 == 'aaa'
assert 'a' * 3 * 2 == 'aaaaaa'
assert 3 * 'bb' == 'bbbbbb'
assert 3 * 'bb' * 2 == 'bbbbbbbbbbbb'
assert 3 * 'bb' * 2 * 3 == 'bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb'
assert b'x' * 3 == b'xxx'
assert b'x' * 3 * 2 == b'xxxxxx'
assert 3 * b'yy' == b'yyyyyy'
assert 3 * b'yy' * 2 == b'yyyyyyyyyyyy'

assert [1,2,3] == [1,2,3]
assert [1,2,3] != [1,2,4]
assert 2 * [1, 2, 3] == [1, 2, 3, 1, 2, 3]
assert [1, 2, 3] * 2 == [1, 2, 3, 1, 2, 3]
assert 3 * [1, 2, 3] == [1, 2, 3, 1, 2, 3, 1, 2, 3]
assert [1, 2, 3] * 3 == [1, 2, 3, 1, 2, 3, 1, 2, 3]

print('PASS')