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


class DynamicField:
    def __init__(self, value=None):
        self.data = {"answer": 0}
        if value is not None:
            self.data = value["data"]


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
dynamic = DynamicField({"data": {"answer": 42}})
print(dynamic.data["answer"])
