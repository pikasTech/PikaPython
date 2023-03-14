
def test1(i):
    def test2(i):
        print('in test2', 'var:', i)

    test2(i)


test1(1)
