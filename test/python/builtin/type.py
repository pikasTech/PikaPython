
class Test1:
    def test(self):
        return 'test 1'


class Test2:
    def test(self):
        return 'test 2'

t1 = Test1()
tt1 = type(t1)
ttt1 = tt1()

t2 = Test2()
tt2 = type(t2)
ttt2 = tt2()

assert ttt1.test() == 'test 1'
assert ttt2.test() == 'test 2'
assert type(ttt1) == Test1
assert type(ttt2) == Test2
assert type(ttt1) != type(ttt2)
assert type(ttt1) != Test2
assert type(ttt2) != Test1

print('PASS')
