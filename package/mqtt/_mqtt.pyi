
class _MQTT:
    def __init__(self, ip: str, port: int, clinetID: str,
                 username: str, password: str, version: str,
                 ca: str, keepalive: int): ...

    def __del__(self): ...

    def setClientID(self, id: str) -> int:
        pass
        """Set the ClientID of the MQTTClient."""

    def setUsername(self, name: str) -> int:
        pass
        """Set the Username of the MQTTClient."""

    def setPassword(self, passwd: str) -> int:
        pass
        """Set the Password of the MQTTClient."""

    def setVersion(self, version: str) -> int:
        pass
        """Set the Version of the MQTTClient."""

    def setCa(self, ca: str) -> int:
        pass
        """Set the Ca of the MQTTClient."""

    def setKeepAlive(self, time: int) -> int:
        pass
        """Set the KeepAlive of the MQTTClient."""

    def setWill(self, topic: str, payload: str, qos: int, retain: int) -> int:
        pass
        """Set the Will of the MQTTClient."""

    def connect(self) -> int:
        pass
        """connect to the mqtt-server."""

    def disconnect(self) -> int:
        pass
        """disconnect to the mqtt-server."""

    def subscribe(self, topic: str, cb: any, qos: int) -> int:
        pass
        """subscribe to the mqtt-server."""

    def unsubscribe(self, topic: str) -> int:
        pass
        """unsubscribe to the mqtt-server."""

    def listSubscribeTopic(self) -> list:
        pass
        """listSubscribeTopic """

    def publish(self, topic: str, payload: str, qos: int) -> int:
        pass
        """publish to the mqtt-server."""

    def setPort(self, port: int) -> int:
        """Set the port num of the MQTTClient."""

    def setHost(self, host_url: str) -> int:
        """Set the host_url of the MQTTClient."""

    def setDisconnectHandler(self, cb: any) -> int:
        pass
        """set disconnect callback fun."""


def __del__():
    pass
