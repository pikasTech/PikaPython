import mqtt
import os

descriptor = os.open(".mqtt-port", os.O_RDONLY)
port = int(os.read(descriptor, 16).decode())
os.close(descriptor)
client = mqtt.MQTTClient("pika-v2-case", "127.0.0.1", port)
client.connect()
client.publish("pika/case", b"hello")
client.ping()
client.disconnect()
print("mqtt-client: connected published pinged")
