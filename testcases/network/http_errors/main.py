import requests
from network_fixture import HOST, PORT


BASE_URL = "http://" + HOST + ":" + str(PORT)


try:
    requests.get("https://example.com/")
except ValueError:
    print("unsupported-scheme")

try:
    requests.get(
        BASE_URL + "/unused",
        {"X-Test": "ok\r\nX-Evil: yes"},
        1,
    )
except ValueError:
    print("header-injection")

try:
    requests.get("x" * 513)
except ValueError:
    print("url-limit")

try:
    requests.get(BASE_URL + "/" + "x" * 257, timeout=1)
except ValueError:
    print("path-limit")

try:
    requests.get(
        BASE_URL + "/unused",
        {"X-Large": "x" * 2048},
        1,
    )
except ValueError:
    print("request-header-limit")

try:
    requests.get("http://127.0.0.1:65536/", timeout=1)
except ValueError:
    print("port-limit")

try:
    requests.get(
        BASE_URL + "/unused",
        {"Host": "invalid"},
        1,
    )
except ValueError:
    print("reserved-header")


def expect_protocol_error(path):
    try:
        requests.get(BASE_URL + path, timeout=1)
    except ValueError:
        print(path)
    response = requests.get(
        BASE_URL + "/recover" + path,
        timeout=1,
    )
    print(response.status_code)


expect_protocol_error("/bad-status")
expect_protocol_error("/truncated")
expect_protocol_error("/negative")
expect_protocol_error("/early-eof")
expect_protocol_error("/duplicate-length")
expect_protocol_error("/transfer-encoding")
expect_protocol_error("/non-ascii-header")
expect_protocol_error("/invalid-header-line")
expect_protocol_error("/header-count-limit")
expect_protocol_error("/header-bytes-limit")
expect_protocol_error("/body-length-limit")
expect_protocol_error("/eof-body-limit")
