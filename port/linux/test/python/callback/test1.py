

class Demo():
    def __init__(self):
        print("__init__")
        self.funcs = []
        self.funcs.append(self.a)
        self.funcs.append(self.b)


    def a(self):
        print('a')

    def b(self):
        print('b')

    def get_funcs(self):
        return self.funcs


demo = Demo()
funcs = demo.get_funcs()
print('----------------------------')
for func in funcs:
    func()
