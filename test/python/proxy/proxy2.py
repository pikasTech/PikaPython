class TestProxy:

    def __setattr__(self, name, value):
        pass

test = TestProxy()
test.A = "FS8000PikaPython"

print('PASS')
