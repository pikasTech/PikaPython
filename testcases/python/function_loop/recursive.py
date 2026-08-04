# SPEC: PJ2026-0501 V2 kernel v0.3; recursive call regression.
def fib(value):
    if value < 2:
        return value
    return fib(value - 1) + fib(value - 2)


print(fib(10))
