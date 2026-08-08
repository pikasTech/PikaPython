def bench_arith(n):
    i = 0
    acc = 0
    while i < n:
        acc = (acc + i * 3 + 7) % 1000003
        i = i + 1
    return acc


def fib_iter(n):
    i = 0
    a = 0
    b = 1
    while i < n:
        c = a + b
        a = b
        b = c
        i = i + 1
    return a


def bench_calls(n):
    i = 0
    acc = 0
    while i < n:
        acc = acc + fib_iter(8)
        i = i + 1
    return acc


def bench_list(n):
    data = []
    i = 0
    while i < n:
        data.append((i * 7) % 1009)
        i = i + 1
    i = 0
    acc = 0
    while i < n:
        acc = (acc + data[i]) % 1000003
        i = i + 1
    return acc


print("bench:v1")
print("arith", bench_arith(300000))
print("calls", bench_calls(60000))
print("list", bench_list(20000))
