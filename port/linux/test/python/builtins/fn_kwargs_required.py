def func(a, **kwargs):
    return a, kwargs


a, kwargs = func(a=1)
assert a == 1
assert kwargs == {}

print('PASS')
