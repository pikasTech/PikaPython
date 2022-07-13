import PikaStdLib
import test
from pika_cjson import cJSON
print('hello pikascript!')
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()

json = cJSON()
json.parse('{"name": "mculover666","age": 22,"weight": 55.5,"address":{"country": "China","zip-code": 111111"},"skill": ["c", "Java", "Python"],"student": false}')
json.print()
print('END')
