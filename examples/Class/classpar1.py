class Test:
    a = 1
    b = 'test'
    def __init__(self):
        self.a = 2
        self.b = 'pewq'

print(Test.a)
print(Test.b)

test = Test()
print(test.a)
print(test.b)
