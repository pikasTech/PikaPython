class Obj1:
    def test(self):
        print("Obj1.test")

class Test:
    a = Obj1()
    a.test()

t = Test()
