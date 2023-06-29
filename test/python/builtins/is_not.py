
assert (1 is not 1) == False
assert (1 is not 2) == True
assert (1 is not None) == True
assert (None is not None) == False
assert (None is not 1) == True
assert (1 is not 1.0) == True
assert (1.0 is not 1) == True
assert (1.0 is not 1.0) == False
print("PASS")
