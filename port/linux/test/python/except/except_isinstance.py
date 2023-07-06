assert isinstance(ValueError(), Exception) == True
assert isinstance(IndexError(), LookupError) == True
assert isinstance(ZeroDivisionError(), ArithmeticError) == True

assert isinstance(TypeError(), Exception) == True
assert isinstance(KeyError(), LookupError) == True
assert isinstance(OverflowError(), ArithmeticError) == True

assert isinstance(FileNotFoundError(), OSError) == True
assert isinstance(TimeoutError(), OSError) == True

assert isinstance(StopIteration(), Exception) == True
assert isinstance(StopAsyncIteration(), Exception) == True

assert isinstance(ValueError(), IndexError) == False
assert isinstance(IndexError(), ValueError) == False
assert isinstance(ZeroDivisionError(), KeyError) == False

assert isinstance(TypeError(), ZeroDivisionError) == False
assert isinstance(KeyError(), TypeError) == False
assert isinstance(OverflowError(), FileNotFoundError) == False

assert isinstance(FileNotFoundError(), TimeoutError) == False
assert isinstance(TimeoutError(), FileNotFoundError) == False

assert isinstance(StopIteration(), StopAsyncIteration) == False
assert isinstance(StopAsyncIteration(), StopIteration) == False


class CustomException(Exception):
    pass


class SpecificException(CustomException):
    pass


class AnotherException(CustomException):
    pass


assert isinstance(SpecificException(), CustomException) == True
assert isinstance(AnotherException(), CustomException) == True

assert isinstance(SpecificException(), Exception) == True
assert isinstance(AnotherException(), Exception) == True

assert isinstance(CustomException(), SpecificException) == False
assert isinstance(CustomException(), AnotherException) == False
print('PASS')
