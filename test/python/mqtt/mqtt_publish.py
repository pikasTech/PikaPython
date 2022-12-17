import mqtt

client = mqtt.MQTT('192.168.1.255')

client.setHost('broker.emqx.io')
client.setPort(1883)
client.setClientID('123456dddecetdc')
client.setUsername('test1')
client.setPassword('aabbccdd')
client.setVersion('3.1')
client.setKeepAlive(10)

ret = client.connect()
print("connect ret:%d" % ret)

client.publish('topic_pikapy',0, 'hello pikascript qos=0')
client.publish('topic_pikapy',1, 'hello pikascript qos=1')
client.publish('topic_pikapy',2, 'hello pikascript qos=2')

ret = client.disconnect()
print("disconnect ret:%d" % ret)