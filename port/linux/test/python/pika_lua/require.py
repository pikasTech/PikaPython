import pika_lua
lua_math = pika_lua.require("math")
assert lua_math.pi == 3.141592653589793
assert lua_math.sin(0) == 0
assert lua_math.sin(lua_math.pi / 2) == 1
assert lua_math.sin(lua_math.pi) == 0
assert lua_math.sin(lua_math.pi * 3 / 2) == -1
assert lua_math.sin(lua_math.pi * 2) == 0
assert lua_math.cos(0) == 1
assert lua_math.cos(lua_math.pi / 2) == 0
assert lua_math.cos(lua_math.pi) == -1
print('PASS')
