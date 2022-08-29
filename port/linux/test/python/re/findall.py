import re
pattern = re.compile('(\d{4})-([1-9]|1[0-2])-([1-9]|[1-2][0-9]|3[01])\b')
s = 'date: 2020-1-1, 2022-12-22, 2018-3-31. Wrong format: 2031-13-31, 2032-12-33 ...'
result1 = pattern.findall(s)
print(result1)
result2 = pattern.sub('\1',s)
print(result2)
