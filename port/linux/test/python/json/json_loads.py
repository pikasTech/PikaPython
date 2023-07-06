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


# Testing empty object and empty array using type and len
res = json.loads('{}')
assert type(res) == dict
assert len(res) == 0

res = json.loads('[]')
assert type(res) == list
assert len(res) == 0

res = json.loads('{"a": {}}')
assert type(res['a']) == dict
assert len(res['a']) == 0

res = json.loads('{"a": []}')
assert type(res['a']) == list
assert len(res['a']) == 0

res = json.loads('{"a": 1, "b": {}, "c": []}')
assert res['a'] == 1
assert type(res['b']) == dict
assert len(res['b']) == 0
assert type(res['c']) == list
assert len(res['c']) == 0

res = json.loads(
    '{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": {}}, "g": [6, {}, 8], "h": null, "i": false, "j": true, "k": "string", "l": 1.234}')
assert res['a'] == 1
assert res['b'] == 2
assert res['c'] == 3
assert res['d']['e'] == 4
assert type(res['d']['f']) == dict
assert len(res['d']['f']) == 0
assert res['g'][0] == 6
assert type(res['g'][1]) == dict
assert len(res['g'][1]) == 0
assert res['g'][2] == 8
assert res['h'] is None
assert res['i'] is False
assert res['j'] is True
assert res['k'] == 'string'
assert res['l'] == 1.234
print('PASS')
