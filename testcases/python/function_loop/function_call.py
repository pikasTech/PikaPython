# SPEC: PJ2026-0501 V2 kernel v0.3; positional call regression.
def mix(left, right, factor):
    adjusted = left + right
    return adjusted * factor % 17


print(mix(4, 5, 3))
