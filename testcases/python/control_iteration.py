# SPEC: PJ2026-0501; branch, return, and range regression.
def classify(value):
    if value < 0:
        return -1
    elif value == 0:
        pass
        return
    else:
        return 1


print(classify(-3))
print(classify(0))
print(classify(4))

total = 0
for value in range(5, -2, -2):
    total = total + value
for value in range(-2, 5, 3):
    total = total + value
for value in range(0, 3, -1):
    total = total + 1000
print(total)
