from mqtt import _encode_remaining_length, _encode_utf8, _packet
from pika_mqtt_config import PIKA_MQTT_TOPIC_LIMIT

print(_encode_remaining_length(0))
print(_encode_remaining_length(127))
print(_encode_remaining_length(128))
print(_encode_remaining_length(16384))
print(_encode_utf8("sensor/temp", PIKA_MQTT_TOPIC_LIMIT, "PIKA_MQTT_TOPIC_LIMIT"))
print(_packet(48, b"\x00\x01t42"))

for value in (-1, 268435456):
    try:
        _encode_remaining_length(value)
    except ValueError as error:
        print(type(error).__name__ + ": " + str(error))
