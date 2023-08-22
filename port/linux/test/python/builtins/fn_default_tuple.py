
def test_default_tuple(t=(1, 2), l=[3, 4], d={'a': 5, 'b': 6}):
    return t, l, d


tt, ll, dd = test_default_tuple()
assert tt[0] == 1
assert tt[1] == 2
assert ll[0] == 3
assert ll[1] == 4
assert dd['a'] == 5
assert dd['b'] == 6
print('PASS')
