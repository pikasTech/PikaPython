# re for pikaScript

This module is made for pikaScript, aiming at providing the same usage as the re module in Python.

## import module

``` python
import re
```

## usage

The functions in this module have almost the same name and usage as the functions in the re module in python, but some features in the python.re module are not avaliable here, pikaScript does not support default arguments for example. 

Here we provide some demonstration programs.

### match

``` python
import re
 
line = "Cats are smarter than dogs"
 
m = re.match( '(.*) are (.*?) .*', line, re.M|re.I)
 
if m:
   print("matchObj.group(0) : ", m.group(0))
   print("matchObj.group(1) : ", m.group(1))
   print("matchObj.group(2) : ", m.group(2))
else:
   print("No match!!")

'''>> runing output
matchObj.group(0) :  Cats are smarter than dogs
matchObj.group(1) :  Cats
matchObj.group(2) :  smarter
'''
```

### search

``` python

print(re.search('www', 'www.runoob.com').span(0))
print(re.search('com', 'www.runoob.com').span(0)) 

'''>> running output
[0, 3]
[11, 14]
'''
```

### sub

```python

phone = "2004-959-559 # this is a phone number"
num = re.sub('#.*$', "", phone)
print("the phone number is: ", num)
num = re.sub('\\D', "", phone)
print("the phone number is: ", num)

'''>> running output
the phone number is:  2004-959-559
the phone number is:  2004959559
'''

```

### findall

``` python
# year-month-day
pattern = re.compile('(\\d{4})-([1-9]|1[0-2])-([1-9]|[1-2][0-9]|3[01])\\b')
s = 'date: 2020-1-1, 2022-12-22, 2018-3-31. Wrong format: 2031-13-31, 2032-12-33 ...'
result1 = pattern.findall(s)
print(result1)
result2 = pattern.sub('\\1',s)
print(result2)

'''>> running output
[['2020-1-1', '2020', '1', '1'], ['2022-12-22', '2022', '12', '22'], ['2018-3-31', '2018', '3', '31']]
date: 2020, 2022, 2018. Wrong format: 2031-13-31, 2032-12-33 ...

'''
```

## the API

This module prototype are likes this:

``` python

# flags

A: int
ASCII: int
I: int
IGNORECASE: int
M: int
MULTILINE: int
S: int
DOTALL: int

class Pattern:
    def __init__(self):
        pass

    def __del__(self):
        pass

    def findall(self, subject: str, *flags) -> list:
        pass

    def sub(self, repl: str, subjet: str, *count__flags) -> str:
        pass

    def subn(self, repl: str, subjet: str, *count__flags) -> list:
        pass

    def match(self, subject: str, *flags) -> Match:
        pass

    def fullmatch(self, subject: str, *flags) -> Match:
        pass

    def search(self, subject: str, *flags) -> Match:
        pass

    def split(self, subject: str, *maxsplit__flags) -> list:
        pass


class Match:
    def __init__(self):
        pass

    def __del__(self):
        pass

    def group(self, *n) -> str:
        pass

    def groups(self) -> list:
        pass

    def span(self, *group_n) -> list:
        pass


def findall(pattern: str, subject: str, *flags) -> list: ...
# def sub(pattern, repl, string, count=0, flags=0)
def sub(pattern: str, repl: str, subjet: str, *count__flags) -> str: ...
def match(pattern: str, subject: str, *flags) -> Match: ...
def fullmatch(pattern: str, subject: str, *flags) -> Match: ...
def search(pattern: str, subject: str, *flags) -> Match: ...
def compile(pattern: str, *flags) -> Pattern: ...
def escape(pattern: str) -> str: ...
# def subn(pattern, repl, string, count=0, flags=0)
def subn(pattern: str, repl: str, subjet: str, *count__flags) -> list: ...
# def finditer(pattern: str, subject: str, *flags):
def split(pattern: str, subject: str, *maxsplit__flags) -> list: ...

```
