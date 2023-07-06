import json
# test parse failure

try:
    json.loads('{')
except:
    print('FAIL')

try:
    json.loads('{"a":1,}')
except:
    print('FAIL')

try:
    json.loads('{"a":1,')
except:
    print('FAIL')

try:
    json.loads('{"a":1, "b":}')
except:
    print('FAIL')

try:
    json.loads('{"a":1, "b":, "c":3}')
except:
    print('FAIL')

try:
    json.loads('{"a":1 "b":2}')
except:
    print('FAIL')

try:
    json.loads('{"a":1, "b":2, "c":3,}')
except:
    print('FAIL')

try:
    json.loads('{"a":1, "b"2, "c":3}')
except:
    print('FAIL')
