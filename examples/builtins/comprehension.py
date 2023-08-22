a = [i for i in range(10)]
assert len(a) == 10

for i in range(10):
    q = [x for x in range(i)]
    assert len(q) == i

print('PASS')
