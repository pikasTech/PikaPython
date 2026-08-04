class Counter:
    def __init__(self, value):
        self.value = value

    def increment(self, amount):
        self.value += amount
        return self.value


def add(left, right):
    return left + right
