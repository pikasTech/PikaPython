dict = {"a": 1, "b": 2, "c": 3}
res = 0
try:
    res += 1
    print("before arise")
    print(dict["d"])
    print("after arise")
    res += 2
except:
    print("in excpet")
    res += 4
