import test_module2
import test_module3


def mytest():
    print('test_module_1_hello')


def test_module_import():
    print('in test module 2')
    test_module2.mytest()
