f = open('test/out/_test.txt', 'w')
w = f.write
n = w('test')
f.close()
assert n == 4
print('PASS')
