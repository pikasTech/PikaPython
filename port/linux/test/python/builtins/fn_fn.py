

def test1():
    return "test1"

def test():
    return test1

l = [test1]

assert l[0]() == "test1"
assert test()() == "test1"
print("PASS")
