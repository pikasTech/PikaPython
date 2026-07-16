def collect(a, b=2, *args):
    return a, b, args


class Collector:
    def collect(self, a, b=2, *args):
        return a, b, args


a, b, args = collect(1, 3, 4, 5)
assert a == 1
assert b == 3
assert args == (4, 5)

collector = Collector()
a, b, args = collector.collect(1, 3, 4, 5)
assert a == 1
assert b == 3
assert args == (4, 5)

print('PASS')
