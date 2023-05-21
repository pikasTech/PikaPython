# Testing with builtin types
assert isinstance(10, int) == True
assert isinstance("Hello, world!", str) == True
assert isinstance([1, 2, 3, 4, 5], list) == True
assert isinstance({"key": "value"}, dict) == True
assert isinstance(3.14, float) == True

class BaseClass:
    def __init__(self):
        self.a = 1

class DerivedClass(BaseClass):
    def __init__(self):
        super().__init__()
        self.b = 2

base_instance = BaseClass()
derived_instance = DerivedClass()

# Instances of DerivedClass should also be instances of BaseClass
assert isinstance(base_instance, BaseClass) == True
assert isinstance(derived_instance, BaseClass) == True

# However, instances of BaseClass are not instances of DerivedClass
assert isinstance(base_instance, DerivedClass) == False

# And instances of DerivedClass are, of course, instances of DerivedClass
assert isinstance(derived_instance, DerivedClass) == True

print('PASS')
