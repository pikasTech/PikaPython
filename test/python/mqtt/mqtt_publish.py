import mqtt

client = mqtt.MQTT('192.168.1.255')

client.setHost('broker.emqx.io')
client.setPort(1883)
client.setClientID('123456dddecetdc')
client.setUsername('test1')
client.setPassword('aabbccdd')
client.setVersion('3')
client.setKeepAlive('10')

ret = client.connect()
print("ret:%d" % ret)

client.publish(0,'topic_pikapy', 'hello pikascript qos=0')
client.publish(1,'topic_pikapy', 'hello pikascript qos=1')
client.publish(2,'topic_pikapy', 'hello pikascript qos=2')

ret = client.disconnect()
print("ret:%d" % ret)