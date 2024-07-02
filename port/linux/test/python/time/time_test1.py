import time
tm = time.localtime()
print(tm)
print(tuple(tm))

assert tuple(time.localtime(1719905967)) == (2024, 7, 2, 15, 39, 27, 1, 184, 0)
assert tuple(time.localtime(1519905963)) == (2018, 3, 1, 20, 6, 3, 3, 60, 0)
assert tuple(time.localtime(1919105967)) == (
    2030, 10, 25, 4, 59, 27, 4, 298, 0)

assert time.mktime((2024, 7, 2, 15, 39, 27, 1, 184, 0)) == 1719905967
assert time.mktime((2018, 3, 1, 20, 6, 3, 3, 60, 0)) == 1519905963
assert time.mktime((2030, 10, 25, 4, 59, 27, 4, 298, 0)) == 1919105967
print('PASS')
