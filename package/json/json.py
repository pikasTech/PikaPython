import pika_cjson as cjson
import _json

CONFIG_USING_CJSON = False


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
        num_f = cjson.getValueDouble()
        num_i = cjson.getValueInt()
        if num_f == num_i:
            return num_i
        else:
            return num_f
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
    if CONFIG_USING_CJSON:
        cj = cjson.Parse(json)
        return _cjson_encode(cj)
    else:
        return _json.loads(json)


def _cjson_decode(d: dict):
    if d == None:
        return cjson.Null()
    elif type(d) == int:
        return cjson.Number(d)
    elif type(d) == float:
        return cjson.Number(d)
    elif type(d) == bool:
        if d:
            return cjson.True_()
        else:
            return cjson.False_()
    elif type(d) == str:
        return cjson.String(d)
    elif type(d) == list:
        res = cjson.Array()
        for i in d:
            res.addItemToArray(_cjson_decode(i))
        return res
    elif type(d) == dict:
        res = cjson.Object()
        for k, v in d.items():
            res.addItemToObject(k, _cjson_decode(v))
        return res
    else:
        return cjson.Null()


def dumps(d: dict) -> str:
    if CONFIG_USING_CJSON:
        return _cjson_decode(d).print()
    else:
        return _json.dumps(d)
