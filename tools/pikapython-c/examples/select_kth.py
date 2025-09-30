"""
Deterministic linear-time selection (median-of-medians) example.
提供一个单函数 `select_kth(arr, k)`：返回无序列表中第 k 小的元素（0-based）。
包含自检：随机数组测试与边界情况，确保在 CPython 下运行通过。
"""

import random


def select_kth(arr, k):
    """Return the k-th smallest element of arr (0-based index) using
    the median-of-medians algorithm to guarantee linear time.

    Args:
        arr: list of comparable items
        k: int, 0 <= k < len(arr)

    Returns:
        The k-th smallest element.

    Raises:
        ValueError: if k is out of range or arr is empty.
    """
    if not arr:
        raise ValueError('empty array')
    if k < 0 or k >= len(arr):
        raise ValueError('k out of range')

    def partition(a, pivot):
        lows = []
        highs = []
        pivots = []
        for x in a:
            if x < pivot:
                lows.append(x)
            elif x > pivot:
                highs.append(x)
            else:
                pivots.append(x)
        return lows, pivots, highs

    def median_of_medians(a):
        # divide a into groups of 5
        groups = [a[i:i+5] for i in range(0, len(a), 5)]
        medians = []
        for g in groups:
            g.sort()
            medians.append(g[len(g)//2])
        if len(medians) <= 5:
            medians.sort()
            return medians[len(medians)//2]
        # recursively find median of medians
        return select_median(medians)

    def select_median(a):
        # helper to select median index
        return median_of_medians(a)

    def select(a, k):
        if len(a) <= 10:
            # small array: sort and return
            b = sorted(a)
            return b[k]
        pivot = median_of_medians(a)
        lows, pivots, highs = partition(a, pivot)
        if k < len(lows):
            return select(lows, k)
        elif k < len(lows) + len(pivots):
            return pivots[0]
        else:
            return select(highs, k - len(lows) - len(pivots))

    return select(list(arr), k)


def _test_small_cases():
    # basic sanity checks
    assert select_kth([1], 0) == 1
    assert select_kth([2, 1], 0) == 1
    assert select_kth([2, 1], 1) == 2
    assert select_kth([3,1,2], 1) == 2
    assert select_kth([5,4,3,2,1], 0) == 1
    assert select_kth([5,4,3,2,1], 4) == 5


def _test_randomized(n=100, trials=50):
    for _ in range(trials):
        length = random.randint(1, n)
        arr = [random.randint(-1000, 1000) for _ in range(length)]
        k = random.randrange(length)
        expected = sorted(arr)[k]
        got = select_kth(arr, k)
        if got != expected:
            print('FAILED on', arr, 'k=', k, 'expected=', expected, 'got=', got)
            raise AssertionError('select_kth mismatch')


def _test_edge_cases():
    # duplicates
    arr = [5] * 10
    assert select_kth(arr, 0) == 5
    assert select_kth(arr, 9) == 5


if __name__ == '__main__':
    print('Running select_kth self-tests...')
    _test_small_cases()
    _test_edge_cases()
    _test_randomized(n=200, trials=100)
    print('All tests passed for select_kth')
