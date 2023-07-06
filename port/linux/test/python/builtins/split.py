a, b = 'test asd'.split()
assert a == 'test'
assert b == 'asd'

a, b = 'test asd'.split(' ')
assert a == 'test'
assert b == 'asd'

print('PASS')
