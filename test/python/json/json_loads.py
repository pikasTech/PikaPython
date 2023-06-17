import json
import time

res = json.loads('{"a": 1, "b": 2, "c": 3}')
assert res['a'] == 1
assert res['b'] == 2
assert res['c'] == 3

res = json.loads('{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}}')
assert res['a'] == 1
assert res['b'] == 2
assert res['c'] == 3
assert res['d']['e'] == 4
assert res['d']['f'] == 5

res = json.loads(
    '{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8]}')
assert res['a'] == 1
assert res['b'] == 2
assert res['c'] == 3
assert res['d']['e'] == 4
assert res['d']['f'] == 5
assert res['g'][0] == 6
assert res['g'][1] == 7
assert res['g'][2] == 8

res = json.loads(
    '{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8], "h": null}')
assert res['a'] == 1
assert res['b'] == 2
assert res['c'] == 3
assert res['d']['e'] == 4
assert res['d']['f'] == 5
assert res['g'][0] == 6
assert res['g'][1] == 7
assert res['g'][2] == 8
assert res['h'] is None

res = json.loads(
    '{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8], "h": null, "i": false}')
assert res['a'] == 1
assert res['b'] == 2
assert res['c'] == 3
assert res['d']['e'] == 4
assert res['d']['f'] == 5
assert res['g'][0] == 6
assert res['g'][1] == 7
assert res['g'][2] == 8
assert res['h'] is None
assert res['i'] is False

res = json.loads(
    '{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8], "h": null, "i": false, "j": true}')
assert res['a'] == 1
assert res['b'] == 2
assert res['c'] == 3
assert res['d']['e'] == 4
assert res['d']['f'] == 5
assert res['g'][0] == 6
assert res['g'][1] == 7
assert res['g'][2] == 8
assert res['h'] is None
assert res['i'] is False
assert res['j'] is True

res = json.loads(
    '{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8], "h": null, "i": false, "j": true, "k": "string"}')
assert res['a'] == 1
assert res['b'] == 2
assert res['c'] == 3
assert res['d']['e'] == 4
assert res['d']['f'] == 5
assert res['g'][0] == 6
assert res['g'][1] == 7
assert res['g'][2] == 8
assert res['h'] is None
assert res['i'] is False
assert res['j'] is True
assert res['k'] == 'string'

res = json.loads(
    '{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8], "h": null, "i": false, "j": true, "k": "string", "l": 1.234}')
assert res['a'] == 1
assert res['b'] == 2
assert res['c'] == 3
assert res['d']['e'] == 4
assert res['d']['f'] == 5
assert res['g'][0] == 6
assert res['g'][1] == 7
assert res['g'][2] == 8
assert res['h'] is None
assert res['i'] is False
assert res['j'] is True
assert res['k'] == 'string'
assert res['l'] == 1.234
print('PASS')

