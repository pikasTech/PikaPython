

def bar():
    # local scope
    local = 5
    assert eval('local') == 5


bar()

assert eval('1+1') == 2


def foo():
    return 3


# global scope
assert eval('foo()') == 3

g_val = 4
assert eval('g_val') == 4


print('PASS')
