import re


print("flags:", int(re.A), int(re.I), int(re.M), int(re.S))
matched = re.match("[A-Z]+", "PIKA2")
print("match:", matched.group(), matched.span())
searched = re.search("[0-9]+", "Pika2026V2")
print("search:", searched.group(), searched.span())
print("fullmatch:", re.fullmatch("[a-z]+[0-9]+", "pika2").group())
print("ignorecase:", re.fullmatch("pika", "PIKA", re.I).group())
print("multiline:", re.search("^two$", "one\ntwo", re.M).span())
print("dotall:", re.fullmatch("a.b", "a\nb", re.S) is not None)
print("ascii:", re.fullmatch("[a-z]+", "pika", re.A) is not None)
print("escape:", re.escape("a&b~c# d"))
print("miss:", re.search("z+", "pika") is None)
try:
    re.match("(", "pika")
    print("invalid-pattern: accepted")
except Exception:
    print("invalid-pattern: rejected")
