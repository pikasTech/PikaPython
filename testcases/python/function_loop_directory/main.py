# SPEC: PJ2026-0501 V2 kernel v0.3; directory entry regression.
def score(limit):
    total = 0
    for value in range(limit):
        if value == 2:
            continue
        total = total + value
    return total


print(score(6))
