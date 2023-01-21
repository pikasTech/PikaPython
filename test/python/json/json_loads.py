import json

json.loads('{"a": 1, "b": 2, "c": 3}')

json.loads('{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}}')

json.loads('{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8]}')

json.loads('{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8], "h": null}')

json.loads('{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8], "h": null, "i": false}')

json.loads('{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8], "h": null, "i": false, "j": true}')

json.loads('{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8], "h": null, "i": false, "j": true, "k": "string"}')

json.loads('{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8], "h": null, "i": false, "j": true, "k": "string", "l": 1.234}')
