import mqtt

client = mqtt.MQTT()

client.set_host('broker.emqx.io')
client.set_port(1883)
ret = client.connect()
print('client.connect:%d' % ret)

ret = client.disconnect()
print('client.disconnect:%d' % ret)



