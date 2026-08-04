def fail():
    raise RuntimeError("failure")


def divide(value):
    try:
        if value == 0:
            raise ValueError("zero")
        print(12 // value)
    except ValueError:
        print(7)
    finally:
        print(9)


divide(3)
divide(0)

try:
    fail()
except RuntimeError:
    print(11)

try:
    try:
        raise TypeError("inner")
    except TypeError:
        raise
except TypeError:
    print(12)
