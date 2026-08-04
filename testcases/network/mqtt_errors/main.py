# SPEC: PJ2026-050109 network-ready v0.3; MQTT failure recovery.
from mqtt import MQTTClient
from network_fixture import HOST, PORT


def expect_connect_error(client_id):
    client = MQTTClient(client_id, HOST, PORT)
    try:
        client.connect()
    except ValueError as error:
        print(client_id + ": " + str(error))
    client.disconnect()


try:
    MQTTClient("bad-keepalive", HOST, PORT, keepalive=-1)
except ValueError as error:
    print("keepalive: " + str(error))

disconnected = MQTTClient("offline", HOST, PORT)
try:
    disconnected.publish("topic", b"value")
except ValueError as error:
    print("offline-publish: " + str(error))
try:
    disconnected.ping()
except ValueError as error:
    print("offline-ping: " + str(error))
disconnected.disconnect()
disconnected.disconnect()

long_client = MQTTClient("x" * 129, HOST, PORT)
try:
    long_client.connect()
except ValueError as error:
    print("client-id-limit: " + str(error))
long_client.disconnect()

expect_connect_error("refused")
expect_connect_error("invalid")
expect_connect_error("session")
expect_connect_error("truncated")

wrong_ping = MQTTClient("ping-wrong", HOST, PORT)
wrong_ping.connect()
try:
    wrong_ping.ping()
except ValueError as error:
    print("ping-wrong: " + str(error))
wrong_ping.disconnect()

limits = MQTTClient("limits", HOST, PORT)
limits.connect()
try:
    limits.publish("x" * 257, b"value")
except ValueError as error:
    print("topic-limit: " + str(error))
try:
    limits.publish("topic", b"x" * 3073)
except ValueError as error:
    print("payload-limit: " + str(error))
limits.ping()
limits.disconnect()

recovered = MQTTClient("recover", HOST, PORT)
recovered.connect()
recovered.publish("recovery", b"ok")
recovered.ping()
recovered.disconnect()
print("recovered")
