"""Small JSON-RPC 2.0 codec with deterministic CPython behaviour."""
import json


def request(method, params=None, request_id=1):
    value = {"jsonrpc": "2.0", "method": method, "id": request_id}
    if params is not None:
        value["params"] = params
    return json.dumps(value)


def notification(method, params=None):
    value = {"jsonrpc": "2.0", "method": method}
    if params is not None:
        value["params"] = params
    return json.dumps(value)


def response(result, request_id=1):
    return json.dumps({"jsonrpc": "2.0", "result": result, "id": request_id})


def error(code, message, request_id=None, data=None):
    item = {"code": code, "message": message}
    if data is not None:
        item["data"] = data
    return json.dumps({"jsonrpc": "2.0", "error": item, "id": request_id})


def loads(payload):
    value = json.loads(payload)
    if not isinstance(value, dict) or value.get("jsonrpc") != "2.0":
        raise ValueError("invalid JSON-RPC 2.0 message")
    return value
