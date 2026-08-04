import json


value = {
    "text": "Pika中",
    "flag": True,
    "none": None,
    2: "int",
    1.5: "float",
}
print("json-default:", json.dumps(value))
print("json-load-text:", json.loads('{"x": 1}')["x"])
print("json-load-bytes:", json.loads(b'{"x": 2}')["x"])
print("json-load-bytearray:", json.loads(bytearray(b'{"x": 3}'))["x"])
try:
    json.loads('{"x": 1} trailing')
    print("json-invalid: accepted")
except Exception:
    print("json-invalid: rejected")
print("json-recovery:", json.loads('{"ok": true}')["ok"])
