import json


value = {
    "text": "Pika中",
    "flag": True,
    "none": None,
    2: "int",
    1.5: "float",
}
print("default:", json.dumps(value))
print("nested:", json.dumps({"items": [1, -2, 1.5, False, None, "中"]}))
print("load-text:", json.loads('{"x": 1}')["x"])
print("load-bytes:", json.loads(b'{"x": 2}')["x"])
print("load-bytearray:", json.loads(bytearray(b'{"x": 3}'))["x"])
try:
    json.loads('{"x": 1} trailing')
    print("trailing-data: accepted")
except Exception:
    print("trailing-data: rejected")
try:
    json.loads('{"x": [1,]}')
    print("invalid-separator: accepted")
except Exception:
    print("invalid-separator: rejected")
print("recovery:", json.loads('{"ok": true}')["ok"])
