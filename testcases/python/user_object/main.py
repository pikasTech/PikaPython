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


class Bucket:
    def __init__(self):
        self.values = []

    def add(self, value):
        self.values.append(value)

    def take(self):
        return self.values.pop()


counter = Derived(3)
print(counter.value)
print(counter.add(4))
print(counter.double())
bucket = Bucket()
bucket.add(11)
bucket.add(22)
print(bucket.take())
counter.value = 5
print(counter.double())
