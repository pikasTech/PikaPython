import json

s0 = json.dumps(1)

s1 = json.dumps({"a": 1, "b": 2, "c": 3})
print(s1)

s2 = json.dumps(
    {
        "a": 1,
        "b": 2,
        "c": 3,
        "d": {
            "e": 4,
            "f": 5
        }
    }
)
print(s2)

s3 = json.dumps(
    {
        "a": 1,
        "b": 2,
        "c": 3,
        "d": {
            "e": 4,
            "f": 5
        },
        "g": [
            6,
            7,
            8
        ]
    }
)
print(s3)

s4 = json.dumps(
    {
        "a": 1,
        "b": 2,
        "c": 3,
        "d": {
            "e": 4,
            "f": 5
        },
        "g": [
            6,
            7,
            8
        ],
        "h": None,
        "i": False,
        "j": True,
        "k": "string",
        "l": 1.234
    }
)
print(s4)
