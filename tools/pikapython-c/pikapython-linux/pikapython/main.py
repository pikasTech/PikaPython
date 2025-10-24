import select_kth

# Python baseline function that works in PikaPython environment
def py_select_kth(arr, k):
    """Baseline implementation using simple sorting"""
    if len(arr) == 0:
        return None
    if k < 0 or k >= len(arr):
        return None
    
    # Manual sorting since PikaPython may not support sorted()
    sorted_arr = []
    for i in range(len(arr)):
        sorted_arr.append(arr[i])
    
    # Simple bubble sort (not efficient but works in restricted environment)
    for i in range(len(sorted_arr)):
        for j in range(i + 1, len(sorted_arr)):
            if sorted_arr[i] > sorted_arr[j]:
                temp = sorted_arr[i]
                sorted_arr[i] = sorted_arr[j]
                sorted_arr[j] = temp
    
    return sorted_arr[k]

# Test with first dataset
data1 = [3, 1, 5, 9, 2]
print("[EXAMPLE] Testing with data1:", data1)

# Test k=0 (smallest)
k = 0
py_result1 = py_select_kth(data1, k)
c_result1 = select_kth.SelectKth().select_kth(data1, k)
print("[EXAMPLE] k=", k, "Python:", py_result1, "C module:", c_result1)
assert py_result1 == c_result1, "Test 1 failed: k=0"

# Test k=2 (median)
k = 2
py_result2 = py_select_kth(data1, k)
c_result2 = select_kth.SelectKth().select_kth(data1, k)
print("[EXAMPLE] k=", k, "Python:", py_result2, "C module:", c_result2)
assert py_result2 == c_result2, "Test 2 failed: k=2"

# Test k=4 (largest)
k = 4
py_result3 = py_select_kth(data1, k)
c_result3 = select_kth.SelectKth().select_kth(data1, k)
print("[EXAMPLE] k=", k, "Python:", py_result3, "C module:", c_result3)
assert py_result3 == c_result3, "Test 3 failed: k=4"

# Test with second dataset
data2 = [10, 20, 30, 5, 15]
print("[EXAMPLE] Testing with data2:", data2)

# Test k=0 (smallest)
k = 0
py_result4 = py_select_kth(data2, k)
c_result4 = select_kth.SelectKth().select_kth(data2, k)
print("[EXAMPLE] k=", k, "Python:", py_result4, "C module:", c_result4)
assert py_result4 == c_result4, "Test 4 failed: k=0"

# Test k=2 (median)
k = 2
py_result5 = py_select_kth(data2, k)
c_result5 = select_kth.SelectKth().select_kth(data2, k)
print("[EXAMPLE] k=", k, "Python:", py_result5, "C module:", c_result5)
assert py_result5 == c_result5, "Test 5 failed: k=2"

# Test k=4 (largest)
k = 4
py_result6 = py_select_kth(data2, k)
c_result6 = select_kth.SelectKth().select_kth(data2, k)
print("[EXAMPLE] k=", k, "Python:", py_result6, "C module:", c_result6)
assert py_result6 == c_result6, "Test 6 failed: k=4"

# Test edge cases
print("[EXAMPLE] Testing edge cases")

# Empty list
empty_list = []
py_empty = py_select_kth(empty_list, 0)
c_empty = select_kth.SelectKth().select_kth(empty_list, 0)
print("[EXAMPLE] Empty list - Python:", py_empty, "C module:", c_empty)
assert py_empty is None, "Empty list Python should return None"
assert c_empty is None, "Empty list C module should return None"

# k out of range
py_out_of_range = py_select_kth(data1, 10)
c_out_of_range = select_kth.SelectKth().select_kth(data1, 10)
print("[EXAMPLE] k out of range - Python:", py_out_of_range, "C module:", c_out_of_range)
assert py_out_of_range is None, "Out of range Python should return None"
assert c_out_of_range is None, "Out of range C module should return None"

# Single element list
single = [42]
py_single = py_select_kth(single, 0)
c_single = select_kth.SelectKth().select_kth(single, 0)
print("[EXAMPLE] Single element - Python:", py_single, "C module:", c_single)
assert py_single == c_single, "Single element test failed"

# Duplicate elements
duplicates = [5, 5, 5, 5, 5]
py_dup = py_select_kth(duplicates, 2)
c_dup = select_kth.SelectKth().select_kth(duplicates, 2)
print("[EXAMPLE] Duplicates k=2 - Python:", py_dup, "C module:", c_dup)
assert py_dup == c_dup, "Duplicates test failed"

print("[EXAMPLE] All functional tests passed!")

# Performance test (only if functional tests pass)
print("[EXAMPLE] Starting performance tests...")

import time

# Larger dataset for performance testing
large_data = []
for i in range(100):
    large_data.append(100 - i)  # Reverse sorted to make it challenging

ITER = 1000

# Time Python baseline
py_start = time.time()
for _ in range(ITER):
    result_py = py_select_kth(large_data, 50)
py_end = time.time()
py_total = py_end - py_start
py_mean = py_total / ITER

print("[PERF] python_total:", py_total, "seconds")
print("[PERF] python_mean:", py_mean, "seconds per call")

# Time C module
c_start = time.time()
for _ in range(ITER):
    result_c = select_kth.SelectKth().select_kth(large_data, 50)
c_end = time.time()
c_total = c_end - c_start
c_mean = c_total / ITER

print("[PERF] cmod_total:", c_total, "seconds")
print("[PERF] cmod_mean:", c_mean, "seconds per call")

speedup = py_mean / c_mean if c_mean > 0 else float('inf')
print("[PERF] speedup:", speedup, "x")

# Verify performance test results match
assert result_py == result_c, "Performance test results mismatch"

print("[EXAMPLE][SELFTEST] OK - All tests passed successfully!")