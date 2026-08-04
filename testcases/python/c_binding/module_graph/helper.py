def add(left, right):
    import native_math
    return native_math.add(left, right)


def increment(start, step):
    import native_math
    counter = native_math.Counter(start)
    return counter.increment(step)
