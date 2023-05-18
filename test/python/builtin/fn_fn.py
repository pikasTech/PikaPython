

def test1():
    print("test1")

def test():
    return test1

l = [test1]

l[0]()
test()()
