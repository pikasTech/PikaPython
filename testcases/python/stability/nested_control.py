total = 0
for outer in range(5):
    for inner in range(5):
        if inner == 1:
            continue
        if outer == 3:
            break
        total += outer * 10 + inner
print(total)

index = 0
while index < 20:
    index += 1
    if index % 2 == 0:
        continue
    if index > 11:
        break
    total += index
print(total, index)
