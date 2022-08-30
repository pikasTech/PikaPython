res = 0
try:
    res += 1
    print("before arise")
    len(a)
    print("after arise")
    res += 2
except:
    print("in excpet")
    res += 4
