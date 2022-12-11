import mqtt

client = mqtt.MQTT('broker.emqx.io',port=1883,clinetID='clientid',username='name_',password='passwd_')

ret = client.connect()
print("ret:%d" % ret)

ret = client.disconnect()
print("ret:%d" % ret)
