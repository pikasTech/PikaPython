import pika_cjson as cjson


def _cjson_encode(cjson: cjson.cJSON):
    if cjson == None:
        return None
    elif cjson.isInvalid():
        return None
    elif cjson.isFalse():
        return False
    elif cjson.isTrue():
        return True
    elif cjson.isNull():
        return None
    elif cjson.isNumber():
        return cjson.getValueDouble()
    elif cjson.isString():
        return cjson.getValueString()
    elif cjson.isArray():
        res = []
        for i in range(cjson.getArraySize()):
            res.append(_cjson_encode(cjson.getArrayItem(i)))
        return res
    elif cjson.isObject():
        res = {}
        child = cjson.getChild()
        for i in range(cjson.getArraySize()):
            key = child.getString()
            res[key] = _cjson_encode(child)
            child = child.getNext()
        return res
    elif cjson.isRaw():
        return cjson.getString()
    else:
        return None


def loads(json: str) -> dict:
    cj = cjson.Parse(json)
    return _cjson_encode(cj)


def _cjson_decode(d: dict):
    if d == None:
        return cjson.Null()
    elif str(type(d)) == "<class 'int'>":
        return cjson.Number(d)
    elif str(type(d)) == "<class 'float'>":
        return cjson.Number(d)
    elif str(type(d)) == "<class 'bool'>":
        if d:
            return cjson.True_()
        else:
            return cjson.False_()
    elif str(type(d)) == "<class 'str'>":
        return cjson.String(d)
    elif str(type(d)) == "<class 'list'>":
        res = cjson.Array()
        for i in d:
            res.addItemToArray(_cjson_decode(i))
        return res
    elif str(type(d)) == "<class 'dict'>":
        res = cjson.Object()
        for k, v in d.items():
            res.addItemToObject(k, _cjson_decode(v))
        return res
    else:
        return cjson.Null()


def dumps(d: dict) -> str:
    return _cjson_decode(d).print()