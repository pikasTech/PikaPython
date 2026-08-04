class Counter:
    def __init__(self, start):
        self.value = start

    def add(self, amount):
        self.value += amount
        return self.value


class Derived(Counter):
    def double(self):
        self.value += self.value
        return self.value


counter = Derived(3)
print(counter.value)
print(counter.add(4))
print(counter.double())
counter.value = 5
print(counter.double())
