
class TestProxy:
    def __proxy__(self, name, value):
        return name, value

test = TestProxy()
name, value = test.function1(123)
assert name == 'function1'
assert value == 123
print('PASS')
