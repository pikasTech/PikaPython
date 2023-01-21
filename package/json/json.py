import pika_cjson as cjson


def _cjson_encode(cjson):
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
