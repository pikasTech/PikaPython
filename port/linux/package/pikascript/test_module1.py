import test_module2
import test_module3


def mytest():
    print('test_module_1_hello')


def test_module_import():
    print('in test module 2')
    test_module2.mytest()


def for_loop():
    for i in range(0, 10):
        print(i)


def while_loop():
    i = 0
    while i < 4:
        i += 1
        print(i)
