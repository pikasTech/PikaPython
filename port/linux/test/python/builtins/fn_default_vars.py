def collect(a, b=2, *args):
    return a, b, args


class Collector:
    def collect(self, a, b=2, *args):
        return a, b, args


a, b, args = collect(1, 3, 4, 5)
assert a == 1
assert b == 3
assert args == (4, 5)

a, b, args = collect(1)
assert a == 1
assert b == 2
assert args == ()

a, b, args = collect(1, b=4)
assert a == 1
assert b == 4
assert args == ()

collector = Collector()
a, b, args = collector.collect(1, 3, 4, 5)
assert a == 1
assert b == 3
assert args == (4, 5)

a, b, args = collector.collect(1)
assert a == 1
assert b == 2
assert args == ()

def collect_all(a, b=2, *args, **kwargs):
    return a, b, args, kwargs


a, b, args, kwargs = collect_all(1, 3, 4, 5, alpha=6, beta=7)
assert a == 1
assert b == 3
assert args == (4, 5)
assert kwargs['alpha'] == 6
assert kwargs['beta'] == 7

print('PASS')
