# SPEC: PJ2026-0501 V2 kernel v0.3; while control regression.
count = 0
total = 0
while count < 8:
    count = count + 1
    if count == 3:
        continue
    if count == 7:
        break
    total = total + count
print(total)
