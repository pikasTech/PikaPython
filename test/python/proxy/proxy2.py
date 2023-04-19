
_dict = {}

class TestProxy:
    def __getattr__(self, name):
        return _dict[name]
    

    def __setattr__(self, name, value):
        _dict[name] = value


test = TestProxy()
test.A = 10
assert test.A == 10

class Test:
    proxy = TestProxy()

test2 = Test()
test2.proxy.B = 20
assert test2.proxy.B == 20

print('PASS')
