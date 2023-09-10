def test_kw(**kw):
    if 'a' in kw:
        return kw['a']
    if 'b' in kw:
        return kw['b']

a = test_kw(a=1)
b = test_kw(b=2)
assert a == 1
assert b == 2

print('PASS')
