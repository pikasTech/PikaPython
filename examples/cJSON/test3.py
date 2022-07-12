import pika_cjson as cjson

root = cjson.Object()
root.addItemToObject('name', cjson.String('mculover666'))
root.addItemToObject('age', cjson.Number(22))
root.addItemToObject('weight', cjson.Number(55.5))
address = cjson.Object()
address.addItemToObject('country', cjson.String('China'))
address.addItemToObject('zip-code', cjson.String('111111'))
root.addItemToObject('address', address)
skill = cjson.Array()
skill.addItemToArray(cjson.String('c'))
skill.addItemToArray(cjson.String('Java'))
skill.addItemToArray(cjson.String('Python'))
root.addItemToObject('skill', skill)
root.addItemToObject('student', cjson.False_())
root.print()