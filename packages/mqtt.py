# SPEC: PJ2026-050109 network-ready v0.2; pure Python MQTT 3.1.1 client.
import socket
from pika_mqtt_config import PIKA_MQTT_CLIENT_ID_LIMIT, PIKA_MQTT_PACKET_LIMIT, PIKA_MQTT_PAYLOAD_LIMIT, PIKA_MQTT_REMAINING_LENGTH_BYTES, PIKA_MQTT_TOPIC_LIMIT


def _encode_remaining_length(value):
    if value < 0 or value > 268435455:
        raise ValueError("MQTT remaining length is outside 0..268435455")
    encoded = bytearray()
    while True:
        digit = value % 128
        value = value // 128
        if value > 0:
            digit |= 128
        encoded.append(digit)
        if value == 0:
            break
    if len(encoded) > PIKA_MQTT_REMAINING_LENGTH_BYTES:
        raise ValueError("MQTT remaining length exceeds PIKA_MQTT_REMAINING_LENGTH_BYTES")
    return bytes(encoded)


def _encode_utf8(value, limit, limit_name):
    encoded = value.encode()
    if len(encoded) > limit:
        raise ValueError("MQTT field exceeds " + limit_name)
    if len(encoded) > 65535:
        raise ValueError("MQTT UTF-8 field exceeds 65535 bytes")
    return bytes([len(encoded) >> 8, len(encoded) & 255]) + encoded


def _packet(packet_type, body):
    if len(body) > PIKA_MQTT_PACKET_LIMIT:
        raise ValueError("MQTT packet exceeds PIKA_MQTT_PACKET_LIMIT")
    return bytes([packet_type]) + _encode_remaining_length(len(body)) + body


class MQTTClient:
    def __init__(self, client_id, host, port=1883, keepalive=60):
        if keepalive < 0 or keepalive > 65535:
            raise ValueError("MQTT keepalive is outside 0..65535")
        self.client_id = client_id
        self.host = host
        self.port = port
        self.keepalive = keepalive
        self.connection = None

    def _receive_exact(self, size):
        data = b""
        while len(data) < size:
            block = self.connection.recv(size - len(data))
            if len(block) == 0:
                raise ValueError("MQTT packet ended early")
            data += block
        return data

    def connect(self):
        if self.connection is not None:
            raise ValueError("MQTT client is already connected")
        client = _encode_utf8(
            self.client_id,
            PIKA_MQTT_CLIENT_ID_LIMIT,
            "PIKA_MQTT_CLIENT_ID_LIMIT",
        )
        variable = b"\x00\x04MQTT\x04\x02"
        variable += bytes([self.keepalive >> 8, self.keepalive & 255])
        connection = socket.socket()
        try:
            connection.connect((self.host, self.port))
            connection.sendall(_packet(16, variable + client))
            response = b""
            while len(response) < 4:
                block = connection.recv(4 - len(response))
                if len(block) == 0:
                    raise ValueError("MQTT CONNACK ended early")
                response += block
            if response[0] != 32 or response[1] != 2:
                raise ValueError("MQTT CONNACK packet is invalid")
            if response[2] != 0:
                raise ValueError("MQTT session-present is unsupported")
            if response[3] != 0:
                raise ValueError("MQTT CONNACK refused connection")
            self.connection = connection
        finally:
            if self.connection is None:
                connection.close()

    def publish(self, topic, payload):
        if self.connection is None:
            raise ValueError("MQTT client is not connected")
        topic_data = _encode_utf8(
            topic, PIKA_MQTT_TOPIC_LIMIT, "PIKA_MQTT_TOPIC_LIMIT"
        )
        if len(payload) > PIKA_MQTT_PAYLOAD_LIMIT:
            raise ValueError("MQTT payload exceeds PIKA_MQTT_PAYLOAD_LIMIT")
        self.connection.sendall(_packet(48, topic_data + payload))

    def ping(self):
        if self.connection is None:
            raise ValueError("MQTT client is not connected")
        self.connection.sendall(b"\xc0\x00")
        response = self._receive_exact(2)
        if response != b"\xd0\x00":
            raise ValueError("MQTT PINGRESP packet is invalid")

    def disconnect(self):
        if self.connection is None:
            return
        connection = self.connection
        self.connection = None
        try:
            connection.sendall(b"\xe0\x00")
        finally:
            connection.close()
