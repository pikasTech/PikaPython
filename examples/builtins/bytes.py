def test():
    a = bytes(10)
    a[1] = 1
    return a

print(test())
res = test()
