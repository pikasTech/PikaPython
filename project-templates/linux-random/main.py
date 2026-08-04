import random


random.seed(123)
first = random.random()
random.seed(123)
print("repeat:", first == random.random())
print("unit:", first >= 0.0 and first < 1.0)
print("randint-single:", random.randint(4, 4))
print("randrange-single:", random.randrange(9, 10))
print("randrange-step:", random.randrange(2, 3, 4))
print("uniform-single:", random.uniform(2.5, 2.5))
try:
    random.randrange(0)
    print("empty-range: accepted")
except ValueError:
    print("empty-range: ValueError")
try:
    random.randrange(1, 2, 0)
    print("zero-step: accepted")
except ValueError:
    print("zero-step: ValueError")
