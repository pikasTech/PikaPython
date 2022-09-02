

class Demo():
    def __init__(self):
        print("__init__")
        self.funcs = []
        self.funcs.append(self.a)
        self.funcs.append(self.b)
        self.funcs.append(self.c)
        self.val = 'ppp'


    def a(self):
        print('a')

    def b(self):
        print('b')

    def c(self):
        print(self.val)

    def get_funcs(self):
        return self.funcs

class Test():
    def funcs_test(self):
        demo = Demo()
        funcs = demo.get_funcs()
        print('----------------------------')
        for func in funcs:
            demo.func = func 
            demo.func()

test = Test()
test.funcs_test()

