import _pika_lua

"""
## PikaPython lua auto binging module

### Import lua module as a python module
``` python
import pika_lua

# import lua module as a python module
lua_math = pika_lua.require("math")

# get vars from lua module
print(lua_math.pi)

# call lua function
print(lua_math.sin(1))
```

### Eval lua code

``` python
import pika_lua

# eval lua code
print(pika_lua.eval("print(1 + 1)"))
"""

def eval(cmd: str):
    return _pika_lua.eval(cmd)

def evalLine(line: str):
    return _pika_lua.evalLine(line)

