class Counter:
    def __init__(self, value):
        self.value = value

    def add(self, amount):
        self.value += amount
        return self.value


class Derived(Counter):
    def add(self, amount):
        self.value += amount * 2
        return self.value


left = Counter(1)
right = Counter(10)
derived = Derived(5)
alias = left
print(left.add(2), right.add(3), derived.add(4))
print(left.value, right.value, derived.value)
print(alias is left, left is not right)
print(derived.value == 13, left.value == 3)
