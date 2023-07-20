# True and False are the only values for the bool type.
assert isinstance(True, bool)
assert isinstance(False, bool)

# The bool function allows other data types to be converted to bool types.
assert bool(1) == True
assert bool(0) == False
assert bool([]) == False
assert bool([1, 2, 3]) == True
assert bool("") == False
assert bool("hello") == True
assert bool(None) == False

# Boolean values are also integers
assert True == 1
assert False == 0

# Boolean arithmetic
assert True + True == 2
assert True - False == 1
assert True * True == 1
assert True / True == 1.0

# Boolean and bitwise operators
assert (True and False) == False
assert (True or False) == True
assert (not True) == False
assert (not False) == True

# Using bool in a if condition
if True:
    assert True
else:
    assert False

if False:
    assert False
else:
    assert True

print("PASS")
