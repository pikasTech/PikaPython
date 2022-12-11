import mqtt

client = mqtt.MQTT('11111',port=1883,clinetID='clientid',username='name_',password='passwd_')

ret = client.connect()
print("ret:%d" % ret)

ret = client.disconnect()
print("ret:%d" % ret)
