# SPEC: PJ2026-0501 V2 kernel v0.3; range lowering regression.
total = 0
for value in range(2, 7):
    if value == 4:
        continue
    total = total + value
for value in range(2):
    total = total + value
print(total)
