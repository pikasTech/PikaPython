
del_times = 0

class Test:
    def __del__(self):
        global del_times
        # print("del")
        del_times += 1

def test_del():
    t = Test()
    assert del_times == 0
    del t
    assert del_times == 1

test_del()
print("PASS")
