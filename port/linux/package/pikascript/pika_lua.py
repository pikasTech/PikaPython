import _pika_lua

"""
PikaPython lua auto binging module

- Import lua module as a python module

``` python
import pika_lua

# import lua module as a python module
lua_math = pika_lua.require("math")

# get vars from lua module
print(lua_math.pi)

# call lua function
print(lua_math.sin(1))
```

- Eval lua code

``` python
import pika_lua

# eval lua code
print(pika_lua.eval("print(1 + 1)"))
```
"""


def eval(cmd: str):
    return _pika_lua.eval(cmd)


def evalLine(line: str):
    return _pika_lua.evalLine(line)


def getVar(name: str):
    # print("getVar: %s" % name)
    return eval("return %s" % name)


def setVar(name: str, value):
    # print("setVar: %s = %s" % (name, value))
    eval("%s = %s" % (name, value))


class LuaModuleProxy:
    _name: str = ''

    def __init__(self, name: str):
        self._name = name

    def __getattr__(self, name: str):
        return getVar("%s.%s" % (self._name, name))

    def __setattr__(self, name: str, value) -> None:
        if (name == "_name"):
            setattr(self, name, value)
        else:
            setVar("%s.%s" % (self._name, name), value)

    def __proxy__(self, methodName, *args):
        strArgs = []
        for arg in args:
            strArgs.append(str(arg))

        return eval("return %s.%s(%s)" % (self._name, methodName, ",".join(strArgs)))


def require(module: str) -> LuaModuleProxy:
    try:
        eval("require('%s')" % module)
        return LuaModuleProxy(module)
    except:
        print("require module '%s' failed" % module)
        return None
