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

# data1 = "{"data":{"name":"11"}}"
# data1 = "{"data":{"token":"3E6EA1D907B9CFEB6AB1DECB5667E4A7","resultCode":"0000"},"resultCode":"0000"}"
#data1 = '{"data":{"requestSocialInsuranceFromYangCheng":"","authenticationComparison":"no","startupLogo":"4","cardType":"00,01,02,03,04","synfromhis":"no","alarmThresholdValue":"37.2","hospitalName":"jell","facediscernMode":"01","hospitalCode":"102"},"success":true,"resultCode":"0000","time":"2022-05-20 14:10:27","message":"ok"}'
#data1 = '{"data":{"validTime":28800,"token":"3E6EA1D907B9CFEB6AB1DECB5667E4A7"},"success":true,"resultCode":"0000"}'
# data1 = "{"data":{"jjj":"","333":"no","startupLogo":"4","cardType":"00,01,02,03,04","synfromhis":"no","alarmThresholdValue":"37.2","hospitalName":"jell","facediscernMode":"01","hospitalCode":"102"},"success":true,"resultCode":"0000","time":"2022-05-20 14:10:27","message":"ok"}"
#data1 = "{\"data\":{\"requestSocialInsuranceFromYangCheng\":\"\",\"authenticationComparison\":\"no\",\"startupLogo\":\"4\",\"cardType\":\"00,01,02,03,04\",\"synfromhis\":\"no\",\"alarmThresholdValue\":\"37.2\",\"hospitalName\":\"余杭农贸市场\",\"facediscernMode\":\"01\",\"hospitalCode\":\"102\"},\"success\":true,\"resultCode\":\"0000\",\"time\":\"2022-05-20 14:10:27\",\"message\":\"操作成功\"} "
#data1 = '{"sites": [{ "name":"Google", "info":[ "Android", "Google 搜索", "Google 翻译" ] }],"arraytest":{"test1":["c", "Java", "Python"],"test2":["c2", "Java2", "Python2"]},"data":{"requestSocialInsuranceFromYangCheng":"","authenticationComparison":"no","startupLogo":"4","cardType":"00,01,02,03,04","synfromhis":"no","alarmThresholdValue":"37.2","hospitalName":"余杭农贸市场","facediscernMode":"01","hospitalCode":"102"},"success":"true","resultCode":"0000","time":"2022-05-20 14:10:27","message":"操作成功"}'
data1 = '{"sites": [{ "name":"Google", "info":[ "Android", "Google search", "Google translation" ] },{ "name":"Runoob", "info":[ "ciniao jiaoc", "ciniao tool", "ciniao wechat" ] },{ "name":"Taobao", "info":[ "taobao", "shopping" ] }],"arraytest":{"test1":["c", "Java", "Python"],"test2":["c2", "Java2", "Python2"]},"data":{"requestSocialInsuranceFromYangCheng":"","authenticationComparison":"no","startupLogo":"4","cardType":"00,01,02,03,04","synfromhis":"no","alarmThresholdValue":"37.2","hospitalName":"余杭农贸市场","facediscernMode":"01","hospitalCode":"102"},"success":"true","resultCode":"0000","time":"2022-05-20 14:10:27","message":"操作成功"}'

a = cjson.Parse(data1)
a.print()
success = a.getObjectItem("success")
success.print()
value = success.getValueString()
print(value)
data = a.getObjectItem("data")
ret = data.isNull()
if ret == 0:
    startupLogo = data.getObjectItem("startupLogo")
    startupLogo.print()
    startupLogovalue = startupLogo.getValueString()
    print(startupLogovalue)
else:
    print("data is null")

sites = a.getObjectItem("sites")
sites_num = sites.getArraySize()
for i in range(0, sites_num):
    sites_item = sites.getArrayItem(i)
    print("-------sites array ------")
    # sites_item.print()
    name = sites_item.getObjectItem("name")
    namevalue = name.getValueString()
    print(namevalue)
    print("-------sites array  info------")
    info = sites_item.getObjectItem("info")
    info_num = info.getArraySize()
    print(info_num)
    for j in range(0, info_num):
        info_item = info.getArrayItem(j)
        # print(info_item)
        # info_item.print()
        infodata = info_item.getValueString()
        print(infodata)
