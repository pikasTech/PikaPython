# SPEC: PJ2026-050109 network-ready v0.3; MQTT session regression.
from mqtt import MQTTClient
from network_fixture import HOST, PORT


client = MQTTClient("direct", HOST, PORT, keepalive=30)
client.connect()
client.publish("device/status", b"ready")
client.ping()
client.disconnect()
client.disconnect()
print("direct-ok")


class DeviceClient:
    def __init__(self, host, port, topic_prefix):
        self.client = MQTTClient("device-7", host, port)
        self.topic_prefix = topic_prefix

    def connect(self):
        self.client.connect()

    def publish_state(self, state):
        self.client.publish(
            self.topic_prefix + "/state",
            state.encode(),
        )

    def keepalive(self):
        self.client.ping()

    def disconnect(self):
        self.client.disconnect()


device = DeviceClient(HOST, PORT, "fleet/node")
device.connect()
device.publish_state("ready")
device.keepalive()
device.disconnect()
print("device-ok")
