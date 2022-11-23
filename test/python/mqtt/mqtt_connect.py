import mqtt

client = mqtt.MQTT('192.168.1.255')

client.setHost('broker.emqx.io')
client.setPort(1883)
client.setClientID('123456dddecetdc')
client.setUsername('test1')
client.setPassword('aabbccdd')
client.setVersion('4')
client.setKeepAlive('10')

ret = client.connect()
print("ret:%d" % ret)

ret = client.subscribe('topic',1,'111')

#ret = client.disconnect()
#print("ret:%d" % ret)
