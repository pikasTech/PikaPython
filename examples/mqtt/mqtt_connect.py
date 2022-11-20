import mqtt

client = mqtt.MQTT('192.168.1.255')

client.setHost('test.mosquitto.org')
client.setPort(1883)

client.connect()

client.disconnect()
