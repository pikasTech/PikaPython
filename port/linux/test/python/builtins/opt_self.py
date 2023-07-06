a = 1

a += -1  # Should result in 0
assert a == 0

a -= -1  # Subtracting -1 is same as adding 1, should result in 1
assert a == 1

a *= -1  # Should result in -1
assert a == -1

# Be careful with dividing by -1 if a is 0, it will raise a ZeroDivisionError
a /= -1  # Should result in 1
assert a == 1

a **= -1  # Should result in 1.0 (since a is 1)
assert a == 1.0

# Be careful with floordiv by -1 if a is 0, it will raise a ZeroDivisionError
a //= -1  # Should result in -1.0
assert a == -1.0

# Here onwards, comparison operators are used, they will return a boolean
assert (a >= -1) == True
assert (a <= -1) == True
assert (a != -1) == False

# Be careful with modulo by -1 if a is 0, it will raise a ZeroDivisionError
a %= -1  # Should result in 0
assert a == 0
print("PASS")
