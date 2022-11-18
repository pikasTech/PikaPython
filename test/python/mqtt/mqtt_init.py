import mqtt

client = mqtt.MQTT('192.168.1.255')

client.set_host('broker.emqx.io')
client.set_port(1883)
ret = client.connect()
print('client.connect:%d' % ret)

ret = client.disconnect()
print('client.disconnect:%d' % ret)



