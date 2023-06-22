import _json

def loads(json: str) -> dict:
    return _json.loads(json)


def dumps(d: dict) -> str:
    return _json.dumps(d)
