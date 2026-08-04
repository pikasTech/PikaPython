import _json


def _parse(text, position, depth):
    if depth > _json.DEPTH_LIMIT:
        raise ValueError("JSON input exceeds DEPTH_LIMIT")
    token = _json.scan(text, position)
    kind = token[0]
    if kind >= _json.TOKEN_STRING and kind <= _json.TOKEN_NULL:
        return token[1], token[2]
    if kind != _json.TOKEN_LEFT_BRACKET and kind != _json.TOKEN_LEFT_BRACE:
        raise ValueError("JSON value is invalid")
    mapping = kind == _json.TOKEN_LEFT_BRACE
    if mapping:
        result = {}
        closing = _json.TOKEN_RIGHT_BRACE
    else:
        result = []
        closing = _json.TOKEN_RIGHT_BRACKET
    position = token[2]
    token = _json.scan(text, position)
    if token[0] == closing:
        return result, token[2]
    count = 0
    while True:
        if count >= _json.CONTAINER_MEMBER_LIMIT:
            raise ValueError("JSON container exceeds CONTAINER_MEMBER_LIMIT")
        if mapping:
            token = _json.scan(text, position)
            if token[0] != _json.TOKEN_STRING:
                raise ValueError("JSON object key must be a string")
            key = token[1]
            token = _json.scan(text, token[2])
            if token[0] != _json.TOKEN_COLON:
                raise ValueError("JSON object colon is missing")
            position = token[2]
        parsed = _parse(text, position, depth + 1)
        if mapping:
            result[key] = parsed[0]
        else:
            result.append(parsed[0])
        count += 1
        token = _json.scan(text, parsed[1])
        if token[0] == closing:
            return result, token[2]
        if token[0] != _json.TOKEN_COMMA:
            raise ValueError("JSON container separator is invalid")
        position = token[2]


def loads(text):
    kind = type(text)
    if kind == bytes:
        text = text.decode()
    elif kind == bytearray:
        text = bytes(text).decode()
    elif kind != str:
        raise TypeError("loads expects str")
    if len(text) > _json.INPUT_BYTE_LIMIT:
        raise ValueError("JSON input exceeds INPUT_BYTE_LIMIT")
    parsed = _parse(text, 0, 0)
    if _json.scan(text, parsed[1])[0] != _json.TOKEN_END:
        raise ValueError("JSON input has trailing data")
    return parsed[0]


def _fit(value):
    if len(value) > _json.OUTPUT_BYTE_LIMIT:
        raise ValueError("JSON output exceeds OUTPUT_BYTE_LIMIT")
    return value


def _dump_key(value):
    kind = type(value)
    if kind == str:
        text = value
    elif kind == bool:
        if value:
            text = "true"
        else:
            text = "false"
    elif value is None:
        text = "null"
    elif kind == int or kind == float:
        text = str(value)
    else:
        raise TypeError("JSON object keys must be str, int, float, bool or None")
    if len(text) > _json.STRING_BYTE_LIMIT:
        raise ValueError("JSON string exceeds STRING_BYTE_LIMIT")
    return '"' + _json.escape(text) + '"'


def _dump(value, depth, stack):
    kind = type(value)
    if kind == str:
        if len(value) > _json.STRING_BYTE_LIMIT:
            raise ValueError("JSON string exceeds STRING_BYTE_LIMIT")
        return _fit('"' + _json.escape(value) + '"')
    if kind == bool:
        if value:
            return "true"
        return "false"
    if value is None:
        return "null"
    if kind == int:
        return str(value)
    if kind == float:
        result = str(value)
        if result in ("nan", "inf", "-inf"):
            raise ValueError("non-finite float is not valid JSON")
        return result
    if kind != list and kind != tuple and kind != dict:
        raise TypeError("value is not JSON serializable")
    if depth >= _json.DEPTH_LIMIT:
        raise ValueError("JSON output exceeds DEPTH_LIMIT")
    if len(value) > _json.CONTAINER_MEMBER_LIMIT:
        raise ValueError("JSON container exceeds CONTAINER_MEMBER_LIMIT")
    for active in stack:
        if active is value:
            raise ValueError("circular JSON value")
    stack.append(value)
    mapping = kind == dict
    if mapping:
        result = "{"
        closing = "}"
    else:
        result = "["
        closing = "]"
    index = 0
    for item in value:
        if index != 0:
            result = _fit(result + ", ")
        if mapping:
            encoded = _dump_key(item) + ": " + _dump(
                value[item], depth + 1, stack)
        else:
            encoded = _dump(item, depth + 1, stack)
        result = _fit(result + encoded)
        index += 1
    stack.pop()
    return _fit(result + closing)


def dumps(value):
    return _fit(_dump(value, 0, []))
