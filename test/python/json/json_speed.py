import json
import time

json.CONFIG_USING_JSMN = False
start = time.tick_ms()
for i in range(1000):
    res = json.loads(
        '{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8], "h": null, "i": false, "j": true, "k": "string", "l": 1.234}')
end = time.tick_ms()
time_cjson = end - start
print('cjson:', time_cjson, 'ms')

json.CONFIG_USING_JSMN = True
start = time.tick_ms()
for i in range(1000):
    res = json.loads(
        '{"a": 1, "b": 2, "c": 3, "d": {"e": 4, "f": 5}, "g": [6, 7, 8], "h": null, "i": false, "j": true, "k": "string", "l": 1.234}')
end = time.tick_ms()
time_jsmn = end - start
print('jsmn:', time_jsmn, 'ms')

print('jsmn is', (time_cjson / time_jsmn), 'times faster than cjson')
